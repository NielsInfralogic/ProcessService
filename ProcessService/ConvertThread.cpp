#include "StdAfx.h"
#include <afxmt.h>
#include <afxtempl.h> 
#include <direct.h>
#include "Defs.h"
#include "DatabaseManager.h"
#include "Utils.h"
#include "Prefs.h"
#include "ConvertThread.h"
#include "PDFutils.h"

extern CPrefs g_prefs;
extern CUtils g_util;
extern BOOL g_BreakSignal;
extern BOOL g_convertthreadrunning;

UINT ConvertThread(LPVOID pParam)
{
	CString sErrorMessage;
	CDatabaseManager m_convertDB;
	BOOL bReconnect = FALSE;
	BOOL bDBReconnect = FALSE;
	CString sLastError = _T("");
	::Sleep(2000);
	int nPollTime = 1;
	CJobAttributes Job;

	int nInstanceNumber = g_prefs.m_instancenumber;

	int nType = (int)pParam;  // 0: lowres, 1:print
	if (nType == 11)
	{
		nInstanceNumber += 10;
		nType = 1;
	}
	int nPDFmode = nType == 0 ? POLLINPUTTYPE_LOWRESPDF : POLLINPUTTYPE_PRINTPDF;

	BOOL m_connected = m_convertDB.InitDB(g_prefs.m_DBserver, g_prefs.m_Database, g_prefs.m_DBuser, g_prefs.m_DBpassword,
																				g_prefs.m_IntegratedSecurity, sErrorMessage);

	g_util.LogprintfConvert("INFO: Convertthread entering loop..");

	if (m_connected) {
		m_convertDB.ResetConvertStatus(nPDFmode, nInstanceNumber, sErrorMessage);
	}

	int tick = 0;
	while (g_BreakSignal == FALSE) {

		tick++;
		BOOL bGotJobs = FALSE;
		if (bDBReconnect) {
			m_convertDB.ExitDB();

			for (int i = 0; i < 16; i++) {
				::Sleep(250);
				if (g_BreakSignal)
					break;
			}
			if (g_BreakSignal)
				break;

			bDBReconnect = FALSE;

			m_connected = m_convertDB.InitDB(g_prefs.m_DBserver, g_prefs.m_Database, g_prefs.m_DBuser, g_prefs.m_DBpassword,
				g_prefs.m_IntegratedSecurity, sErrorMessage);

			if (m_connected == FALSE) {

				g_util.LogprintfConvert("ERROR: Database connection error");
				g_util.SendMail(MAILTYPE_DBERROR, sErrorMessage);
			}
		}


		for (int tm = 0; tm < 100; tm++) {

			::Sleep(10 * nPollTime);
			if (g_BreakSignal)
				break;
		}

		if (g_BreakSignal)
			break;

		g_convertthreadrunning = TRUE;

		g_util.AliveConverting();
		if ((tick) % 5 == 0 && nType == 0) {
			if (m_convertDB.UpdateService(SERVICESTATUS_RUNNING, _T(""), _T(""), sErrorMessage) == FALSE) {
				g_util.LogprintfConvert("ERROR: Database error - %s", sErrorMessage);
				bDBReconnect = TRUE;
				continue;
			}
		}


		// Run through LOWRES and PRINT convertion...
		//for (int nType = 0; nType <= 1; nType++) {
		

		if (tick % 120 == 0) {
			m_convertDB.ResetConvertStatus(nPDFmode, nInstanceNumber, sErrorMessage);
		}


		Job.m_mastercopyseparationset = 0;
		Job.m_pdfprocessID = 0;

		if (m_convertDB.AcuireConvertLock(nType, nInstanceNumber, sErrorMessage) == 0) {
			::Sleep(1000);
			// Did not get exclusive lock on - try again later
			continue;
		}


		if (m_convertDB.LookupConvertJob(Job, nPDFmode, sErrorMessage) == FALSE) {
			g_util.LogprintfConvert("ERROR: LookupConvertJob() - %s", sErrorMessage);

			bGotJobs = FALSE;
			bDBReconnect = TRUE;
			g_util.SendMail(MAILTYPE_DBERROR, "Database error - " + sErrorMessage);

			m_convertDB.ReleaseConvertLock(nType, nInstanceNumber, sErrorMessage);
			m_convertDB.UpdateService(SERVICESTATUS_HASERROR, _T(""), sErrorMessage, sErrorMessage);

			continue;
		}

		PDFPROCESSTYPE *pConvertType = g_prefs.GetPdfProcessType(Job.m_pdfprocessID);

		if (Job.m_mastercopyseparationset == 0) {
			/*if (nPDFmode == POLLINPUTTYPE_LOWRESPDF)
				g_util.LogprintfConvert("INFO: No lowres jobs found..");
			else
				g_util.LogprintfConvert("INFO: No printres jobs found..");
*/
			m_convertDB.ReleaseConvertLock(nType, nInstanceNumber, sErrorMessage);
			bGotJobs = FALSE;
			continue;
		}

		// Set status 'converting'
		m_convertDB.UpdateConvertStatus(CONVERTSTATUSNUMBER_CONVERTING, Job.m_mastercopyseparationset, nPDFmode, _T(""), Job.m_ccdatafilename, nInstanceNumber, sErrorMessage);

		m_convertDB.ReleaseConvertLock(nType, nInstanceNumber, sErrorMessage);
		// Check existence of highres file..

		if (Job.m_jobname == "")
			Job.m_jobname.Format("%s-%.2d%.2d-%s-%s-%s", g_prefs.GetPublicationName(Job.m_publicationID), Job.m_pubdate.GetMonth(), Job.m_pubdate.GetDay(),
				g_prefs.GetEditionName(Job.m_editionID), g_prefs.GetSectionName(Job.m_sectionID), Job.m_pagename);

		CString sSourcePath = g_prefs.FormCCFilesName(POLLINPUTTYPE_HIRESPDF, Job.m_mastercopyseparationset, Job.m_ccdatafilename);

		if (g_util.FileExist(sSourcePath) == FALSE) {

			bReconnect = TRUE;
			sLastError = "Error accessing Page " + Job.m_jobname + " (" + sSourcePath + ")";

			g_util.LogprintfConvert("ERROR: %s - MasterCopySepSet=%d - %s", sLastError, Job.m_mastercopyseparationset, sSourcePath);
			g_util.SendMail(MAILTYPE_PROOFINGERROR, sLastError);

			m_convertDB.UpdateConvertStatus(CONVERTSTATUSNUMBER_CONVERTERROR, Job.m_mastercopyseparationset, nPDFmode, sLastError, Job.m_ccdatafilename, nInstanceNumber, sErrorMessage);

			m_convertDB.UpdateService(SERVICESTATUS_HASERROR, _T(""), sLastError, sErrorMessage);
			m_convertDB.InsertLogEntry(nType == POLLINPUTTYPE_LOWRESPDF ? EVENTCODE_CONVERTERROR_LOWRES : EVENTCODE_CONVERTERROR_PRINT,
				nType == POLLINPUTTYPE_LOWRESPDF ? "PDF Convert lowres" : "PDF Convert print",
				Job.m_filename, sLastError, Job.m_mastercopyseparationset, Job.m_version, 0, _T(""), sErrorMessage);
			m_convertDB.ReleaseConvertLock(nType, nInstanceNumber, sErrorMessage);
			continue;
		}


		CString sDestPath = g_prefs.FormCCFilesName(nPDFmode, Job.m_mastercopyseparationset, Job.m_ccdatafilename);
		CString sConvertErrorMessage = "";
		g_util.LogprintfConvert("INFO: Starting convertion of file %s to %s (PDFmode %d) (PDFProcessID=%d)...", sSourcePath, sDestPath, nPDFmode, Job.m_pdfprocessID);

		BOOL bConvertStatus = FALSE;
		int nTryNo = 0;
		int nCRC = 0;

		int nNumberOfRetries = nPDFmode == POLLINPUTTYPE_LOWRESPDF ? 3 : 2;
		while (++nTryNo <= nNumberOfRetries) {

			bConvertStatus = ConvertPDF(nTryNo, nPDFmode, sSourcePath, sDestPath, Job.m_pdfprocessID, sConvertErrorMessage);
			if (bConvertStatus <= 0) {
				::Sleep(1000);
				g_util.LogprintfConvert("ERROR: Try %d - Error converting file %s to %s (PDFmode %d) - %s", nTryNo, sSourcePath, sDestPath, nPDFmode, sConvertErrorMessage);
				continue;
			}

			g_util.LogprintfConvert("INFO: Successfully converted file %s to %s (PDFmode %d)", sSourcePath, sDestPath, nPDFmode);
			// Calculate CR
			nCRC = g_util.CRC32Convert(sDestPath);

			g_util.LogprintfConvert("INFO: CRC = %d for converted file %s", nCRC, sDestPath);
			// Retry reading checksum..
			if (nCRC == 0) {
				::Sleep(2500);
				g_util.LogprintfConvert("ERROR: Try %d - No Checksum for file %s to %s (PDFmode %d) - %s", nTryNo, sSourcePath, sDestPath, nPDFmode, sConvertErrorMessage);
				continue;
			}

			break;
		}


		if (bConvertStatus > 0) {

			if (nPDFmode == POLLINPUTTYPE_LOWRESPDF && g_prefs.m_trimpdf) {
				CString sTempFileTrimIn = g_util.GetTempFolder() + _T("\\trimin_") + +g_util.GenerateTimeStamp() + _T("_") + g_util.GetFileName(sDestPath) + g_util.GenerateTimeStamp();
				CString sTempFileTrimOut = g_util.GetTempFolder() + _T("\\trimout_") + +g_util.GenerateTimeStamp() + _T("_") + g_util.GetFileName(sDestPath) + g_util.GenerateTimeStamp();

				if (::CopyFile(sDestPath, sTempFileTrimIn, FALSE) == FALSE) {
					::Sleep(1000);
					::CopyFile(sDestPath, sTempFileTrimIn, FALSE);
				}

				if (TrimPage(sTempFileTrimIn, sTempFileTrimOut, sErrorMessage) == 1) {
					g_util.LogprintfConvert("INFO: Page %s was trimmed!", sTempFileTrimIn);
					if (g_util.FileExist(sTempFileTrimOut)) {

						if (::CopyFile(sTempFileTrimOut, sDestPath, FALSE) == FALSE)
						{
							::Sleep(1000);
							::CopyFile(sTempFileTrimOut, sDestPath, FALSE);
						}
					}
				}
				::DeleteFile(sTempFileTrimIn);
				::DeleteFile(sTempFileTrimOut);
				nCRC = g_util.CRC32Convert(sDestPath);

			}

			m_convertDB.UpdateConvertCRC(nCRC, Job.m_mastercopyseparationset, nPDFmode, sErrorMessage);
			m_convertDB.UpdateConvertStatus(CONVERTSTATUSNUMBER_CONVERTED, Job.m_mastercopyseparationset, nPDFmode, _T(""), Job.m_ccdatafilename, nInstanceNumber, sErrorMessage);
			m_convertDB.InsertLogEntry(nType == POLLINPUTTYPE_LOWRESPDF ? EVENTCODE_CONVERTED_LOWRES : EVENTCODE_CONVERTED_PRINT,
				nType == POLLINPUTTYPE_LOWRESPDF ? "PDF Convert lowres" : "PDF Convert print",
				Job.m_filename, "Ok", Job.m_mastercopyseparationset, Job.m_version, 0, _T(""), sErrorMessage);

			// Set status for each channel
		}
		else {

			g_util.LogprintfConvert("ERROR: Error converting file %s to %s (PDFmode %d) - %s", sSourcePath, sDestPath, nPDFmode, sConvertErrorMessage);
			m_convertDB.UpdateConvertStatus(CONVERTSTATUSNUMBER_CONVERTERROR, Job.m_mastercopyseparationset, nPDFmode, sConvertErrorMessage, Job.m_ccdatafilename, nInstanceNumber, sErrorMessage);
			m_convertDB.InsertLogEntry(nType == POLLINPUTTYPE_LOWRESPDF ? EVENTCODE_CONVERTERROR_LOWRES : EVENTCODE_CONVERTERROR_PRINT,
				nType == POLLINPUTTYPE_LOWRESPDF ? "PDF Convert lowres" : "PDF Convert print",
				Job.m_filename, sConvertErrorMessage, Job.m_mastercopyseparationset, Job.m_version, 0, _T(""), sErrorMessage);
			
			if (g_prefs.m_generatestatusmessage && g_prefs.m_messageonerror) {
				int nTypeNumber = 0;
				if (nPDFmode == POLLINPUTTYPE_PRINTPDF)
					nTypeNumber = 1014;
				else if (nPDFmode == POLLINPUTTYPE_HIRESPDF)
					nTypeNumber = 1015;
				else if (nPDFmode == POLLINPUTTYPE_LOWRESPDF) // VisioLink only!! 
					nTypeNumber = 1013;

				g_util.IssueMessage(Job.m_filename, CONVERTSTATUSNUMBER_CONVERTERROR, g_prefs.m_messageoutputfolder, nTypeNumber, sErrorMessage);
			}
		}

		m_convertDB.ReleaseConvertLock(nType, nInstanceNumber, sErrorMessage);
		//	}

	}

	g_convertthreadrunning = FALSE;
	m_convertDB.UpdateService(SERVICESTATUS_STOPPED, _T(""), _T(""), sErrorMessage);
	m_convertDB.ExitDB();

	g_util.LogprintfConvert("INFO: ConvertThread() terminated");

	return 0;

}


int ConvertPDF(int nTryNo, int nPDFmode, CString sSourcePath, CString sDestPath, int nPdfProcessID, CString &sErrorMessage)
{
	sErrorMessage = _T("");
	CString sProfile = (nPDFmode == POLLINPUTTYPE_LOWRESPDF) ? g_prefs.m_callasProfileLowres : g_prefs.m_callasProfileCMYK;
	CString sProfilePath = g_util.GetFilePath(g_prefs.m_callasProfileLowres);
	BOOL bConvertExternally = FALSE;
	if (nPdfProcessID > 0) {

		PDFPROCESSTYPE *pThisPdfProceeType = g_prefs.GetPdfProcessType(nPdfProcessID);
		if (pThisPdfProceeType != NULL) {
			// pThisPdfProceeType->m_ProcessType == 0 _> Lowres!!
			// pThisPdfProceeType->m_ProcessType == 1 _> Print!!
			// Ensure LOW-LOW or PRINT-PRINT-..
			if ((nPDFmode  == POLLINPUTTYPE_LOWRESPDF && pThisPdfProceeType->m_ProcessType == 0) || (nPDFmode == POLLINPUTTYPE_PRINTPDF && pThisPdfProceeType->m_ProcessType == 1)) {
				
				sProfile = sProfilePath + _T("\\") + pThisPdfProceeType->m_ConvertProfile;
				bConvertExternally = pThisPdfProceeType->m_ExternalProcess;
			}
		}

	}

	if (bConvertExternally) 
		return RunExternalConvert(nTryNo, sSourcePath, sDestPath, nPdfProcessID, sErrorMessage);
	

	// not externally processed...

	// catch misconfigs--
	if (g_util.FileExist(sProfile) == FALSE)
		sProfile = (nPDFmode == POLLINPUTTYPE_LOWRESPDF) ? g_prefs.m_callasProfileLowres : g_prefs.m_callasProfileCMYK;

	DWORD dwExitCode = 0;
	int nLogStatusOK = 0;
	CString sLogFileErrorMessage = _T("");

	CString sTempFolder = g_util.GetTempFolder();
	if (g_prefs.m_callastempfolder != "")
		sTempFolder = g_prefs.m_callastempfolder;

	CString sReportPath = sTempFolder + _T("\\callasreport-") + g_util.GenerateTimeStamp() + _T(".xml");
	CString sLogPath = sTempFolder + _T("\\callaslog-") + g_util.GenerateTimeStamp() + _T(".xml");
	BOOL bFileOutputOK = FALSE;
	CString sDestTempPath = sTempFolder + _T("\\") + g_util.GetFileName(sDestPath, TRUE) + _T("-") + g_util.GenerateTimeStamp() + _T(".pdf");

	::DeleteFile(sLogPath);
	::DeleteFile(sReportPath);
	::DeleteFile(sDestTempPath);

	CString sReportSummary;

	BOOL nFireCmdRet = RunCallasProfile(sProfile, sSourcePath, sDestTempPath, sReportPath, sLogPath,
		dwExitCode, nLogStatusOK, sLogFileErrorMessage, bFileOutputOK);


	int nRetries = 10;
	while (--nRetries >= 0 && g_util.FileExist(sDestTempPath) == FALSE) {
		::Sleep(2000);
		// retry
		nFireCmdRet = RunCallasProfile(sProfile, sSourcePath, sDestTempPath, sReportPath, sLogPath,
											dwExitCode, nLogStatusOK, sLogFileErrorMessage, bFileOutputOK);
	}

	if (g_util.FileExist(sDestTempPath) == FALSE)
		bFileOutputOK = FALSE;

	if (nFireCmdRet > 0 && bFileOutputOK) {
		::DeleteFile(sDestPath);
		::CopyFile(sDestTempPath, sDestPath, FALSE);
	}

	if (nFireCmdRet <= 0) {
		sErrorMessage = nFireCmdRet == -1 ? _T("Timeout waiting for fixup command") : _T("Error processing file (convert) " + sProfile);
		bFileOutputOK = FALSE;
	}

	::DeleteFile(sLogPath);
	::DeleteFile(sReportPath);
	::DeleteFile(sDestTempPath);

	g_util.LogprintfConvert(_T("INFO: fixup command returned %d"), nFireCmdRet);

	return bFileOutputOK;
}

int RunCallasProfile(CString sConfigFile, CString sInputPath, CString sOutputPath, CString sReportPath, CString sLogPath,
	DWORD &dwExitCode, int &nLogStatusOK, CString &sLogFileErrorMessage, BOOL &bFileOutputOK)
{
	TCHAR szBuf[512];
	DWORD dwProcessID = 0;

	CString	sReportFlags = _T("xml");

	sLogFileErrorMessage = _T("");
	nLogStatusOK = FALSE;
	bFileOutputOK = FALSE;

	::DeleteFile(sOutputPath);
	::DeleteFile(sLogPath);
	::DeleteFile(sReportPath);

	::GetCurrentDirectory(MAX_PATH, szBuf);
	::SetCurrentDirectory(g_util.GetFilePath(g_prefs.m_callascommandfilepath));

	if (g_util.FileExist(sInputPath) == FALSE) {
		g_util.LogprintfConvert(_T("ERROR:  DoCreateProcess() - input file %s missing !"), sInputPath);
	}
	if (g_util.FileExist(sConfigFile) == FALSE) {
		g_util.LogprintfConvert(_T("ERROR:  DoCreateProcess() - config file %s missing !"), sConfigFile);
	}

	CString sCmd;
	sCmd.Format(_T("\"%s\" %s %s \"%s\" \"%s\" \"%s\" \"%s\" %d \"%s\" xml en"),
		g_prefs.m_callascommandfilepath,
		g_prefs.m_callaskeyCode,
		g_prefs.m_callaskeyCode2,
		sConfigFile,
		sInputPath,
		sOutputPath,
		sReportPath,
		0,
		sLogPath);

	CString strResult = _T("");
	g_util.LogprintfConvert(_T("INFO:  Calling DoCreateProcess(%s)..."), sCmd);
	CTime tStart = CTime::GetCurrentTime();

	int nRet = g_util.DoCreateProcess(sCmd, g_prefs.m_processtimeout, TRUE, dwExitCode, dwProcessID);
	// -1:Timeout
	// 1: OK
	// 0: Error

	CTime tEnd = CTime::GetCurrentTime();
	CTimeSpan ts = tEnd - tStart;
	int nTimeTaken = (int)ts.GetTotalSeconds();
	g_util.LogprintfConvert(_T("INFO:  DoCreateProcess() returned %d - Time: %d sec."), nRet, nTimeTaken);

	::SetCurrentDirectory(szBuf);


	if (nRet == -1 && dwProcessID > 0)
		g_util.KillProcess(dwProcessID);

	if (nRet != 0) {
		if (g_util.FileExist(sLogPath))
			nLogStatusOK = g_util.GetLogFileStatus(sLogPath, sLogFileErrorMessage); //, bHasErrorSeverityHit, sHitMessage);

		bFileOutputOK = g_util.FileExist(sOutputPath) && g_util.GetFileSize(sOutputPath) > 0;
	}

	CString sInfo;
	sInfo.Format(_T("%s - %s (%s) %d sec - status: %s/%s"), g_util.GetFileName(sInputPath), g_util.GetFileName(sConfigFile), _T(""), nTimeTaken, nLogStatusOK ? _T("OK") : _T("Error"), bFileOutputOK ? _T("OK") : _T("No output file"));
	//AddDebugLogMessage(sInfo);

	return nRet;
}


static int g_nJobCounter = 0;

// Specials:
// on try 2 - send directly to CMYK folder 
// on try 2 - cut timeout in half..

int  RunExternalConvert(int nTryNo, CString sSourceFile, CString sDestinationFile, int nPdfProcessID, CString &sErrorMessage)
{
	CString sCmd;
	sErrorMessage = _T("");
	g_util.LogprintfConvert("INFO:  Converting pdf externally..%s (try %d)", sSourceFile, nTryNo);

	::DeleteFile(sDestinationFile);

	CString sExternalInputFolder = _T("");
	CString sExternalOutputFolder = _T("");
	CString sExternalErrorFolder = _T("");
	int nTimeOut = 720;

	if (nPdfProcessID > 0) {

		PDFPROCESSTYPE *pThisPdfProceeType = g_prefs.GetPdfProcessType(nPdfProcessID);
		if (pThisPdfProceeType != NULL) {
			sExternalInputFolder = pThisPdfProceeType->m_ExternalInputFolder;
			sExternalOutputFolder = pThisPdfProceeType->m_ExternalOutputFolder;
			sExternalErrorFolder = pThisPdfProceeType->m_ExternalErrorFolder;
			nTimeOut = pThisPdfProceeType->m_ProcessTimeOut;
		}

	}


	CString gsFileID;
	gsFileID.Format("{%s%.5d}", g_util.GenerateTimeStamp(), ++g_nJobCounter);

	// Special hack to bypass image enhancement due to timeout on previous attempt..
	sExternalInputFolder.MakeUpper();
	if (nTryNo == 2 && sExternalInputFolder.Find("\\ENHANCE") != -1)
		sExternalInputFolder.Replace("\\ENHANCE", "\\CMYK");

	CString sTmpName = g_util.GetFileName(sSourceFile, TRUE) + _T("_") + gsFileID + _T(".") + g_util.GetExtension(sSourceFile);
	CString sInputFile = sExternalInputFolder + _T("\\") + sTmpName;
	CString sOutputFile = sExternalOutputFolder + _T("\\") + sTmpName;
	CString sErrorFile = sExternalErrorFolder + _T("\\") + sTmpName;
	::DeleteFile(sOutputFile);
	::DeleteFile(sErrorFile);
	
	// Check if file already in input queue..
	// "APFYZ1_20191115_E1_1_1_C4_011#1682_{2019112613005000004}.PDF";
	// If so - delete and replace with newer file..
	CString sExistingFileInQueue = g_util.GetFirstFile(sExternalInputFolder, g_util.GetFileName(sSourceFile, TRUE) + +_T("_{*}.") + g_util.GetExtension(sSourceFile));
	if (sExistingFileInQueue != _T(""))
	{
		g_util.LogprintfConvert("WARNING: Deleting previous version of file %s from external input folder", sExistingFileInQueue, sExternalInputFolder);
		::DeleteFile(sExistingFileInQueue);
	}


	g_util.LogprintfConvert("INFO:  Converting pdf document %s to %s externally..", sInputFile, sOutputFile);

	::CopyFile(sSourceFile, sInputFile, FALSE);
	::Sleep(1000);

	BOOL bFoundInErrorFolder = FALSE;
	BOOL bFileLost = FALSE;

	if (nTimeOut <= 0)
		nTimeOut = g_prefs.m_processtimeout; // default if now provided from setup
	
	if (nTryNo == 2)
		nTimeOut /= 4;	// Only wait half as long second try..

	// Wait for output..(or error)
	int nProgress = 10;
	int nSecondsGone = 0;
	while (nTimeOut-- > 0 && g_BreakSignal == FALSE) {

		if (g_util.FileExist(sErrorFile)) {
			bFoundInErrorFolder = TRUE;
			break;
		}

		if (g_util.FileExist(sOutputFile))
		{
			if (g_prefs.m_externalProcessStableTime > 0)
				::Sleep(1000 * g_prefs.m_externalProcessStableTime);
			break;
		}

		::Sleep(1000);
		nSecondsGone++;

		// For CMYK queues - if file not in error,output or input - it has been killed.
		if (nSecondsGone >= 10 && g_util.FileExist(sInputFile) == FALSE && sExternalInputFolder.Find("CMYK") != -1)
		{
			if (g_util.FileExist(sOutputFile) == FALSE && g_util.FileExist(sErrorFile) == FALSE) {
				g_util.LogprintfConvert("WARNING: RunExternalConvert - file lost?  %s", sInputFile);
				//bFileLost = TRUE;
				//	break;
			}
			
		}
	}

	if (bFileLost) {
		g_util.LogprintfConvert("ERROR: RunExternalConvert failed - file lost - %s", sInputFile);
		::DeleteFile(sErrorFile);
		::DeleteFile(sInputFile);
		return FALSE;
	}


	if (bFoundInErrorFolder) {
		g_util.LogprintfConvert("ERROR: RunExternalConvert failed - file found in error folder - %s", sErrorFile);
		::DeleteFile(sErrorFile);
		::DeleteFile(sInputFile);
		return FALSE;
	}

	if (g_util.FileExist(sOutputFile) == FALSE) {
		g_util.LogprintfResample("ERROR: RunExternalConvert failed - %s not found in output folder (timeout)", sOutputFile);
		::DeleteFile(sErrorFile);
		::DeleteFile(sInputFile);
		return FALSE;
	}

	g_util.LogprintfConvert("INFO: RunExternalConvert() - pdf document %s converted successfully", g_util.GetFileName(sSourceFile));
	BOOL ret = g_util.CopyFileWithRetry(sOutputFile, sDestinationFile, sErrorMessage);
	::DeleteFile(sOutputFile);
	if (ret == FALSE)
		g_util.LogprintfConvert("ERROR: RunExternalConvert() - Error copying %s to %s - %s", sOutputFile, sDestinationFile, sErrorMessage);
	return ret;
}
