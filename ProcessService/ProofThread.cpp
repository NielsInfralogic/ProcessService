#include "StdAfx.h"
#include <afxmt.h>
#include <afxtempl.h> 
#include <direct.h>
#include "Defs.h"
#include "DatabaseManager.h"
#include "Utils.h"
#include "Prefs.h"
#include "TiffUtils.h"
#include "icc.h"
#include "ProofThread.h"
#include "ConvertThread.h"
#include "tiffutils.h"

extern CPrefs g_prefs;
extern CUtils g_util;
extern BOOL g_BreakSignal;

BOOL g_proofthreadrunning = FALSE;
CWinThread *g_pProofThread = NULL;

BOOL g_convertthreadrunning = FALSE;
CWinThread *g_pConvertThread = NULL;
CWinThread *g_pConvertThread2 = NULL;
CWinThread *g_pConvertThread3 = NULL;
BOOL StartAllThreads()
{
	// Ini file IS loaded - startup all threads..
	CString sErrorMessage;

	// The thread loads database names etc..
	g_util.LogprintfResample("INFO: Starting proof thread..");
	g_pProofThread = AfxBeginThread(ProofThread, (LPVOID)NULL);
	g_util.LogprintfResample("INFO: Starting lowresconvert thread..");
	g_pConvertThread = AfxBeginThread(ConvertThread, (LPVOID)0);
	g_util.LogprintfResample("INFO: Starting highresconvert thread..");
	g_pConvertThread2 = AfxBeginThread(ConvertThread, (LPVOID)1);

	if (g_prefs.m_extraConvertTread) {
		g_util.LogprintfResample("INFO: Starting highresconvert thread 2..");
		g_pConvertThread3 = AfxBeginThread(ConvertThread, (LPVOID)11);
	}

	// Hang around...
	while (g_BreakSignal == FALSE) {
		::Sleep(500);
	}

	return TRUE;

}



UINT ProofThread(LPVOID nothing)
{
	CString sString, s;
	CString sErrorMessage;

	CJobAttributes Job;
	
	CDatabaseManager m_proofDB;
	BOOL bReconnect = FALSE;
	BOOL bDBReconnect = FALSE;
	CString sLastError = _T("");
	int nPollTime = 1;
	BOOL bFirstTime = TRUE;

	BOOL m_connected = m_proofDB.InitDB(g_prefs.m_DBserver, g_prefs.m_Database, g_prefs.m_DBuser, g_prefs.m_DBpassword,
												g_prefs.m_IntegratedSecurity, sErrorMessage);

	if (m_connected) {

		if (m_proofDB.LoadAllPrefs(sErrorMessage) == FALSE) {
			g_util.LogprintfResample("ERROR: LoadAllPrefs() returned - %s", sErrorMessage);
		}
		if (m_proofDB.RegisterService(sErrorMessage) == FALSE) {
			g_util.LogprintfResample("ERROR: RegisterService() returned - %s", sErrorMessage);
		}
	}
	else
		g_util.LogprintfResample("ERROR: InitDB() returned - %s", sErrorMessage);


	if (g_util.CheckFolder(g_prefs.m_hiresPDFPath) == FALSE) {
		g_util.LogprintfResample("ERROR: Proofthread cannot connect to %s", g_prefs.m_hiresPDFPath);

		m_proofDB.UpdateService(SERVICESTATUS_HASERROR, _T(""), _T("Storage folder connection error"), sErrorMessage);

		g_proofthreadrunning = FALSE;
		//return FALSE;
	}

	g_util.LogprintfResample("INFO: Proofthread entering loop..");

	int tick = 0;
	while (g_BreakSignal == FALSE) {

		tick++;
		BOOL bGotJobs = FALSE;
		if (bDBReconnect) {
			m_proofDB.ExitDB();

			for (int i = 0; i < 16; i++) {
				::Sleep(250);
				if (g_BreakSignal)
					break;
			}
			if (g_BreakSignal)
				break;

			bDBReconnect = FALSE;

			m_connected = m_proofDB.InitDB(g_prefs.m_DBserver, g_prefs.m_Database, g_prefs.m_DBuser, g_prefs.m_DBpassword,
																	g_prefs.m_IntegratedSecurity, sErrorMessage);

			if (m_connected == FALSE) {

				g_util.LogprintfResample("ERROR: Database connection error");
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

		g_proofthreadrunning = TRUE;

		g_util.AliveResampling();

		if ((tick) % 5 == 0) {
			if (m_proofDB.UpdateService(SERVICESTATUS_RUNNING, _T(""), _T(""), sErrorMessage) == FALSE) {
				g_util.LogprintfResample("ERROR: Database error - %s", sErrorMessage);
				bDBReconnect = TRUE;
				continue;
			}
		}


		if (tick % 120 == 0)
			m_proofDB.ResetProofStatus(sErrorMessage);

		if (m_proofDB.AcuireProofLock(sErrorMessage) == 0) {
			::Sleep(1000);
			// Did not get exclusive lock on - try again later
			continue;
		}


		if (bGotJobs == FALSE) {
			for (int i = 0; i < g_prefs.m_sleeptimebewteenresampling * 5; i++) {
				Sleep(100);
				if (g_BreakSignal)
					break;
			}
			if (g_BreakSignal) {
				m_proofDB.ReleaseProofLock(sErrorMessage);
				break;
			}
			for (int i = 0; i < g_prefs.m_sleeptimebewteenresampling * 5; i++) {
				Sleep(100);
				if (g_BreakSignal) {
					m_proofDB.ReleaseProofLock(sErrorMessage);
					break;
				}
			}
			if (g_BreakSignal) {
				m_proofDB.ReleaseProofLock(sErrorMessage);
				break;
			}
		}
		else
			Sleep(500);

		Job.Init();
		Job.m_mastercopyseparationset = 0;
		int nPDFType = POLLINPUTTYPE_HIRESPDF;
		BOOL bFilesPresent = TRUE;
		int nMasterSetToAvoid = 0;

		//g_util.LogprintfResample("INFO: Calling LookupProofJob..");

		if (m_proofDB.LookupProofJob(Job, nPDFType, sErrorMessage) == FALSE) {

			g_util.LogprintfResample("ERROR: LookupProofJob() - %s", sErrorMessage);

			bGotJobs = FALSE;
			bDBReconnect = TRUE;
			g_util.SendMail(MAILTYPE_DBERROR, "Database error - " + sErrorMessage);

			m_proofDB.ReleaseProofLock(sErrorMessage);
			m_proofDB.UpdateService(SERVICESTATUS_HASERROR, _T(""), sErrorMessage, sErrorMessage);

			continue;
		}
		if (Job.m_mastercopyseparationset == 0) {
			//g_util.LogprintfResample("INFO: No jobs found..");
			// No pages to proof - hang around..
			//bGotJobs = FALSE;
		//	util.LogprintfResample("DEBUG: LookupProofJob() returned - no jobs found ColorsOnPage=%d, MCSS=%d",nColorsOnPage, Job.m_mastercopyseparationset );
			m_proofDB.ReleaseProofLock(sErrorMessage);
			bGotJobs = FALSE;
			continue;
		}

		g_util.LogprintfResample("DEBUG: GOT JOB mastercolorseparationset=%d, PDFType=%d, Filename=%s", Job.m_mastercopyseparationset, nPDFType,Job.m_ccdatafilename);

		PROOFPROCESSSTRUCT *pProofTemplate = g_prefs.GetProofStruct(Job.m_prooftemplateID);
		if (pProofTemplate == NULL) {
			m_proofDB.ReleaseProofLock(sErrorMessage);
			bGotJobs = FALSE;
			continue;
		}

		///////////////////////////////////////////////
		// Proof job found
		///////////////////////////////////////////////

		bGotJobs = TRUE;

		if (Job.m_jobname == "")
			Job.m_jobname.Format("%s-%.2d%.2d-%s-%s-%s", g_prefs.GetPublicationName(Job.m_publicationID), Job.m_pubdate.GetMonth(), Job.m_pubdate.GetDay(),
				g_prefs.GetEditionName(Job.m_editionID), g_prefs.GetSectionName(Job.m_sectionID), Job.m_pagename);

		CString sDestPath = g_prefs.FormCCFilesName(nPDFType, Job.m_mastercopyseparationset, Job.m_ccdatafilename);

		CString s, sLastError;
		BOOL bUpdateOK = FALSE;
		bReconnect = FALSE;
		if (g_util.FileExist(sDestPath) == FALSE) {
			bReconnect = TRUE;
			sLastError = "Error accessing Page " + Job.m_jobname;

			bFilesPresent = FALSE;
			nMasterSetToAvoid = Job.m_mastercopyseparationset;
			g_util.LogprintfResample("ERROR: %s - MasterCopySepSet=%d - %s", sLastError, Job.m_mastercopyseparationset, sDestPath);
			g_util.SendMail(MAILTYPE_PROOFINGERROR, sLastError);

			m_proofDB.UpdateService(SERVICESTATUS_HASERROR, _T(""), sLastError, sErrorMessage);
			m_proofDB.InsertLogEntry(EVENTCODE_PROOFERROR, pProofTemplate->m_proofname, Job.m_filename, sLastError, Job.m_mastercopyseparationset, Job.m_version, 0, _T(""), sErrorMessage);


			if (m_proofDB.UpdateStatusProof(Job.m_mastercopyseparationset, PROOFSTATUSNUMBER_PROOFERROR, sLastError, bUpdateOK, sErrorMessage) == FALSE) {

				g_util.LogprintfResample("ERROR: db.UpdateStatusProof() - %s", sErrorMessage);
				bGotJobs = FALSE;
				bDBReconnect = TRUE;
				m_proofDB.ReleaseProofLock(sErrorMessage);
				m_proofDB.UpdateService(SERVICESTATUS_HASERROR, _T(""), sErrorMessage, sErrorMessage);
				break;
			}

			bUpdateOK = TRUE;
		}

		if (bReconnect) {
			m_proofDB.ReleaseProofLock(sErrorMessage);
			Sleep(1000);
			continue;
		}

		nMasterSetToAvoid = 0;
		BOOL bHasPage = FALSE;
		m_proofDB.GetPolledStatus(Job.m_mastercopyseparationset, bHasPage, sErrorMessage);
		if (bHasPage == FALSE) {
			// Abort - polling has occured while analyzing this file
			m_proofDB.ReleaseProofLock(sErrorMessage);
			g_util.LogprintfResample("WARNING:  Aborting job mastercopyseparationset=%d because input is in progress to this page.", Job.m_mastercopyseparationset);
			continue;
		}

		if (m_proofDB.UpdateStatusProof(Job.m_mastercopyseparationset, PROOFSTATUSNUMBER_PROOFING, _T(""), bUpdateOK, sErrorMessage) == FALSE) {

			g_util.LogprintfResample("ERROR: UpdateStatusProof() - %s", sErrorMessage);

			bGotJobs = FALSE;
			bDBReconnect = TRUE;
			m_proofDB.ReleaseProofLock(sErrorMessage);
			m_proofDB.UpdateService(SERVICESTATUS_HASERROR, _T(""), sErrorMessage, sErrorMessage);
			continue;
		}


		bUpdateOK = TRUE;

		m_proofDB.ReleaseProofLock(sErrorMessage);

		int bSuccess = FALSE;
		int bSuccess2 = TRUE;
		int bFinalSuccess = FALSE;

		CString sJPGname;
		sJPGname.Format("%d.jpg", Job.m_mastercopyseparationset);
		
		CString  sProofMsg = _T("");

		CString sPubName = g_prefs.GetPublicationName(Job.m_publicationID);
		BOOL bFileDeleteSuccess = TRUE;
		g_util.DeleteOldWebFiles(Job.m_mastercopyseparationset, Job.m_filename, pProofTemplate->m_generateforflash);

		g_util.LogprintfResample("DEBUG: Processing PDF job..");

		CString sTitle;
		sTitle.Format("##%s#%.4d%.2d%.2d#%s#%s#%s##", sPubName, Job.m_pubdate.GetYear(), Job.m_pubdate.GetMonth(), Job.m_pubdate.GetDay(), g_prefs.GetEditionName(Job.m_editionID), g_prefs.GetSectionName(Job.m_sectionID), Job.m_pagename);
		switch (g_prefs.m_pdfproofmethod) {
		case PDFPROOFMETHOD_GHOSTSCRIPT:
			bSuccess = GhostPDF(sDestPath, sJPGname, pProofTemplate->output.m_jpegquality, (int)pProofTemplate->resample.m_resolution, (int)pProofTemplate->output.m_thumbnailresolution, TRUE, pProofTemplate, sTitle);
			if (bSuccess == FALSE) {
				::Sleep(1000);
				if (bSuccess == FALSE) {
					bSuccess = GhostPDF(sDestPath, sJPGname, pProofTemplate->output.m_jpegquality, (int)pProofTemplate->resample.m_resolution, (int)pProofTemplate->output.m_thumbnailresolution, TRUE, pProofTemplate, sTitle);
					CString sPageTitle;
					sPageTitle.Format("%s %.4d%.2d%.2d %s %s %s", sPubName, Job.m_pubdate.GetYear(), Job.m_pubdate.GetMonth(), Job.m_pubdate.GetDay(), g_prefs.GetEditionName(Job.m_editionID), g_prefs.GetSectionName(Job.m_sectionID), Job.m_pagename);
					g_util.SendMail("Ghostscript proof error " + sPageTitle, "Ghostscript errored on job\r\n" + sPageTitle);
				}
			}
			g_util.LogprintfResample("DEBUG: Ghostscript returned code = %d", bSuccess);
			break;
		case PDFPROOFMETHOD_EXTERNALRIP:
			bSuccess = PDF2RIP(sDestPath, sJPGname, pProofTemplate->output.m_jpegquality, (int)pProofTemplate->resample.m_resolution, (int)pProofTemplate->output.m_thumbnailresolution, pProofTemplate, sTitle);
			if (bSuccess == FALSE) {
				::Sleep(1000);
				bSuccess = PDF2RIP(sDestPath, sJPGname, pProofTemplate->output.m_jpegquality, (int)pProofTemplate->resample.m_resolution, (int)pProofTemplate->output.m_thumbnailresolution, pProofTemplate, sTitle);
				if (bSuccess == FALSE) {
					CString sPageTitle;
					sPageTitle.Format("%s %.4d%.2d%.2d %s %s %s", sPubName, Job.m_pubdate.GetYear(), Job.m_pubdate.GetMonth(), Job.m_pubdate.GetDay(), g_prefs.GetEditionName(Job.m_editionID), g_prefs.GetSectionName(Job.m_sectionID), Job.m_pagename);
					g_util.SendMail("Ghostscript proof error " + sPageTitle, "RIP errored on job\r\n" + sPageTitle);
				}
			}
			g_util.LogprintfResample("DEBUG: PDF2RIP returned code = %d", bSuccess);
			break;
		case PDFPROOFMETHOD_CALLAS:
			bSuccess = ProofCallas(sDestPath, sJPGname, pProofTemplate->output.m_jpegquality, (int)pProofTemplate->resample.m_resolution, (int)pProofTemplate->output.m_thumbnailresolution, pProofTemplate, sTitle, sErrorMessage);
			if (bSuccess == FALSE) {
				::Sleep(1005);
				bSuccess = ProofCallas(sDestPath, sJPGname, pProofTemplate->output.m_jpegquality, (int)pProofTemplate->resample.m_resolution, (int)pProofTemplate->output.m_thumbnailresolution, pProofTemplate, sTitle, sErrorMessage);
				if (bSuccess == FALSE) {
					::Sleep(1000);
					bSuccess = GhostPDF(sDestPath, sJPGname, pProofTemplate->output.m_jpegquality, (int)pProofTemplate->resample.m_resolution, (int)pProofTemplate->output.m_thumbnailresolution, TRUE, pProofTemplate, sTitle);
					if (bSuccess == FALSE) {

						CString sPageTitle;
						sPageTitle.Format("%s %.4d%.2d%.2d %s %s %s", sPubName, Job.m_pubdate.GetYear(), Job.m_pubdate.GetMonth(), Job.m_pubdate.GetDay(), g_prefs.GetEditionName(Job.m_editionID), g_prefs.GetSectionName(Job.m_sectionID), Job.m_pagename);
						g_util.SendMail("ProofCallas proof error " + sPageTitle, "RIP errored on job\r\n" + sPageTitle + "\r\n" + sErrorMessage);
					}
				}
			}
			g_util.LogprintfResample("DEBUG: ProofCallas returned code = %d", bSuccess);
			break;
		}

		if (bSuccess) {
			int nThumbnailSize = g_util.GetFileSize(g_prefs.m_thumbnailPath + sJPGname);

			if (g_prefs.m_copyToWeb) {
				CString sWebpath, sSourcePath;
				CString sWebRoot = g_prefs.m_webPath;

				g_util.Reconnect(g_prefs.m_webPath, g_prefs.m_usecurrentuserweb ? g_prefs.m_webFTPuser : _T(""), g_prefs.m_usecurrentuserweb ? g_prefs.m_webFTPpassword : _T(""));

				if (sWebRoot.Right(1) != "/" || sWebRoot.Right(1) != "\\")
					sWebRoot += _T("/");
				sSourcePath.Format(_T("%s%d.jpg"), g_prefs.m_previewPath, Job.m_mastercopyseparationset);
				sWebpath.Format(_T("%sCCpreviews\\%d.jpg"), sWebRoot, Job.m_mastercopyseparationset);
				::CopyFile(sSourcePath, sWebpath, 0);
				sSourcePath.Format(_T("%s%d.jpg"), g_prefs.m_thumbnailPath, Job.m_mastercopyseparationset);
				sWebpath.Format(_T("%sCCthumbnails\\%d.jpg"), sWebRoot, Job.m_mastercopyseparationset);
				::CopyFile(sSourcePath, sWebpath, 0);

				if (pProofTemplate->m_generateforflash == 1) { // Generated locally..

					if (g_prefs.m_webversionpreviews)
						sWebpath.Format(_T("%sCCpreviews\\%d-%d"), sWebRoot, Job.m_mastercopyseparationset, Job.m_version);
					else
						sWebpath.Format(_T("%sCCpreviews\\%d"), sWebRoot, Job.m_mastercopyseparationset);
					::CreateDirectory(sWebpath, NULL);

					sSourcePath.Format(_T("%s%d"), g_prefs.m_previewPath, Job.m_mastercopyseparationset);
					::CopyFile(sSourcePath + "\\ImageProperties.xml", sWebpath + "\\ImageProperties.xml", 0);

					sSourcePath.Format(_T("%s%d\\TileGroup0"), g_prefs.m_previewPath, Job.m_mastercopyseparationset);

					sWebpath.Format(_T("%sCCpreviews\\%d\\TileGroup0"), sWebRoot, Job.m_mastercopyseparationset);
					::CreateDirectory(sWebpath, NULL);

					g_util.CopyFiles(sSourcePath, "*.jpg", sWebpath);

				}


				bFinalSuccess = bSuccess;
				if (pProofTemplate->m_generateforflash == 2) {
					s.Format("%d.jpg", Job.m_mastercopyseparationset);
					m_proofDB.InsertFlashJob(s, sErrorMessage);
				}
			}

			bSuccess = m_proofDB.UpdateStatusProof(Job.m_mastercopyseparationset, bSuccess ? PROOFSTATUSNUMBER_PROOFED : PROOFSTATUSNUMBER_PROOFERROR, sProofMsg, bUpdateOK, sErrorMessage);

			if (!bSuccess) {
				g_util.LogprintfResample("ERROR: UpdateStatusProof() - %s", sErrorMessage);

				bGotJobs = FALSE;

				sLastError = sErrorMessage;
				bDBReconnect = TRUE;

				m_proofDB.UpdateService(SERVICESTATUS_HASERROR, _T(""), sLastError, sErrorMessage);
				continue;
			}
			bFinalSuccess = bSuccess;
			m_proofDB.UpdateService(SERVICESTATUS_RUNNING, Job.m_filename, _T(""), sErrorMessage);
			m_proofDB.InsertLogEntry(EVENTCODE_PROOFED, pProofTemplate->m_proofname, Job.m_filename, _T(""), Job.m_mastercopyseparationset, Job.m_version, 0, _T(""), sErrorMessage);

		}
	}

	g_proofthreadrunning = FALSE;
	m_proofDB.UpdateService(SERVICESTATUS_STOPPED, _T(""), _T(""), sErrorMessage);
	m_proofDB.ExitDB();

	g_util.LogprintfResample("INFO: ProofThread() terminated");

	return 0;
}

int GhostPDF(CString sFile, CString sJPEGname, int nJpegQuality, int nPreviewResolution, int nThumbnailResolution,
	BOOL bBlocking, PROOFPROCESSSTRUCT * pProofTemplate, CString sTitle)
{
	TCHAR lpstrFile[MAX_PATH];
	CString sCmd;
	CString sErrorMessage = _T("");

	CTiffUtils tiffutil;

	CString sExeFile = g_prefs.m_ghostscriptpath + _T("\\bin\\") + g_prefs.m_ghostscriptcommandfile;
	if (g_util.FileExist(sExeFile) == FALSE) {
		g_util.LogprintfResample("ERROR: Ghostscript command file %s not found", sExeFile);
		return FALSE;
	}

	_tcscpy(lpstrFile, sFile);

	CString sOutputFile = sFile;
	int n = sFile.ReverseFind('.');
	if (n != -1)
		sOutputFile = sFile.Left(n);
	else
		sOutputFile = sFile;

	CString sTempIn = g_util.GetTempFolder() + _T("\\") +  g_util.GetFileName(sFile);
	::CopyFile(sFile, sTempIn, FALSE);
	CString sTempOut = g_util.GetTempFolder() + _T("\\") + g_util.GetFileName(sOutputFile) + ".tif";

	::DeleteFile(sTempOut);

	sCmd.Format("\"%s\" \"-I%s\\lib;%s\" -dNOPAUSE -dBATCH -dNOSAFER -sDEVICE=tiff32nc -r%d -sOutputFile=\"%s\" \"%s\"",
		sExeFile, g_prefs.m_ghostscriptpath, g_prefs.m_ghostscriptfontpath, nPreviewResolution, sTempOut, sTempIn);

	if (bBlocking)
		::DeleteFile(sOutputFile);

	::DeleteFile(sTempOut);

	g_util.LogprintfResample("INFO:  Ghostripping pdf document %s - command %s", sTempIn, sCmd);

	if (g_util.DoCreateProcess(sCmd, g_prefs.m_pdfprinttimeout) == FALSE) {
		g_util.LogprintfResample("ERROR: gswin32c.exe background shell command %s failed\r\n", sCmd);
		return FALSE;
	}

	g_util.LogprintfResample("INFO:  Print command issued - waiing for preview TIFF file %s...", sTempOut);

	::Sleep(3000);
	if (bBlocking) {

		int nTimeOut = g_prefs.m_pdfprinttimeout;
		int nProgress = 10;
		while (nTimeOut-- > 0 && g_BreakSignal == FALSE) {
			if (g_util.FileExist(sTempOut))
				break;
			::Sleep(1000);
		}

		if (nTimeOut <= 0 || g_BreakSignal == TRUE) {
			g_util.LogprintfResample("INFO:  Timeout waiting for Ghostscript output file...(%d sec timeout)", g_prefs.m_pdfprinttimeout);
			// Gun down the process..			
			sCmd.Format("taskkill /f /i \"%s\"", g_prefs.m_apppath, g_prefs.m_ghostscriptcommandfile);
			g_util.DoCreateProcess(sCmd, 60);
			::Sleep(1000);
			// True again - just in case...
			sCmd.Format("taskkill /f /i \"%s\"", g_prefs.m_apppath, g_prefs.m_ghostscriptcommandfile);
			g_util.DoCreateProcess(sCmd, 60);
			::Sleep(1000);
			::DeleteFile(sTempIn);
			::DeleteFile(sTempOut);
			if (g_prefs.m_usedummypreviewonpdffail && g_prefs.m_dummypreview != "" && g_prefs.m_dummythumbnail != "") {
				::CopyFile(g_prefs.m_dummypreview, g_prefs.m_previewPath + sJPEGname, FALSE);
				::CopyFile(g_prefs.m_dummythumbnail, g_prefs.m_thumbnailPath + sJPEGname, FALSE);
				return TRUE;
			}
			else
				return FALSE;
		}
	}

	//	if (util.FileExist(sOutputFile) == FALSE) {
	if (g_util.FileExist(sTempOut) == FALSE) {
		g_util.LogprintfResample("ERROR: gswin32c failed (command %s)", sCmd);
		::DeleteFile(sTempIn);
		return FALSE;
	}

	g_util.LogprintfResample("INFO:  Moving file %s to %s)", sTempOut, sOutputFile);
	::CopyFile(sTempOut, sOutputFile, FALSE);
	::DeleteFile(sTempIn);
	::DeleteFile(sTempOut);

	if (g_util.FileExist(sOutputFile) == FALSE) {
		g_util.LogprintfResample("ERROR: File not copied to %s", sOutputFile);
		return FALSE;
	}

	//util.LogprintfResample("INFO:  pdf document %s printed successfully", sCmd);



	int tnw, tnh, type;

	BYTE *image = ReadTiffAndCropMargin(sOutputFile.GetBuffer(MAX_PATH), &tnw, &tnh, &type, 0);
	sOutputFile.ReleaseBuffer();
	DeleteFile(sOutputFile);
	if (image == NULL) {
		g_util.LogprintfResample("ERROR: ReadTiff returned NULL (%s)", sOutputFile);
		return FALSE;
	}
	g_util.LogprintfResample("INFO:  ReadTiff returned width=%d, height=%d, bytes per sample = %d", tnw, tnh, type);


	if (pProofTemplate->icc.m_icc_enable) {
		g_util.LogprintfResample("INFO:  Profiling PDF preview..");
		BYTE *pProfiledData = ComputeICC2RGB(image, tnw, tnh,
			pProofTemplate->icc.m_icc_input_profile,
			pProofTemplate->icc.m_icc_monitor_profile,
			pProofTemplate->icc.m_icc_proof_profile,
			pProofTemplate->icc.m_icc_enable_proof,
			pProofTemplate->icc.m_icc_rendering,
			pProofTemplate->icc.m_icc_proof_rendering,
			pProofTemplate->icc.m_icc_out_of_gamut, sErrorMessage);
		if (pProfiledData != NULL) {
			delete image;
			image = pProfiledData;
		}
	}
	else if (type == 4) {
		g_util.LogprintfResample("INFO:  Running CMYK-to-RGB on PDF preview..");
		image = CMYKInterleavedToRGB(image, tnw, tnh);
	}

	double unitfactor = 1.0 / 25.4;
	if (pProofTemplate->output.m_useboundingbox == 1) {
		BYTE *pNewData = ApplyBoundingBox(image, &tnw, &tnh, 3, nPreviewResolution,
			pProofTemplate->output.m_boundingboxsizex * unitfactor,
			pProofTemplate->output.m_boundingboxsizey * unitfactor,
			pProofTemplate->output.m_boundingboxposx * unitfactor,
			pProofTemplate->output.m_boundingboxposy * unitfactor);
		if (pNewData != NULL) {
			//	delete image;
			image = pNewData;
		}
	}

	if (pProofTemplate->resample.m_rotate == 180 || pProofTemplate->resample.m_rotate == 2) {
		BYTE *pNewData = Rotate180RGB(image, tnw, tnh);
		if (pNewData != NULL) {
			//	delete image;
			image = pNewData;
		}
	}
	else if (pProofTemplate->resample.m_rotate == 90 || pProofTemplate->resample.m_rotate == 1) {
		BYTE *pNewData = RotateRGB90(image, &tnw, &tnh, FALSE);
		if (pNewData != NULL) {
			//	delete image;
			image = pNewData;
		}
	}
	else if (pProofTemplate->resample.m_rotate == 270 || pProofTemplate->resample.m_rotate == 3) {
		BYTE *pNewData = RotateRGB90(image, &tnw, &tnh, TRUE);
		if (pNewData != NULL) {
			//	delete image;
			image = pNewData;
		}
	}

	if (pProofTemplate->resample.m_invert) {
		InvertRGB(image, tnw, tnh);
	}
	if (pProofTemplate->resample.m_mirror) {
		MirrorRGB(image, tnw, tnh);
	}


	BOOL ret = CreateJPEGsFromBuffer(image, tnw, tnh, 3, g_prefs.m_previewPath + sJPEGname, 
					g_prefs.m_thumbnailPath + sJPEGname, nJpegQuality, (int)nPreviewResolution, nPreviewResolution, 
					nThumbnailResolution, pProofTemplate->filter.m_usefilter, sTitle);

	return ret;
}


int PDF2RIP(CString sFile, CString sJPEGname, int nJpegQuality, int nPreviewResolution, int nThumbnailResolution, PROOFPROCESSSTRUCT * pProofTemplate, CString sTitle)
{
	CString sCmd;
	CString sErrorMessage = _T("");

	CTiffUtils tiffutil;
	int ret;

	CString sInputFile = g_prefs.m_pdfripinputfolder + _T("\\") + g_util.GetFileName(sFile);
	CString sOutputFile = g_prefs.m_pdfripoutputfolder + _T("\\") + g_util.GetFileName(sFile) + ".tif";


	::DeleteFile(sOutputFile);



	for (int outerretry = 0; outerretry < 2; outerretry++) {
		sInputFile = g_prefs.m_pdfripinputfolder + _T("\\") + g_util.GetFileName(sFile);
		sOutputFile = g_prefs.m_pdfripoutputfolder + _T("\\") + g_util.GetFileName(sFile) + ".tif";

		if (outerretry == 1 && (g_prefs.m_pdfripinputfolder2 == "" || g_prefs.m_pdfripoutputfolder2 == ""))
			break;

		if (outerretry == 1) {
			sInputFile = g_prefs.m_pdfripinputfolder2 + _T("\\") + g_util.GetFileName(sFile);
			sOutputFile = g_prefs.m_pdfripoutputfolder2 + _T("\\") + g_util.GetFileName(sFile) + ".tif";
		}

		g_util.LogprintfResample("INFO:  RIPing pdf document %s to %s..", sInputFile, sOutputFile);
		::DeleteFile(sOutputFile);
		::CopyFile(sFile, sInputFile, FALSE);

		if (g_prefs.m_pdfprinttimeout > 0) {

			int nTimeOut = g_prefs.m_pdfprinttimeout;
			float Xres, Yres, Width, Height;
			DWORD w, h, dwBPL;
			BOOL  bBlackIsMin;
			int nProgress = 10;
			while (nTimeOut-- > 0 && g_BreakSignal == FALSE) {
				if (tiffutil.TiffInfo(sOutputFile, &Xres, &Yres, &Width, &Height, &bBlackIsMin, &w, &h, &dwBPL))
					break;
				Sleep(1000);
				if (++nProgress > 50)
					nProgress = 50;

			}

			if (g_BreakSignal == TRUE)
				break;
		}
		if (g_util.FileExist(sOutputFile))
			break;
	}

	if (g_util.FileExist(sOutputFile) == FALSE) {
		g_util.LogprintfResample("ERROR: PDF2RIP failed - %s not found", sOutputFile);
		return FALSE;
	}

	g_util.LogprintfResample("INFO:  pdf document %s ripped successfully", sFile);


	int tnw, tnh, type;
	int orgResolution;
	BYTE *image = ReadTiff(sOutputFile.GetBuffer(MAX_PATH), &tnw, &tnh, &type, &orgResolution);
	sOutputFile.ReleaseBuffer();

	::DeleteFile(sInputFile);
	::DeleteFile(sOutputFile);
	g_util.LogprintfResample("INFO:  pdf document ReadTiff(%s) OK", sOutputFile);

	if (image == NULL) {
		g_util.LogprintfResample("ERROR: ReadTiff returned NULL (%s)", sOutputFile);
		return FALSE;
	}

	// image has CMYK data
	if (pProofTemplate->resample.m_writeeachcolor) {
		BYTE *c_buf = NULL;
		BYTE *m_buf = NULL;
		BYTE *y_buf = NULL;
		BYTE *k_buf = NULL;

		c_buf = new BYTE[tnw*tnh];
		m_buf = new BYTE[tnw*tnh];
		y_buf = new BYTE[tnw*tnh];
		k_buf = new BYTE[tnw*tnh];

		if (CMYKInterleaved2Separated(image, c_buf, m_buf, y_buf, k_buf, tnw, tnh)) {

			CString sPreviewName = g_prefs.m_previewPath + g_util.GetFileName(sJPEGname, TRUE) + _T("_C.jpg");
			ret = CreateSingleColorJPEG(c_buf, tnw, tnh, 0, sPreviewName.GetBuffer(255), nJpegQuality, sTitle, orgResolution);
			//ret = CreateJPEG2(c_buf, tnw, tnh, 1, sPreviewName.GetBuffer(255), nJpegQuality, sTitle);
			sPreviewName.ReleaseBuffer();
			sPreviewName = g_prefs.m_previewPath + g_util.GetFileName(sJPEGname, TRUE) + _T("_M.jpg");
			ret = CreateSingleColorJPEG(m_buf, tnw, tnh, 1, sPreviewName.GetBuffer(255), nJpegQuality, sTitle, orgResolution);
			//ret = CreateJPEG2(m_buf, tnw, tnh, 1, sPreviewName.GetBuffer(255), nJpegQuality, sTitle);
			sPreviewName.ReleaseBuffer();
			sPreviewName = g_prefs.m_previewPath + g_util.GetFileName(sJPEGname, TRUE) + _T("_Y.jpg");
			ret = CreateSingleColorJPEG(y_buf, tnw, tnh, 2, sPreviewName.GetBuffer(255), nJpegQuality, sTitle, orgResolution);

			//			ret = CreateJPEG2(y_buf, tnw, tnh, 1, sPreviewName.GetBuffer(255), nJpegQuality, sTitle);
			sPreviewName.ReleaseBuffer();
			sPreviewName = g_prefs.m_previewPath + g_util.GetFileName(sJPEGname, TRUE) + _T("_K.jpg");
			ret = CreateSingleColorJPEG(k_buf, tnw, tnh, 3, sPreviewName.GetBuffer(255), nJpegQuality, sTitle, orgResolution);
			//			ret = CreateJPEG2(k_buf, tnw, tnh, 1, sPreviewName.GetBuffer(255), nJpegQuality, sTitle);
			sPreviewName.ReleaseBuffer();

			if (pProofTemplate->m_generateheatmap && c_buf != NULL && m_buf != NULL && y_buf != NULL && k_buf != NULL) {
				BYTE  *heatmapimage = GenerateHeatMap(c_buf, m_buf, y_buf, k_buf, tnw, tnh);
				if (heatmapimage != NULL) {
					sPreviewName = g_prefs.m_previewPath + g_util.GetFileName(sJPEGname, TRUE) + _T("_dns.jpg");
					ret = CreateJPEG2(heatmapimage, tnw, tnh, 3, sPreviewName.GetBuffer(255), nJpegQuality, sTitle);

					delete heatmapimage;
				}
			}

			if (c_buf != NULL)
				delete c_buf;
			if (m_buf != NULL)
				delete m_buf;
			if (y_buf != NULL)
				delete y_buf;
			if (k_buf != NULL)
				delete k_buf;
		}
	}

	if (pProofTemplate->icc.m_icc_enable) {
		BYTE *pProfiledData = ComputeICC2RGB(image, tnw, tnh,
			pProofTemplate->icc.m_icc_input_profile,
			pProofTemplate->icc.m_icc_monitor_profile,
			pProofTemplate->icc.m_icc_proof_profile,
			pProofTemplate->icc.m_icc_enable_proof,
			pProofTemplate->icc.m_icc_rendering,
			pProofTemplate->icc.m_icc_proof_rendering,
			pProofTemplate->icc.m_icc_out_of_gamut, sErrorMessage);
		if (pProfiledData != NULL) {
			delete image;
			image = pProfiledData;
		}
		else
			g_util.LogprintfResample("ERROR: ComputeICC2RGB failed!");

	}
	else {
		image = CMYKInterleavedToRGB(image, tnw, tnh);


	}
	g_util.LogprintfResample("INFO: Preview is now RGB");

	// Data is now RGB

	double unitfactor = 1.0 / 25.4;
	if (pProofTemplate->output.m_useboundingbox == 1) {
		BYTE *pNewData = ApplyBoundingBox(image, &tnw, &tnh, 3, orgResolution,
			pProofTemplate->output.m_boundingboxsizex * unitfactor,
			pProofTemplate->output.m_boundingboxsizey * unitfactor,
			pProofTemplate->output.m_boundingboxposx * unitfactor,
			pProofTemplate->output.m_boundingboxposy * unitfactor);
		if (pNewData != NULL) {
			delete image;
			image = pNewData;
		}
	}

	if (pProofTemplate->resample.m_rotate == 180 || pProofTemplate->resample.m_rotate == 2) {
		BYTE *pNewData = Rotate180RGB(image, tnw, tnh);
		if (pNewData != NULL) {
			delete image;
			image = pNewData;
		}
	}
	else if (pProofTemplate->resample.m_rotate == 90 || pProofTemplate->resample.m_rotate == 1) {
		BYTE *pNewData = RotateRGB90(image, &tnw, &tnh, FALSE);
		if (pNewData != NULL) {
			delete image;
			image = pNewData;
		}
	}
	else if (pProofTemplate->resample.m_rotate == 270 || pProofTemplate->resample.m_rotate == 3) {
		BYTE *pNewData = RotateRGB90(image, &tnw, &tnh, TRUE);
		if (pNewData != NULL) {
			delete image;
			image = pNewData;
		}
	}

	if (pProofTemplate->resample.m_invert) {
		InvertRGB(image, tnw, tnh);
	}
	if (pProofTemplate->resample.m_mirror) {
		MirrorRGB(image, tnw, tnh);
	}


	g_util.LogprintfResample("INFO: Calling CreateJPEGsFromBuffer()");
	ret = CreateJPEGsFromBuffer(image, tnw, tnh, 3, g_prefs.m_previewPath + sJPEGname, g_prefs.m_thumbnailPath + sJPEGname, nJpegQuality, orgResolution, nPreviewResolution, nThumbnailResolution, pProofTemplate->filter.m_usefilter, sTitle);
	g_util.LogprintfResample("INFO: CreateJPEGsFromBuffer() returned OK");

	return ret;
}



int ProofCallas(CString sFile, CString sJPEGname, int nJpegQuality, int nPreviewResolution, int nThumbnailResolution, PROOFPROCESSSTRUCT * pProofTemplate, CString sTitle, CString &sErrorMessage)
{
	CString sCmd;
	sErrorMessage = _T("");
	DWORD dwPID;
	DWORD dwExitCode = 0;
	int nLogStatusOK = 0;
	CString sLogFileErrorMessage = _T("");

	CString sOutputFolder = g_util.GetTempFolder();
	if (sOutputFolder.Right(1) == "\\")
		sOutputFolder = sOutputFolder.Left(sOutputFolder.GetLength() - 1);

	CString sOutputPath = sOutputFolder + _T("\\") + g_util.GetFileName(sFile, TRUE) + _T("_1.jpg");
	CString sLogPath = g_util.GetTempFolder() + _T("\\") + g_util.GetFileName(sFile, TRUE) + _T(".log");
	CString sPreviewPath = g_prefs.m_previewPath + sJPEGname;
	CString sThumbnailsPath = g_prefs.m_thumbnailPath + sJPEGname;
	::DeleteFile(sOutputPath);
	::DeleteFile(sLogPath);

	sCmd.Format(_T("\"%s\" %s --saveasimg \"%s\" 0 %d 0 0 1 \"%s\" \"%s\""),
				g_prefs.m_callascommandfilepath,
				g_prefs.m_callaskeyCode,
				sFile,
				nPreviewResolution,
				sOutputFolder,
				sLogPath);

	g_util.LogprintfResample("INFO:  ProofCallas() - " + sCmd);

	int nFireCmdRet = g_util.DoCreateProcess(sCmd, g_prefs.m_processtimeout, TRUE, dwExitCode, dwPID);

	BOOL bLogStatusOK = TRUE;
	BOOL bFileOutputOK = FALSE;

	if (nFireCmdRet == TRUE) {
		bFileOutputOK = g_util.FileExist(sOutputPath) && (g_util.GetFileSize(sOutputPath) > 0);
		g_util.LogprintfResample("INFO:  bFileOutputOK(1)=%d", bFileOutputOK);

		if (g_util.FileExist(sLogPath))
			bLogStatusOK = g_util.GetLogFileStatus(sLogPath, sErrorMessage);
		
		if (bFileOutputOK) {
			::DeleteFile(sPreviewPath);

			//g_util.LogprintfResample("INFO:  Adding title %s to file %s", sTitle, sOutputPath);
			// AddCommentToJPEG(sOutputPath, sTitle);
			bFileOutputOK = ::CopyFile(sOutputPath, sPreviewPath, FALSE);
			g_util.LogprintfResample("INFO:  bFileOutputOK(1.1)=%d", bFileOutputOK);
			if (bFileOutputOK == FALSE) {
				g_util.LogprintfResample("WARNING: CopyFile(1) returned FALSE - " + g_util.GetLastWin32Error());
				::Sleep(1000);
				bFileOutputOK = ::CopyFile(sOutputPath, sPreviewPath, FALSE);
				
				if (bFileOutputOK == FALSE)
					g_util.LogprintfResample("WARNING: CopyFile(2) returned FALSE - " + g_util.GetLastWin32Error());
			}	
		}		
	}
	g_util.LogprintfResample("INFO:  bFileOutputOK=%d", bFileOutputOK);
	g_util.LogprintfResample("INFO:  bLogStatusOK=%d", bLogStatusOK);

	::DeleteFile(sOutputPath);
	::DeleteFile(sLogPath);

	if (nFireCmdRet == -1 && dwPID > 0) {
		g_util.KillProcess(dwPID);
		sErrorMessage.Format("ERROR: Timeout waiting for callas proof process - %s -> %s", sFile, sOutputPath);
		g_util.LogprintfResample("%s", sErrorMessage);
		return FALSE;
	}
	
	if (bFileOutputOK == FALSE) {
		sErrorMessage.Format("ERROR: callas proof process output file %s not found.", sOutputPath);
		g_util.LogprintfResample("%s", sErrorMessage);
		return FALSE;
	}

	DWORD tnw, tnh, jpegthumbnailwidth, jpegthumbnailheight;
	BYTE *imageBuffer = ReadJPEG(sPreviewPath.GetBuffer(256), &tnw, &tnh);
	sPreviewPath.ReleaseBuffer();
	if (imageBuffer == NULL) {
		sErrorMessage.Format("ERROR: callas proof process ReadJPEG() failed for file %s", sPreviewPath);
		return FALSE;
	}
	BYTE *pThumbnailData = DownSampleCT(imageBuffer, tnw, tnh, 3, &jpegthumbnailwidth, &jpegthumbnailheight, nPreviewResolution, nThumbnailResolution, TRUE);
	if (pThumbnailData == NULL)
	{
		sErrorMessage.Format("ERROR: callas proof process DownSampleCT() failed for file %s", sPreviewPath);
		return FALSE;
	}

	delete imageBuffer;

	int ret = CreateJPEG2(pThumbnailData, jpegthumbnailwidth, jpegthumbnailheight, 3, sThumbnailsPath.GetBuffer(256), 100, sTitle);
	sThumbnailsPath.ReleaseBuffer();
	delete pThumbnailData;
	

	if (ret == FALSE) {
		sErrorMessage.Format("ERROR: callas proof process thumbnail CreateJPEG2() failed for file %s", sThumbnailsPath);
		return FALSE;
	}
	
	return TRUE;
}


