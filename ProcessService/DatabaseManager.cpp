#include "StdAfx.h"
#include "Defs.h"
#include "DatabaseManager.h"
#include "Utils.h"
#include "Prefs.h"

extern CPrefs g_prefs;
extern CUtils g_util;
extern bool g_pollthreadrunning;

CDatabaseManager::CDatabaseManager(void)
{
	m_DBopen = FALSE;
	m_pDB = NULL;

	m_DBserver = _T(".");
	m_Database = _T("ControlCenter");
	m_DBuser = "saxxxxxxxxx";
	m_DBpassword = "xxxxxx";
	m_IntegratedSecurity = FALSE;
	m_PersistentConnection = FALSE;

}

CDatabaseManager::~CDatabaseManager(void)
{
	ExitDB();
	if (m_pDB != NULL)
		delete m_pDB;
}

BOOL CDatabaseManager::InitDB(CString sDBserver, CString sDatabase, CString sDBuser, CString sDBpassword, BOOL bIntegratedSecurity, CString &sErrorMessage)
{
	m_DBserver = sDBserver;
	m_Database = sDatabase;
	m_DBuser = sDBuser;
	m_DBpassword = sDBpassword;
	m_IntegratedSecurity = bIntegratedSecurity;

	return InitDB(sErrorMessage);
}


int CDatabaseManager::InitDB(CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (m_pDB) {
		if (m_pDB->IsOpen() == FALSE) {
			try {
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
		}
	}

	if (!m_PersistentConnection)
		ExitDB();

	if (m_DBopen)
		return TRUE;

	if (m_DBserver == _T("") || m_Database == _T("") || m_DBuser == _T("")) {
		sErrorMessage = _T("Empty server, database or username not allowed");
		return FALSE;
	}

	if (m_pDB == NULL)
		m_pDB = new CDatabase;

	m_pDB->SetLoginTimeout(g_prefs.m_databaselogintimeout);
	m_pDB->SetQueryTimeout(g_prefs.m_databasequerytimeout);

	CString sConnectStr = _T("Driver={SQL Server}; Server=") + m_DBserver + _T("; ") +
		_T("Database=") + m_Database + _T("; ");

	if (m_IntegratedSecurity)
		sConnectStr += _T(" Integrated Security=True;");
	else
		sConnectStr += _T("USER=") + m_DBuser + _T("; PASSWORD=") + m_DBpassword + _T(";");

	try {
		if (!m_pDB->OpenEx((LPCTSTR)sConnectStr, CDatabase::noOdbcDialog)) {
			sErrorMessage.Format(_T("Error connecting to database with connection string '%s'"), (LPCSTR)sConnectStr);
			return FALSE;
		}
	}
	catch (CDBException* e) {
		sErrorMessage.Format(_T("Error connecting to database - %s (%s)"), (LPCSTR)e->m_strError, (LPCSTR)sConnectStr);
		e->Delete();
		return FALSE;
	}

	m_DBopen = TRUE;
	return TRUE;
}

void CDatabaseManager::ExitDB()
{
	if (!m_DBopen)
		return;

	if (m_pDB)
		m_pDB->Close();

	m_DBopen = FALSE;

	return;
}

BOOL CDatabaseManager::IsOpen()
{
	return m_DBopen;
}

//
// SERVICE RELATED METHODS
//

BOOL CDatabaseManager::LoadConfigIniFile(int nInstanceNumber, CString &sFileName, CString &sFileName2, CString &sFileName3, CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;

	CRecordset Rs(m_pDB);
	CString sSQL, s;
	sSQL.Format("SELECT ConfigData,ConfigData2,ConfigData3 FROM ServiceConfigurations WHERE ServiceName='ProcessService' AND InstanceNumber=%d", nInstanceNumber);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format(_T("Query failed - %s"), (LPCSTR)sSQL);
			return FALSE;
		}

		if (!Rs.IsEOF()) {
			Rs.GetFieldValue((short)0, s);
			sFileName = s;
			Rs.GetFieldValue((short)1, s);
			sFileName2  = s;
			
			Rs.GetFieldValue((short)2, s);
			sFileName3 = s;
			
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format(_T("ERROR (DATABASEMGR): Query failed - %s"), (LPCSTR)e->m_strError);
		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}
	return TRUE;

}


BOOL CDatabaseManager::RegisterService(CString &sErrorMessage)
{
	CString sSQL, s;
	sErrorMessage = _T("");

	if (InitDB(sErrorMessage) == FALSE)
		return -1;
	
	CRecordset Rs(m_pDB);

	TCHAR buf[MAX_PATH];
	DWORD buflen = MAX_PATH;
	::GetComputerName(buf, &buflen);

	sSQL.Format("{CALL spRegisterService ('ProcessService', %d, 3, '%s',-1,'','','')}", g_prefs.m_instancenumber, buf);

	try {
		m_pDB->ExecuteSQL(sSQL);
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Insert failed - %s", (LPCSTR)e->m_strError);
		e->Delete();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CDatabaseManager::InsertLogEntry(int nEvent, CString sSource,  CString sFileName, CString sMessage, int nMasterCopySeparationSet, int nVersion, int nMiscInt, CString sMiscString, CString &sErrorMessage)
{
	CString sSQL;
	
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;
	
	if (sMessage == "")
		sMessage = "Ok";

	sSQL.Format("{CALL [spAddServiceLogEntry] ('%s',%d,%d, '%s','%s', '%s',%d,%d,%d,'%s')}",
		_T("ProcessService"), g_prefs.m_instancenumber, nEvent, sSource, sFileName, sMessage, nMasterCopySeparationSet, nVersion, nMiscInt, sMiscString);

	try {
		m_pDB->ExecuteSQL( sSQL );
	}
	catch( CDBException* e ) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Insert failed - %s", (LPCSTR)e->m_strError);
		e->Delete();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch( CDBException* e ) {
			// So what..! Go on!;
		}
		return FALSE;
	}

	return TRUE;
}

BOOL CDatabaseManager::UpdateService(int nCurrentState, CString sCurrentJob, CString sLastError, CString &sErrorMessage)
{
	return UpdateService(nCurrentState, sCurrentJob, sLastError, _T(""), sErrorMessage);
}

BOOL CDatabaseManager::UpdateService(int nCurrentState, CString sCurrentJob, CString sLastError, CString sAddedLogData, CString &sErrorMessage)
{
	CString sSQL;

	TCHAR buf[MAX_PATH];
	DWORD buflen = MAX_PATH;
	::GetComputerName(buf, &buflen);
	sErrorMessage = _T("");

	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;
	sSQL.Format("{CALL spRegisterService ('ProcessService', %d, 3, '%s',%d,'%s','%s','%s')}", g_prefs.m_instancenumber, buf, nCurrentState, sCurrentJob, sLastError, sAddedLogData);

	try {
		m_pDB->ExecuteSQL( sSQL );
	}
	catch( CDBException* e ) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Update failed - %s", (LPCSTR)e->m_strError);
		e->Delete();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch( CDBException* e ) {
			// So what..! Go on!;
		}
		return FALSE;
	}
 
	return TRUE;
}



//
// TX-TRIGGER RELATED METHODS
//

int CDatabaseManager::ReleaseProofLock(CString &sErrorMessage)
{
	BOOL ret = -1;
	int nRetries = g_prefs.m_nQueryRetries;

	while (ret == -1 && nRetries-- > 0) {

		ret = SetProofLock(0, sErrorMessage);
		if (ret == -1)
			::Sleep(g_prefs.m_QueryBackoffTime);
	}

	return ret;
}

int CDatabaseManager::ReleaseConvertLock(int nConvertType, int nInstanceNumber, CString &sErrorMessage)
{
	BOOL ret = -1;
	int nRetries = g_prefs.m_nQueryRetries;

	while (ret == -1 && nRetries-- > 0) {

		ret = SetConvertLock(nConvertType, 0, nInstanceNumber, sErrorMessage);
		if (ret == -1)
			::Sleep(g_prefs.m_QueryBackoffTime);
	}

	return ret;
}


int	CDatabaseManager::AcuireProofLock(CString &sErrorMessage)
{
	BOOL ret = -1;
	int nRetries = g_prefs.m_nQueryRetries;

	while (ret == -1 && nRetries-- > 0) {

		ret = SetProofLock(1, sErrorMessage);
		if (ret == -1)
			::Sleep(g_prefs.m_QueryBackoffTime);
	}
	return ret;
}

int CDatabaseManager::SetProofLock(int nLockType, CString &sErrorMessage)
{
	CString sSQL;
	
	int nCurrentLock = -1;

	if (g_prefs.m_prooflocksystem == FALSE)
		return 1;

	CString sComputer = g_util.GetComputerName();

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;

	CRecordset Rs(m_pDB);

	sSQL.Format("{CALL spProofLock (%d,'%s')}", nLockType, sComputer);
	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return FALSE;
		}

		if (!Rs.IsEOF()) {
			CString s;
			Rs.GetFieldValue((short)0, s);
			nCurrentLock = atoi(s);
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return -1;
	}
	return nCurrentLock;
}


int	CDatabaseManager::AcuireConvertLock(int nConvertType, int nInstanceNumber, CString &sErrorMessage)
{
	BOOL ret = -1;
	int nRetries = g_prefs.m_nQueryRetries;

	while (ret == -1 && nRetries-- > 0) {

		ret = SetConvertLock(nConvertType,1, nInstanceNumber,sErrorMessage);
		if (ret == -1)
			::Sleep(g_prefs.m_QueryBackoffTime);
	}
	return ret;
}

int CDatabaseManager::SetConvertLock(int nConvertType, int nLockType, int nInstanceNumber, CString &sErrorMessage)
{
	CString sSQL;
	int nCurrentLock = -1;

	//if (g_prefs.m_prooflocksystem == FALSE)
	//	return 1;

	CString sComputer = g_util.GetComputerName();

	CString sFullMachineName;
	sFullMachineName.Format("%s-%s-%d", sComputer, nConvertType ? _T("-low") : _T("-print"), nInstanceNumber);
	

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;

	CRecordset Rs(m_pDB);

	sSQL.Format("{CALL spConvertLock (%d,'%s')}", nLockType, sFullMachineName);
	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return FALSE;
		}

		if (!Rs.IsEOF()) {
			CString s;
			Rs.GetFieldValue((short)0, s);
			nCurrentLock = atoi(s);
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return -1;
	}
	return nCurrentLock;
}



BOOL CDatabaseManager::LoadSTMPSetup(CString &sErrorMessage)
{
	sErrorMessage = _T("");

	CString sSQL = _T("SELECT TOP 1 SMTPServer,SMTPPort, SMTPUserName,SMTPPassword,UseSSL,SMTPConnectionTimeout,SMTPFrom,SMTPCC,SMTPTo,SMTPSubject FROM SMTPPreferences");
	CString s;
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset Rs(m_pDB);

	try {
		if (!Rs.Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return FALSE;
		}

		if (!Rs.IsEOF()) {
			CString s;
			int fld = 0;
			Rs.GetFieldValue((short)fld++, g_prefs.m_emailsmtpserver);
			Rs.GetFieldValue((short)fld++, s);
			g_prefs.m_mailsmtpport = atoi(s);

			Rs.GetFieldValue((short)fld++, g_prefs.m_mailsmtpserverusername);
			Rs.GetFieldValue((short)fld++, g_prefs.m_mailsmtpserverpassword);
			Rs.GetFieldValue((short)fld++, s);
			g_prefs.m_mailusessl = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			g_prefs.m_emailtimeout = atoi(s);

			Rs.GetFieldValue((short)fld++, g_prefs.m_emailfrom);
			Rs.GetFieldValue((short)fld++, g_prefs.m_emailcc);
			Rs.GetFieldValue((short)fld++, g_prefs.m_emailto);
			Rs.GetFieldValue((short)fld++, g_prefs.m_emailsubject);

		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("Query failed - %s (%s)", e->m_strError, sSQL);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
			return FALSE;
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}

	return TRUE;

}

BOOL CDatabaseManager::LoadAllPrefs(CString &sErrorMessage)
{
	if (LoadGeneralPreferences(sErrorMessage) <= 0)
		return FALSE;

	LoadSTMPSetup(sErrorMessage);

	if (LoadPublicationList(sErrorMessage) == FALSE)
		return FALSE;

	if (LoadSectionNameList(sErrorMessage) == FALSE)
		return FALSE;

	if (LoadEditionNameList(sErrorMessage) == FALSE)
		return FALSE;


	if (LoadFileServerList(sErrorMessage) == FALSE)
		return FALSE;


	if (LoadAliasList(sErrorMessage) == FALSE)
		return FALSE;


	g_prefs.m_StatusList.RemoveAll();
	if (LoadStatusList(_T("StatusCodes"), g_prefs.m_StatusList, sErrorMessage) == FALSE)
		return FALSE;

	g_prefs.m_ExternalStatusList.RemoveAll();
	if (LoadStatusList(_T("ExternalStatusCodes"), g_prefs.m_ExternalStatusList, sErrorMessage) == FALSE)
		return FALSE;

	if (LoadPublisherList(sErrorMessage) == -1)
		return FALSE;

	
	if (LoadChannelList(sErrorMessage) == -1)
		return FALSE;

	if (LoadProofProcessList(sErrorMessage) == FALSE)
		return FALSE;

	if (LoadPDFProcessList(sErrorMessage) == FALSE)
		return FALSE;

	return TRUE;
}


// Returns : -1 error
//			  1 found data
//            0 No data (fatal)
int	CDatabaseManager::LoadGeneralPreferences(CString &sErrorMessage)
{
	BOOL foundData = FALSE;

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;

	CRecordset Rs(m_pDB);

	CString sSQL = _T("SELECT TOP 1 ServerName,ServerShare,ServerFilePath,ServerPreviewPath,ServerThumbnailPath,ServerLogPath,ServerConfigPath,ServerErrorPath,CopyProofToWeb,WebProofPath,ServerUseCurrentUser,ServerUserName,ServerPassword FROM GeneralPreferences");
	try {
		if (!Rs.Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return -1;
		}

		if (!Rs.IsEOF()) {
			CString s;
			int fld = 0;
			Rs.GetFieldValue((short)fld++, g_prefs.m_serverName);
			Rs.GetFieldValue((short)fld++, g_prefs.m_serverShare);

			Rs.GetFieldValue((short)fld++, g_prefs.m_hiresPDFPath);
			if (g_prefs.m_hiresPDFPath.Right(1) != "\\")
				g_prefs.m_hiresPDFPath += _T("\\");

			g_prefs.m_lowresPDFPath = g_prefs.m_hiresPDFPath;
			g_prefs.m_lowresPDFPath.MakeUpper();
			g_prefs.m_lowresPDFPath.Replace("CCFILESHIRES", "CCFILESLOWRES");

			g_prefs.m_printPDFPath = g_prefs.m_hiresPDFPath;
			g_prefs.m_printPDFPath.MakeUpper();
			g_prefs.m_printPDFPath.Replace("CCFILESHIRES", "CCFILESPRINT");


			Rs.GetFieldValue((short)fld++, g_prefs.m_previewPath);
			if (g_prefs.m_previewPath.Right(1) != "\\")
				g_prefs.m_previewPath += _T("\\");

			g_prefs.m_readviewpreviewPath = g_prefs.m_previewPath;
			g_prefs.m_readviewpreviewPath.MakeUpper();
			g_prefs.m_readviewpreviewPath.Replace("CCPREVIEWS", "CCreadviewpreviews");



			Rs.GetFieldValue((short)fld++, g_prefs.m_thumbnailPath);
			if (g_prefs.m_thumbnailPath.Right(1) != "\\")
				g_prefs.m_thumbnailPath += _T("\\");
			Rs.GetFieldValue((short)fld++, s);


			Rs.GetFieldValue((short)fld++, g_prefs.m_configPath);
			if (g_prefs.m_configPath.Right(1) != "\\")
				g_prefs.m_configPath += _T("\\");
			Rs.GetFieldValue((short)fld++, g_prefs.m_errorPath);
			if (g_prefs.m_errorPath.Right(1) != "\\")
				g_prefs.m_errorPath += _T("\\");


			Rs.GetFieldValue((short)fld++, s);
			g_prefs.m_copyToWeb = atoi(s);

			Rs.GetFieldValue((short)fld++, g_prefs.m_webPath);
			if (g_prefs.m_webPath.Right(1) != "\\")
				g_prefs.m_webPath += _T("\\");

			Rs.GetFieldValue((short)fld++, s);
			g_prefs.m_mainserverusecussrentuser = atoi(s);

			Rs.GetFieldValue((short)fld++, g_prefs.m_mainserverusername);
			Rs.GetFieldValue((short)fld++, g_prefs.m_mainserverpassword);



			foundData = TRUE;
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("Query failed - %s (%s)", e->m_strError, sSQL);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
			return -1;
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return -1;
	}

	if (foundData == 0)
		sErrorMessage = _T("FATAL ERROR: No data found in GeneralPrefences table");
	return foundData;
}


BOOL CDatabaseManager::LoadPublicationList(CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset Rs(m_pDB);
	CString sSQL, s;
	g_prefs.m_PublicationList.RemoveAll();

	sSQL.Format("SELECT DISTINCT PublicationID,[Name],PageFormatID,TrimToFormat,LatestHour,DefaultProofID,DefaultApprove,DefaultPriority,CustomerID,AutoPurgeKeepDays,EmailRecipient,EmailCC,EmailSubject,EmailBody,UploadFolder,Deadline,AnnumText,AllowUnplanned,ReleaseDays,ReleaseTime,PubdateMove,PubdateMoveDays,InputAlias,OutputALias,ExtendedAlias,PublisherID FROM PublicationNames ORDER BY [Name]");

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			LogprintfDB("Query failed : %s", sSQL);
			return FALSE;
		}

		while (!Rs.IsEOF()) {
			int f = 0;
			PUBLICATIONSTRUCT *pItem = new PUBLICATIONSTRUCT;
			Rs.GetFieldValue((short)f++, s);
			pItem->m_ID = atoi(s);

			Rs.GetFieldValue((short)f++, pItem->m_name);
			Rs.GetFieldValue((short)f++, s);
			pItem->m_PageFormatID = atoi(s);
			Rs.GetFieldValue((short)f++, s);
			pItem->m_TrimToFormat = atoi(s);
			Rs.GetFieldValue((short)f++, s);
			pItem->m_LatestHour = atof(s);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_ProofID = atoi(s);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_Approve = atoi(s);
			Rs.GetFieldValue((short)f++, s);
			pItem->m_priority = atoi(s);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_customerID = atoi(s);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_autoPurgeKeepDays = atoi(s);

			Rs.GetFieldValue((short)f++, pItem->m_emailrecipient);
			Rs.GetFieldValue((short)f++, pItem->m_emailCC);
			Rs.GetFieldValue((short)f++, pItem->m_emailSubject);
			Rs.GetFieldValue((short)f++, pItem->m_emailBody);
			Rs.GetFieldValue((short)f++, pItem->m_uploadfolder);

			pItem->m_deadline = CTime(1975, 1, 1);

			try {
				CDBVariant DBvar;
				Rs.GetFieldValue((short)f++, DBvar, SQL_C_TIMESTAMP);
				CTime t(DBvar.m_pdate->year, DBvar.m_pdate->month, DBvar.m_pdate->day, DBvar.m_pdate->hour, DBvar.m_pdate->minute, DBvar.m_pdate->second);
				pItem->m_deadline = t;
			}
			catch (...)
			{
			}

			Rs.GetFieldValue((short)f++, pItem->m_annumtext);
			Rs.GetFieldValue((short)f++, s);
			pItem->m_allowunplanned = atoi(s);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_releasedays = atoi(s);

			Rs.GetFieldValue((short)f++, s);
			int n = atoi(s);
			pItem->m_releasetimehour = n / 100;
			pItem->m_releasetimeminute = n - pItem->m_releasetimehour;

			Rs.GetFieldValue((short)f++, s);
			pItem->m_pubdatemove = atoi(s) > 0;

			Rs.GetFieldValue((short)f++, s);
			pItem->m_pubdatemovedays = atoi(s);

			Rs.GetFieldValue((short)f++, pItem->m_alias);
			Rs.GetFieldValue((short)f++, pItem->m_outputalias);
			Rs.GetFieldValue((short)f++, pItem->m_extendedalias);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_publisherID = atoi(s);

			g_prefs.m_PublicationList.Add(*pItem);

			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}



	for (int i = 0; i < g_prefs.m_PublicationList.GetCount(); i++)
		if (LoadPublicationChannels(g_prefs.m_PublicationList[i].m_ID, sErrorMessage) == FALSE)
			return FALSE;

	return TRUE;
}


int CDatabaseManager::LoadChannelList(CString &sErrorMessage)
{

	int foundJob = 0;
	sErrorMessage = _T("");

	if (InitDB(sErrorMessage) == FALSE)
		return -1;

	g_prefs.m_ChannelList.RemoveAll();
	CRecordset Rs(m_pDB);

	CString sSQL = _T("SELECT ChannelID,Name,Enabled,ChannelNameAlias,PDFType,MergedPDF,PDFProcessID FROM ChannelNames WHERE ChannelID>0 AND Name<>''  ORDER BY Name");
	try {
		if (!Rs.Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return -1;
		}

		while (!Rs.IsEOF()) {
			CString s;
			int fld = 0;
			CHANNELSTRUCT *pItem = new CHANNELSTRUCT();

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_channelID = atoi(s);


			Rs.GetFieldValue((short)fld++, pItem->m_channelname);

			Rs.GetFieldValue((short)fld++, s); // Enabled
			pItem->m_enabled = atoi(s);

			Rs.GetFieldValue((short)fld++, pItem->m_channelnamealias);

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_pdftype = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->m_mergePDF = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->m_pdfprocessID = atoi(s);

			g_prefs.m_ChannelList.Add(*pItem);
			foundJob++;

			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
		sErrorMessage.Format("Query failed - %s (%s)", e->m_strError, sSQL);
		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
			return -1;
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return -1;
	}

	return foundJob;
}





int CDatabaseManager::LoadPublisherList(CString &sErrorMessage)
{
	int foundJob = 0;

	//if (g_prefs.m_DBcapabilities_ChannelGroupNames == FALSE)
	//	return 0;

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;

	g_prefs.m_PublisherList.RemoveAll();
	CRecordset Rs(m_pDB);

	CString sSQL = _T("SELECT PublisherID,PublisherName FROM PublisherNames WHERE PublisherID>0 AND PublisherName<>''  ORDER BY PublisherName");
	try {
		if (!Rs.Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return -1;
		}

		while (!Rs.IsEOF()) {
			CString s;
			int fld = 0;
			ITEMSTRUCT *pItem = new ITEMSTRUCT();

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_ID = atoi(s);

			Rs.GetFieldValue((short)fld++, pItem->m_name);

			g_prefs.m_PublisherList.Add(*pItem);
			foundJob++;

			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
		sErrorMessage.Format("Query failed - %s (%s)", e->m_strError, sSQL);
		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
			return -1;
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return -1;
	}

	return foundJob;
}



BOOL CDatabaseManager::LoadPDFProcessList(CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset Rs(m_pDB);

	g_prefs.m_PDFProcessList.RemoveAll();
	CString sSQL;
	CString s;

	sSQL = _T("SELECT ProcessID, ProcessName, ProcessType, ConvertProfile, ExternalProcess, ExternalInputFolder, ExternalOutputFolder, ExternalErrorFolder, ProcessTimeOut FROM PDFProcessConfigurations WHERE ProcessType < 2");

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return FALSE;
		}

		while (!Rs.IsEOF()) {
			PDFPROCESSTYPE *pItem = new PDFPROCESSTYPE;
			Rs.GetFieldValue((short)0, s);
			pItem->m_ProcessID = atoi(s);
			
			Rs.GetFieldValue((short)1, pItem->m_ProcessName);
			
			Rs.GetFieldValue((short)2, s);
			pItem->m_ProcessType = atoi(s);

			Rs.GetFieldValue((short)3, pItem->m_ConvertProfile);

			Rs.GetFieldValue((short)4, s);
			pItem->m_ExternalProcess = atoi(s);
			Rs.GetFieldValue((short)5, pItem->m_ExternalInputFolder);
			Rs.GetFieldValue((short)6, pItem->m_ExternalOutputFolder);
			Rs.GetFieldValue((short)7, pItem->m_ExternalErrorFolder);
			Rs.GetFieldValue((short)8, s);
			pItem->m_ProcessTimeOut = atoi(s);

			g_prefs.m_PDFProcessList.Add(*pItem);
			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}
	return TRUE;
}


BOOL CDatabaseManager::LoadPublicationChannels(int nID, CString &sErrorMessage)
{
	sErrorMessage = _T("");

	PUBLICATIONSTRUCT *pItem = g_prefs.GetPublicationStruct(nID);
	if (pItem == NULL)
		return FALSE;

	pItem->m_numberOfChannels = 0;

	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset Rs(m_pDB);

	CString sSQL, s;

	CString sList = _T("");
	sSQL.Format("SELECT DISTINCT ChannelID,PushTrigger,PubDateMoveDays,ReleaseDelay FROM PublicationChannels WHERE PublicationID=%d", nID);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return FALSE;
		}

		while (!Rs.IsEOF()) {

			Rs.GetFieldValue((short)0, s);
			pItem->m_channels[pItem->m_numberOfChannels].m_channelID = atoi(s);
			Rs.GetFieldValue((short)1, s);
			pItem->m_channels[pItem->m_numberOfChannels].m_pushtrigger = atoi(s);
			Rs.GetFieldValue((short)2, s);
			pItem->m_channels[pItem->m_numberOfChannels].m_pubdatemovedays = atoi(s);
			Rs.GetFieldValue((short)3, s);
			pItem->m_channels[pItem->m_numberOfChannels].m_releasedelay = atoi(s);
			Rs.MoveNext();
		}

		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}

	return TRUE;
}

CString CDatabaseManager::LoadNewPublicationName(int nID, CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return 0;

	CRecordset Rs(m_pDB);

	CString sSQL, s;
	CString sName = _T("");
	PUBLICATIONSTRUCT *pItem = g_prefs.GetPublicationStructNoDbLookup(nID);
	BOOL bNewEntry = pItem == NULL;

	sSQL.Format("SELECT DISTINCT [Name], PageFormatID, TrimToFormat, LatestHour, DefaultProofID, DefaultApprove, DefaultPriority, CustomerID, AutoPurgeKeepDays, EmailRecipient, EmailCC, EmailSubject, EmailBody, UploadFolder, Deadline, AnnumText, AllowUnplanned, ReleaseDays, ReleaseTime, PubdateMove, PubdateMoveDays, InputAlias, OutputALias, ExtendedAlias, PublisherID FROM PublicationNames WHERE PublicationID=%d", nID);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return _T("");
		}

		if (!Rs.IsEOF()) {
			int f = 0;
			if (bNewEntry)
				pItem = new PUBLICATIONSTRUCT();
			pItem->m_ID = nID;

			Rs.GetFieldValue((short)f++, pItem->m_name);
			Rs.GetFieldValue((short)f++, s);
			pItem->m_PageFormatID = atoi(s);
			Rs.GetFieldValue((short)f++, s);
			pItem->m_TrimToFormat = atoi(s);
			Rs.GetFieldValue((short)f++, s);
			pItem->m_LatestHour = atof(s);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_ProofID = atoi(s);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_Approve = atoi(s);
			Rs.GetFieldValue((short)f++, s);
			pItem->m_priority = atoi(s);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_customerID = atoi(s);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_autoPurgeKeepDays = atoi(s);

			Rs.GetFieldValue((short)f++, pItem->m_emailrecipient);
			Rs.GetFieldValue((short)f++, pItem->m_emailCC);
			Rs.GetFieldValue((short)f++, pItem->m_emailSubject);
			Rs.GetFieldValue((short)f++, pItem->m_emailBody);
			Rs.GetFieldValue((short)f++, pItem->m_uploadfolder);

			pItem->m_deadline = CTime(1975, 1, 1);

			try {
				CDBVariant DBvar;
				Rs.GetFieldValue((short)f++, DBvar, SQL_C_TIMESTAMP);
				CTime t(DBvar.m_pdate->year, DBvar.m_pdate->month, DBvar.m_pdate->day, DBvar.m_pdate->hour, DBvar.m_pdate->minute, DBvar.m_pdate->second);
				pItem->m_deadline = t;
			}
			catch (...)
			{
			}

			Rs.GetFieldValue((short)f++, pItem->m_annumtext);
			Rs.GetFieldValue((short)f++, s);
			pItem->m_allowunplanned = atoi(s);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_releasedays = atoi(s);

			Rs.GetFieldValue((short)f++, s);
			int n = atoi(s);
			pItem->m_releasetimehour = n / 100;
			pItem->m_releasetimeminute = n - pItem->m_releasetimehour;

			Rs.GetFieldValue((short)f++, s);
			pItem->m_pubdatemove = atoi(s) > 0;

			Rs.GetFieldValue((short)f++, s);
			pItem->m_pubdatemovedays = atoi(s);

			Rs.GetFieldValue((short)f++, pItem->m_alias);
			Rs.GetFieldValue((short)f++, pItem->m_outputalias);
			Rs.GetFieldValue((short)f++, pItem->m_extendedalias);

			Rs.GetFieldValue((short)f++, s);
			pItem->m_publisherID = atoi(s);
			if (bNewEntry)
				g_prefs.m_PublicationList.Add(*pItem);
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return _T("");
	}

	LoadPublicationChannels(nID, sErrorMessage);

	return sName;
}


CString CDatabaseManager::LoadNewEditionName(int nID, CString &sErrorMessage)
{
	sErrorMessage = _T( "");
	if (InitDB(sErrorMessage) == FALSE)
		return "";

	CRecordset Rs(m_pDB);

	CString sSQL;
	CString sEd = "";
	sSQL.Format("SELECT Name FROM EditionNames WHERE EditionID=%d", nID);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return "";
		}

		if (!Rs.IsEOF()) {
			ITEMSTRUCT *pItem = new ITEMSTRUCT();
			Rs.GetFieldValue((short)0, sEd);
			pItem->m_ID = nID;
			pItem->m_name = sEd;		
			g_prefs.m_EditionList.Add(*pItem);
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return "";
	}

	return sEd;
}

int CDatabaseManager::LoadNewEditionID(CString sName, CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return 0;

	CRecordset Rs(m_pDB);

	CString sSQL, s;
	int nID = 0;
	sSQL.Format("SELECT EditionID FROM EditionNames WHERE Name='%s'", sName);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return 0;
		}

		if (!Rs.IsEOF()) {
			ITEMSTRUCT *pItem = new ITEMSTRUCT();
			Rs.GetFieldValue((short)0, s);
			nID = atoi(s);
			pItem->m_ID = nID;
			pItem->m_name = sName;
			g_prefs.m_EditionList.Add(*pItem);
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return 0;
	}

	return nID;
}


CString CDatabaseManager::LoadNewSectionName(int nID, CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return "";

	CRecordset Rs(m_pDB);

	CString sSQL;
	CString sEd = "";
	sSQL.Format("SELECT Name FROM SectionNames WHERE SectionID=%d", nID);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return "";
		}

		if (!Rs.IsEOF()) {
			ITEMSTRUCT *pItem = new ITEMSTRUCT();
			Rs.GetFieldValue((short)0, sEd);
			pItem->m_ID = nID;
			pItem->m_name = sEd;
			g_prefs.m_SectionList.Add(*pItem);
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return "";
	}

	return sEd;
}

int CDatabaseManager::LoadNewSectionID(CString sName, CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return 0;

	CRecordset Rs(m_pDB);

	CString sSQL,s;
	int nID = 0;
	sSQL.Format("SELECT SectionID FROM SectionNames WHERE Name='%s'", sName);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return 0;
		}

		if (!Rs.IsEOF()) {
			ITEMSTRUCT *pItem = new ITEMSTRUCT();
			Rs.GetFieldValue((short)0, s);
			nID = atoi(s);
			pItem->m_ID = nID;
			pItem->m_name = sName;
			g_prefs.m_SectionList.Add(*pItem);
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return 0;
	}

	return nID;
}

CString CDatabaseManager::LoadNewPublisherName(int nID, CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return "";

	CRecordset Rs(m_pDB);

	CString sSQL;
	CString sName = "";
	sSQL.Format("SELECT PublisherName FROM PublisherNames WHERE PublisherID=%d", nID);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return "";
		}

		if (!Rs.IsEOF()) {
			ITEMSTRUCT *pItem = new ITEMSTRUCT();
			Rs.GetFieldValue((short)0, sName);
			pItem->m_ID = nID;
			pItem->m_name = sName;
			g_prefs.m_PublisherList.Add(*pItem);
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return "";
	}

	return sName;
}


BOOL CDatabaseManager::LoadEditionNameList(CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset Rs(m_pDB);

	g_prefs.m_EditionList.RemoveAll();
	CString sSQL;
	CString s;

	sSQL = _T("SELECT EditionID,Name FROM EditionNames");

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return FALSE;
		}

		while (!Rs.IsEOF()) {
			ITEMSTRUCT *pItem = new ITEMSTRUCT;
			Rs.GetFieldValue((short)0, s);
			pItem->m_ID = atoi(s);
			Rs.GetFieldValue((short)1, pItem->m_name);
			g_prefs.m_EditionList.Add(*pItem);
			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CDatabaseManager::LoadSectionNameList(CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset Rs(m_pDB);

	g_prefs.m_SectionList.RemoveAll();
	CString sSQL;
	CString s;

	sSQL = _T("SELECT SectionID,Name FROM SectionNames");

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return FALSE;
		}

		while (!Rs.IsEOF()) {
			ITEMSTRUCT *pItem = new ITEMSTRUCT;
			Rs.GetFieldValue((short)0, s);
			pItem->m_ID = atoi(s);
			Rs.GetFieldValue((short)1, pItem->m_name);
			g_prefs.m_SectionList.Add(*pItem);
			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}
	return TRUE;
}


BOOL CDatabaseManager::LoadFileServerList(CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset Rs(m_pDB);

	CString sSQL, s;

	sSQL.Format("SELECT [Name],ServerType,CCdatashare,Username,password,IP,Locationid,uselocaluser FROM FileServers ORDER BY ServerType");

	g_prefs.m_FileServerList.RemoveAll();
	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return FALSE;
		}

		while (!Rs.IsEOF()) {
			FILESERVERSTRUCT *pItem = new FILESERVERSTRUCT();

			Rs.GetFieldValue((short)0, pItem->m_servername);

			Rs.GetFieldValue((short)1, s);
			pItem->m_servertype = atoi(s);

			Rs.GetFieldValue((short)2, pItem->m_sharename);

			Rs.GetFieldValue((short)3, pItem->m_username);

			Rs.GetFieldValue((short)4, pItem->m_password);

			Rs.GetFieldValue((short)5, pItem->m_IP);

			pItem->m_IP.Trim();

			Rs.GetFieldValue((short)6, s);
			pItem->m_locationID = atoi(s);

			Rs.GetFieldValue((short)7, s);
			pItem->m_uselocaluser = atoi(s);

			g_prefs.m_FileServerList.Add(*pItem);
			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}
	return TRUE;
}


BOOL CDatabaseManager::LoadStatusList(CString sIDtable, STATUSLIST &v, CString &sErrorMessage)
{
	CString sSQL;

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset Rs(m_pDB);
	sSQL.Format("SELECT * FROM %s", sIDtable);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return FALSE;
		}

		while (!Rs.IsEOF()) {
			STATUSSTRUCT *pItem = new STATUSSTRUCT;
			CString s;
			Rs.GetFieldValue((short)0, s);
			pItem->m_statusnumber = atoi(s);
			Rs.GetFieldValue((short)1, pItem->m_statusname);
			Rs.GetFieldValue((short)2, pItem->m_statuscolor);
			v.Add(*pItem);
			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}
	return TRUE;
}




BOOL CDatabaseManager::LoadAliasList(CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset Rs(m_pDB);

	g_prefs.m_AliasList.RemoveAll();

	CString sSQL;
	sSQL = _T("SELECT Type,Longname,Shortname FROM InputAliases");

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return FALSE;
		}

		while (!Rs.IsEOF()) {
			ALIASSTRUCT *pItem = new ALIASSTRUCT();
			CString s;
			Rs.GetFieldValue((short)0, s);
			pItem->sType = s;
			Rs.GetFieldValue((short)1, s);
			pItem->sLongName = s;
			Rs.GetFieldValue((short)2, s);
			pItem->sShortName = s;
			g_prefs.m_AliasList.Add(*pItem);
			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}
	return TRUE;
}

CString CDatabaseManager::LoadSpecificAlias(CString sType, CString sLongName, CString &sErrorMessage)
{

	CString sShortName = sLongName;

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return sShortName;

	CRecordset Rs(m_pDB);

	CString sSQL;
	sSQL.Format("SELECT Shortname FROM InputAliases WHERE Type='%s' AND LongName='%s'", sType, sLongName);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return FALSE;
		}

		if (!Rs.IsEOF()) {
			Rs.GetFieldValue((short)0, sShortName);

			ALIASSTRUCT *pItem = new ALIASSTRUCT();
			pItem->sType = sType;
			pItem->sLongName = sLongName;
			pItem->sShortName = sShortName;
			g_prefs.FlushAlias(sType, sLongName);
			g_prefs.m_AliasList.Add(*pItem);
			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return sShortName;
	}
	return sShortName;
}


BOOL CDatabaseManager::LoadNewChannel(int nChannelID, CString &sErrorMessage)
{
	int foundJob = 0;

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CHANNELSTRUCT *pItem = g_prefs.GetChannelStructNoDbLookup(nChannelID);
	BOOL bNewEntry = pItem == NULL;
	if (bNewEntry) {
		pItem = new CHANNELSTRUCT();
		pItem->m_channelID = nChannelID;
	}

	CRecordset Rs(m_pDB);
	CString sSQL;

	sSQL.Format("SELECT ChannelID,Name,Enabled,ChannelNameAlias,PDFType,MergedPDF,PDFProcessID FROM ChannelNames WHERE WHERE ChannelID=%d", nChannelID);
	try {
		if (!Rs.Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return -1;
		}

		if (!Rs.IsEOF()) {
			CString s;
			int fld = 0;

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_channelID = atoi(s);

			Rs.GetFieldValue((short)fld++, pItem->m_channelname);

			Rs.GetFieldValue((short)fld++, s); // Enabled
			pItem->m_enabled = atoi(s);

			Rs.GetFieldValue((short)fld++, pItem->m_channelnamealias);

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_pdftype = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->m_mergePDF =atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->m_pdfprocessID = atoi(s);

			if (bNewEntry)
				g_prefs.m_ChannelList.Add(*pItem);

		}
		Rs.Close();
	}
	catch (CDBException* e) {
		LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
		sErrorMessage.Format("Query failed - %s (%s)", e->m_strError, sSQL);
		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
			return FALSE;
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}

	return TRUE;
}

/*
int CDatabaseManager::LoadNewChannelGroup(int nID, CString &sErrorMessage)
{
	int foundJob = 0;

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;


	CRecordset Rs(m_pDB);

	CString sSQL;

	sSQL.Format("SELECT ChannelGroupID,Name,TransmitNameFormat,TransmitNameDateFormat,TransmitNameUseAbbr,TransmitNameOptions,[PDFType],[MergedPDF],[EditionsToGenerate],[SendCommonPages],[SubFolderNamingConvension],[ChannelGroupNameAlias],MiscInt,MiscString FROM ChannelGroupNames WHERE ChannelGroupID=%d", nID);

	try {
		if (!Rs.Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", sSQL);
			return -1;
		}

		if (!Rs.IsEOF()) {
			CString s;
			int fld = 0;
			CHANNELGROUPSTRUCT *pItem = new CHANNELGROUPSTRUCT();

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_channelgroupID = atoi(s);

			Rs.GetFieldValue((short)fld++, pItem->m_channelgroupname);

			Rs.GetFieldValue((short)fld++, pItem->m_transmitnameconv);
			Rs.GetFieldValue((short)fld++, pItem->m_transmitdateformat);

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_transmitnameuseabbr = atoi(s);

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_transmitnameoptions = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->m_pdftype = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->m_mergePDF = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->m_editionstogenerate = atoi(s);

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_sendCommonPages = atoi(s);

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_subfoldernameconv = s;

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_channelgroupnamealias = s;

			Rs.GetFieldValue((short)fld++, s); // Miscint

			Rs.GetFieldValue((short)fld++, s); // Miscstring


			g_prefs.m_ChannelGroupList.Add(*pItem);

		}
		Rs.Close();
	}
	catch (CDBException* e) {
		LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
		sErrorMessage.Format("Query failed - %s (%s)", e->m_strError, sSQL);
		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
			return FALSE;
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}

	return TRUE;
}
*/


BOOL CDatabaseManager::LoadProofProcessList(CString &sErrorMessage)
{

	sErrorMessage = _T("");

	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset Rs(m_pDB);

	g_prefs.m_ProofProcessList.RemoveAll();

	CString sSQL, s;
	int fld = 0;

	sSQL = _T("SELECT ProofID,ProofName,Invert,Mirror,Rotate,Resolution,ResamplingMethod,ResamplingMethodBlack,SpecialMethodBlack,OutputType,JpegQuality,JpegThumbnail,PrinterDriver,OutputFileFormat,OutputFolder,PostCommand,IccEnable,IccInputProfile,IccMonitorProfile,IccPrinterProfile,IccProofProfile,IccRendering,IccProofRendering,IccEnableProof,IccOutOfGamut,LUTEnable,LUTFile,IncrementalProof,CropProof,GrayScale,WriteEachColor,UseFilter,FilterBlackOnly,FilterCoeff,ColorWaitTime,ThumbnailResolution,UseBoundingBox,BoundingBoxSizeX,BoundingBoxSizeY,BoundingBoxPosX,BoundingBoxPosY FROM ProofConfigurations");
	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return FALSE;
		}

		while (!Rs.IsEOF()) {
			PROOFPROCESSSTRUCT *pItem = new PROOFPROCESSSTRUCT();
			fld = 0;

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_ID = atoi(s);
			Rs.GetFieldValue((short)fld++, pItem->m_proofname);

			// resample
			Rs.GetFieldValue((short)fld++, s);
			pItem->resample.m_invert = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->resample.m_mirror = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->resample.m_rotate = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->resample.m_resolution = atof(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->resample.m_resamplingmethod = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->resample.m_resamplingmethodblack = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->resample.m_specialmethodblack = atoi(s);

			Rs.GetFieldValue((short)fld++, s);
			int n = atoi(s);
			pItem->output.m_outputtype = n >> 2;
			pItem->output.m_softproof = (n & 0x01) > 0 ? TRUE : FALSE;
			pItem->output.m_hardproof = (n & 0x02) > 0 ? TRUE : FALSE;

			Rs.GetFieldValue((short)fld++, s);
			pItem->output.m_jpegquality = atoi(s);
			Rs.GetFieldValue((short)fld++, s);

			int nn = atoi(s);
			pItem->output.m_jpegthumbnail = (nn & 0x01) > 0 ? TRUE : FALSE;
			pItem->output.m_cmykjpeg = (nn & 0x02) > 0 ? TRUE : FALSE;
			pItem->resample.m_maxinkwarning = (nn & 0x04) > 0 ? TRUE : FALSE;

			pItem->resample.m_maxinklimit = (nn >> 3) & 0xFFF;
			pItem->m_maxinkwarnratio = (nn >> 16) & 0xFFFF;


			Rs.GetFieldValue((short)fld++, s);
			pItem->output.m_printerdriver = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->output.m_outputfileformat = atoi(s);
			Rs.GetFieldValue((short)fld++, pItem->output.m_outputfolder);
			Rs.GetFieldValue((short)fld++, pItem->output.m_postcommand);

			// icc
			Rs.GetFieldValue((short)fld++, s);
			pItem->icc.m_icc_enable = atoi(s);
			Rs.GetFieldValue((short)fld++, pItem->icc.m_icc_input_profile);
			Rs.GetFieldValue((short)fld++, pItem->icc.m_icc_monitor_profile);
			Rs.GetFieldValue((short)fld++, pItem->icc.m_icc_printer_profile);
			Rs.GetFieldValue((short)fld++, pItem->icc.m_icc_proof_profile);
			Rs.GetFieldValue((short)fld++, s);
			pItem->icc.m_icc_rendering = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->icc.m_icc_proof_rendering = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->icc.m_icc_enable_proof = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->icc.m_icc_out_of_gamut = atoi(s);

			// LUT
			Rs.GetFieldValue((short)fld++, s);
			pItem->linearization.m_enablelinearization = atoi(s);
			Rs.GetFieldValue((short)fld++, pItem->linearization.m_LUTfile);

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_incrementalproof = atoi(s);

			Rs.GetFieldValue((short)fld++, s);
			pItem->output.m_useboundingbox = atoi(s);

			Rs.GetFieldValue((short)fld++, s);
			pItem->resample.m_grayscale = atoi(s);

			Rs.GetFieldValue((short)fld++, s);

			nn = atoi(s);
			pItem->resample.m_writeeachcolor = (nn & 0x01) > 0 ? TRUE : FALSE;
			pItem->m_waitforblack = (nn & 0x02) > 0 ? TRUE : FALSE;
			int flash1 = (nn & 0x04) > 0 ? TRUE : FALSE;
			pItem->m_generatereadview = (nn & 0x08) > 0 ? TRUE : FALSE;
			pItem->m_readviewpageformatcrop = (nn & 0x10) > 0 ? TRUE : FALSE;
			int flash2 = (nn & 0x20) > 0 ? TRUE : FALSE;
			pItem->m_showframe = (nn & 0x40) > 0 ? TRUE : FALSE;
			pItem->m_showframemode = (nn & 0x80) > 0 ? TRUE : FALSE;


			if (nn & 0x0100)
				pItem->m_readviewpageformatcrop = 2;

			pItem->m_showimageareaframe = (nn & 0x200) > 0 ? TRUE : FALSE;
			pItem->m_usedensitymask = (nn & 0x400) > 0 ? TRUE : FALSE;
			pItem->m_waitforcmyk = (nn & 0x800) > 0 ? TRUE : FALSE;



			pItem->m_generateforflash = 0;
			if (flash1 > 0 || flash2 > 0)
				pItem->m_generateforflash = flash1 ? 1 : 2;

			Rs.GetFieldValue((short)fld++, s);
			nn = atoi(s);
			pItem->filter.m_usefilter = (nn & 0x01) > 0 ? TRUE : FALSE;
			pItem->m_rotateoddpages = (nn & 0x02) > 0 ? TRUE : FALSE;
			pItem->m_writepagecomment = (nn & 0x04) > 0 ? TRUE : FALSE;
			pItem->m_generateheatmap = (nn & 0x08) > 0 ? TRUE : FALSE;
			pItem->m_colorformonosubeditions = (nn & 0x10) > 0 ? TRUE : FALSE;
			pItem->m_alwayssameversion = (nn & 0x20) > 0 ? TRUE : FALSE;

			Rs.GetFieldValue((short)fld++, s);
			pItem->filter.m_filterblackonly = atoi(s);

			Rs.GetFieldValue((short)fld++, s);
			CStringArray sArr;
			n = g_util.StringSplitter(s, ",", sArr);
			if (n > 25)
				n = 25;
			for (int i = 0; i < n; i++)
				pItem->filter.m_filtercoeff[i] = atof(sArr[i]);

			Rs.GetFieldValue((short)fld++, s);
			pItem->m_colorwaittime = atoi(s);

			Rs.GetFieldValue((short)fld++, s);
			pItem->output.m_thumbnailresolution = atoi(s);

			Rs.GetFieldValue((short)fld++, s);
			pItem->output.m_useboundingbox = atoi(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->output.m_boundingboxsizex = atof(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->output.m_boundingboxsizey = atof(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->output.m_boundingboxposx = atof(s);
			Rs.GetFieldValue((short)fld++, s);
			pItem->output.m_boundingboxposy = atof(s);
			//g_prefs.LoadResampleIni(pItem);

			g_prefs.m_ProofProcessList.Add(*pItem);
			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return FALSE;
	}
	return TRUE;
}




CTime CDatabaseManager::GetDatabaseTime(CString &sErrorMessage)
{
	CString sSQL;
	CTime tm(1975, 1, 1, 0, 0, 0);
	CDBVariant DBvar;

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return tm;

	CRecordset Rs(m_pDB);

	sSQL.Format("SELECT GETDATE()");

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return FALSE;
		}

		if (!Rs.IsEOF()) {
			CDBVariant DBvar;
			Rs.GetFieldValue((short)0, DBvar, SQL_C_TIMESTAMP);
			CTime tPubTime(DBvar.m_pdate->year, DBvar.m_pdate->month, DBvar.m_pdate->day, DBvar.m_pdate->hour, DBvar.m_pdate->minute, DBvar.m_pdate->second);
			tm = tPubTime;
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error: %s   (%s)", e->m_strError, sSQL);

		e->Delete();
		Rs.Close();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return tm;
	}
	return tm;
}



BOOL CDatabaseManager::ResetProofStatus(CString &sErrorMessage)
{
	CString sSQL;
	sErrorMessage = _T("");

	sSQL.Format("UPDATE PageTable SET ProofStatus=0 WHERE (Status=5 OR Status=6) AND (SoftProofProcessID=%d OR SoftProofProcessID=0)", g_prefs.m_instancenumber);

	int nRetries = 3; //g_prefs.m_nQueryRetries;
	BOOL bSuccess = FALSE;
	while (!bSuccess && nRetries-- > 0) {
		if (InitDB(sErrorMessage) == FALSE) {
			Sleep(g_prefs.m_QueryBackoffTime);
			continue;
		}

		try {
			m_pDB->ExecuteSQL(sSQL);
			bSuccess = TRUE;
		}
		catch (CDBException* e) {
			sErrorMessage.Format(_T("Update failed - %s"), e->m_strError);
			LogprintfDB(_T("Non-fatal Error : Retry %d - %s   (%s)"), nRetries, e->m_strError, sSQL);
			try {
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
		}
	}

	return bSuccess;
}

BOOL CDatabaseManager::ResetConvertStatus(int nPDFmode, int nInstanceNumber, CString &sErrorMessage)
{
	CString sSQL;
	sErrorMessage = _T("");

	if (nPDFmode == POLLINPUTTYPE_LOWRESPDF)
		sSQL.Format("UPDATE PageTable SET StatusPdfLowres=0,CheckSumPdfLowres=0 WHERE (StatusPdfLowres=5 OR StatusPdfLowres=6) AND (ImagingProcessID=%d OR ImagingProcessID=0)", nInstanceNumber);
	else if (nPDFmode == POLLINPUTTYPE_PRINTPDF)
		sSQL.Format("UPDATE PageTable SET StatusPdfPrint=0,CheckSumPdfPrint=0 WHERE (StatusPdfPrint=5 OR StatusPdfPrint=6) AND (HardProofProcessID=%d OR HardProofProcessID=0)", nInstanceNumber);
	else //if (nPDFmode == POLLINPUTTYPE_HIGHRESPDF)
		sSQL.Format("UPDATE PageTable SET StatusPdfHighres=0,CheckSumPdfHires=0 WHERE (StatusPdfHighres=5 OR StatusPdfHighres=6) AND (InkProcessID=%d OR InkProcessID=0)", nInstanceNumber);

	int nRetries = 3; //g_prefs.m_nQueryRetries;
	BOOL bSuccess = FALSE;
	while (!bSuccess && nRetries-- > 0) {
		if (InitDB(sErrorMessage) == FALSE) {
			Sleep(g_prefs.m_QueryBackoffTime);
			continue;
		}

		try {
			m_pDB->ExecuteSQL(sSQL);
			bSuccess = TRUE;
		}
		catch (CDBException* e) {
			sErrorMessage.Format(_T("Update failed - %s"), e->m_strError);
			LogprintfDB(_T("Non-fatal Error : Retry %d - %s   (%s)"), nRetries, e->m_strError, sSQL);
			try {
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
		}
	}

	return bSuccess;
}

BOOL CDatabaseManager::LookupConvertJob(CJobAttributes &Job, int nToPDFType, CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CRecordset *Rs = NULL;
	CString sSQL, s;

	sSQL.Format("{CALL spConvertLookupNextJob (%d) }", nToPDFType);

	Job.m_mastercopyseparationset = 0;
	BOOL bSuccess = FALSE;

	int nRetries = g_prefs.m_nQueryRetries;

	while (!bSuccess && nRetries-- > 0) {

		try {
			if (InitDB(sErrorMessage) == FALSE) {
				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				continue;
			}

			Rs = new CRecordset(m_pDB);

			if (!Rs->Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
				sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
				Sleep(g_prefs.m_QueryBackoffTime);
				delete Rs;
				Rs = NULL;
				continue;
			}
			if (!Rs->IsEOF()) {
				Rs->GetFieldValue((short)0, s);
				Job.m_mastercopyseparationset = atoi(s);

				Rs->GetFieldValue((short)1, s);
				Job.m_pdfprocessID = atoi(s);
			}
			Rs->Close();
			bSuccess = TRUE;
		}

		catch (CDBException* e) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
			LogprintfDB("Error try %d : %s   (%s)", nRetries, e->m_strError, sSQL);
			e->Delete();

			try {
				if (Rs->IsOpen())
					Rs->Close();
				delete Rs;
				Rs = NULL;
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
			continue;
		}


		if (Rs != NULL)
			delete Rs;

		if (Job.m_mastercopyseparationset == 0)
			return bSuccess;

		sSQL.Format("{CALL spProofLookUpNextJob2 (%d)}", Job.m_mastercopyseparationset);
		//  P.ColorID, P.PublicationID, P.SectionID, P.EditionID, P.IssueID, P.PageName, P.ProofID
		try {
			if (InitDB(sErrorMessage) == FALSE) {
				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				continue;
			}

			Rs = new CRecordset(m_pDB);

			if (!Rs->Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
				sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
				Sleep(g_prefs.m_QueryBackoffTime);
				delete Rs;
				Rs = NULL;
				continue;
			}
			int nFields = Rs->GetODBCFieldCount();

			if (!Rs->IsEOF()) {
				int fld = 0;

				Rs->GetFieldValue((short)fld++, s);
				int nThisColorID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_publicationID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_sectionID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_editionID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_pressrunID = atoi(s);

				Rs->GetFieldValue((short)fld++, Job.m_pagename);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_pagination = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_prooftemplateID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_templateID = atoi(s);


				CDBVariant DBvar;
				Rs->GetFieldValue((short)fld++, DBvar, SQL_C_TIMESTAMP);
				CTime tPubTime(DBvar.m_pdate->year, DBvar.m_pdate->month, DBvar.m_pdate->day, DBvar.m_pdate->hour, DBvar.m_pdate->minute, DBvar.m_pdate->second);
				Job.m_pubdate = tPubTime;

				Rs->GetFieldValue((short)fld++, s);
				Job.m_pagetype = atoi(s);


				Rs->GetFieldValue((short)fld++, s);
				Job.m_jobname = s;
				Job.m_filename = s;
				Job.m_ccdatafilename = g_util.GetFileName(s, TRUE);

				Rs->MoveNext();
			}

			Rs->Close();
			bSuccess = TRUE;

		}
		catch (CDBException* e) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
			LogprintfDB("Error try %d : %s   (%s)", nRetries, e->m_strError, sSQL);
			e->Delete();
			try {
				if (Rs->IsOpen())
					Rs->Close();
				delete Rs;
				Rs = NULL;
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
		}


		if (Rs != NULL)
			delete Rs;
	}

	return bSuccess;
}


BOOL CDatabaseManager::LookupProofJob(CJobAttributes &Job, int &nPDFType, CString &sErrorMessage)
{

	nPDFType = POLLINPUTTYPE_HIRESPDF;

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	CUtils util;
	CRecordset *Rs = NULL;
	CString sSQL, s;

	sSQL.Format("{CALL spProofLookupNextJob }");
	
	Job.m_mastercopyseparationset = 0;
	BOOL bSuccess = FALSE;

	int nRetries = g_prefs.m_nQueryRetries;

	if (g_prefs.m_logToFile > 2)
		util.LogprintfResample("INFO:  Resample query:%s", sSQL);

	while (!bSuccess && nRetries-- > 0) {

		try {
			if (InitDB(sErrorMessage) == FALSE) {
				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				continue;
			}

			Rs = new CRecordset(m_pDB);

			if (!Rs->Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
				sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
				Sleep(g_prefs.m_QueryBackoffTime);
				delete Rs;
				Rs = NULL;
				continue;
			}
			if (!Rs->IsEOF()) {
				Rs->GetFieldValue((short)0, s);
				Job.m_mastercopyseparationset = atoi(s);
				
				Rs->GetFieldValue((short)1, s);
				nPDFType = atoi(s);

			
			}
			Rs->Close();
			bSuccess = TRUE;
		}
		catch (CDBException* e) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
			LogprintfDB("Error try %d : %s   (%s)", nRetries, e->m_strError, sSQL);
			e->Delete();

			try {
				if (Rs->IsOpen())
					Rs->Close();
				delete Rs;
				Rs = NULL;
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
			continue;
		}

		
		
		if (Rs != NULL)
			delete Rs;

		if (Job.m_mastercopyseparationset == 0)
			return bSuccess;

		////////////////////////////
		// Page separation(s) found
		////////////////////////////
		int nColors = 0;
		sSQL.Format("{CALL spProofLookUpNextJob2 (%d)}", Job.m_mastercopyseparationset);
		//  P.ColorID, P.PublicationID, P.SectionID, P.EditionID, P.IssueID, P.PageName, P.ProofID
		try {
			if (InitDB(sErrorMessage) == FALSE) {
				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				continue;
			}

			Rs = new CRecordset(m_pDB);

			if (!Rs->Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
				sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
				Sleep(g_prefs.m_QueryBackoffTime);
				delete Rs;
				Rs = NULL;
				continue;
			}
			int nFields = Rs->GetODBCFieldCount();

			if (!Rs->IsEOF()) {
				int fld = 0;

				Rs->GetFieldValue((short)fld++, s);
				int nThisColorID = atoi(s);
				
				Rs->GetFieldValue((short)fld++, s);
				Job.m_publicationID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_sectionID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_editionID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_pressrunID = atoi(s);

				Rs->GetFieldValue((short)fld++, Job.m_pagename);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_pagination = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_prooftemplateID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				Job.m_templateID = atoi(s);


				CDBVariant DBvar;
				Rs->GetFieldValue((short)fld++, DBvar, SQL_C_TIMESTAMP);
				CTime tPubTime(DBvar.m_pdate->year, DBvar.m_pdate->month, DBvar.m_pdate->day, DBvar.m_pdate->hour, DBvar.m_pdate->minute, DBvar.m_pdate->second);
				Job.m_pubdate = tPubTime;

				Rs->GetFieldValue((short)fld++, s);
				Job.m_pagetype = atoi(s);


				Rs->GetFieldValue((short)fld++, s);
				Job.m_jobname = s;
				Job.m_filename = s;
				Job.m_ccdatafilename = util.GetFileName(s, TRUE);
				
				Rs->MoveNext();
			}
	
			Rs->Close();
			bSuccess = TRUE;

		}
		catch (CDBException* e) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
			LogprintfDB("Error try %d : %s   (%s)", nRetries, e->m_strError, sSQL);
			e->Delete();
			try {
				if (Rs->IsOpen())
					Rs->Close();
				delete Rs;
				Rs = NULL;
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
		}



		if (Rs != NULL)
			delete Rs;

	
	}

	return bSuccess;

}


BOOL CDatabaseManager::UpdateStatusProof(int nMasterCopySeparationSet, int nProofStatus, CString sErr, BOOL &bUpdateOK, CString &sErrorMessage)
{
	CString sSQL, s;
	CRecordset *Rs = NULL;

	BOOL bSuccess = FALSE;
    bUpdateOK = FALSE;

	int nRetries = g_prefs.m_nQueryRetries;
	sErrorMessage = _T("");


	sSQL.Format("{CALL spProofUpdateStatus (%d,%d,'%s',%d)}", nMasterCopySeparationSet, nProofStatus, sErr, g_prefs.m_instancenumber);

	while (!bSuccess && nRetries-- > 0) {

		try {
			if (InitDB(sErrorMessage) == FALSE) {
				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				continue;
			}

			Rs = new CRecordset(m_pDB);

			if (!Rs->Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
				sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				delete Rs;
				Rs = NULL;
				continue;
			}

			if (!Rs->IsEOF()) {
				Rs->GetFieldValue((short)0, s);
				bUpdateOK = atoi(s);
			}
			Rs->Close();

			bSuccess = TRUE;
			break;
		}

		catch (CDBException* e) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
			LogprintfDB("Error try %d : %s   (%s)", nRetries, e->m_strError, sSQL);
			e->Delete();
			try {
				if (Rs->IsOpen())
					Rs->Close();
				delete Rs;
				Rs = NULL;
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
		}
	}

	if (Rs != NULL)
		delete Rs;

	return bSuccess;	
}


BOOL CDatabaseManager::UpdateStatusProofReadView(int nMasterCopySeparationSetLeft, int nMasterCopySeparationSetRight, int nProofStatus, CString sErr, BOOL &bUpdateOK, CString &sErrorMessage)
{
	CString sSQL, s;
	CRecordset *Rs = NULL;
	BOOL bSuccess = FALSE;
	bUpdateOK = FALSE;
	int nRetries = g_prefs.m_nQueryRetries;

	sErrorMessage = _T("");

	sSQL.Format("{CALL spProofUpdateStatusReadView (%d,%d,%d,'%s',%d)}", nMasterCopySeparationSetLeft, nMasterCopySeparationSetRight, nProofStatus, sErr, g_prefs.m_instancenumber);

	while (!bSuccess && nRetries-- > 0) {

		try {
			if (InitDB(sErrorMessage) == FALSE) {
				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				continue;
			}

			Rs = new CRecordset(m_pDB);

			if (!Rs->Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
				sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				delete Rs;
				Rs = NULL;
				continue;
			}

			if (!Rs->IsEOF()) {
				Rs->GetFieldValue((short)0, s);
				bUpdateOK = atoi(s);
			}
			Rs->Close();

			bSuccess = TRUE;
			break;
		}

		catch (CDBException* e) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
			LogprintfDB("Error try %d : %s   (%s)", nRetries, e->m_strError, sSQL);
			e->Delete();
			try {
				if (Rs->IsOpen())
					Rs->Close();
				delete Rs;
				Rs = NULL;
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
		}
	}

	if (Rs != NULL)
		delete Rs;
	
	return bSuccess;
}


BOOL  CDatabaseManager::GetPolledStatus(int nMasterCopySeparationSet, BOOL &bIsPolled, CString &sErrorMessage)
{
	CString sSQL, s;

	sErrorMessage = _T("");

	bIsPolled = FALSE;
	CRecordset *Rs = NULL;

	sSQL.Format("SELECT TOP 1  MasterCopySeparationSet FROM PageTable WITH (NOLOCK) WHERE MasterCopySeparationSet=%d AND Active>0 AND Status>=10 AND Dirty=0", nMasterCopySeparationSet);

	BOOL bSuccess = FALSE;
	int nRetries = g_prefs.m_nQueryRetries;

	while (!bSuccess && nRetries-- > 0) {

		try {

			if (InitDB(sErrorMessage) == FALSE) {
				Sleep(g_prefs.m_QueryBackoffTime);
				continue;
			}

			Rs = new CRecordset(m_pDB);


			if (!Rs->Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
				sErrorMessage.Format("Query failed - %s", sSQL);

				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				delete Rs;
				Rs = NULL;
				continue;
			}

			if (!Rs->IsEOF()) {
				Rs->GetFieldValue((short)0, s);
				bIsPolled = atoi(s) > 0;
			}

			Rs->Close();
			bSuccess = TRUE;
			break;
		}

		catch (CDBException* e) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
			LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
			e->Delete();
			try {
				if (Rs->IsOpen())
					Rs->Close();
				delete Rs;
				Rs = NULL;
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			::Sleep(g_prefs.m_QueryBackoffTime);
		}
	}

	if (Rs != NULL)
		delete Rs;


	return bSuccess;
}

BOOL  CDatabaseManager::GetChannelsForMasterSet(int nMasterCopySeparationSet, int nPDFType, CUIntArray &aChannelIDList, CString &sErrorMessage)
{
	CString sSQL, s;

	sErrorMessage = _T("");

	aChannelIDList.RemoveAll();
	CRecordset *Rs = NULL;

	sSQL.Format("SELECT DISTINCT CS.ChannelID FROM ChannelStatus CS WITH (NOLOCK) INNER JOIN ChannelNames C WITH (NOLOCK) ON C.ChannelID=CS.ChannelID INNER JOIN ChannelGroupNames CG WITH (NOLOCK) ON CG.ChannelGroupID=C.ChannelGroupID WHERE CS.MasterCopySeparationSet=%d AND CG.PDFType=%d ", nMasterCopySeparationSet, nPDFType);

	BOOL bSuccess = FALSE;
	int nRetries = g_prefs.m_nQueryRetries;

	while (!bSuccess && nRetries-- > 0) {

		try {

			if (InitDB(sErrorMessage) == FALSE) {
				Sleep(g_prefs.m_QueryBackoffTime);
				continue;
			}

			Rs = new CRecordset(m_pDB);


			if (!Rs->Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
				sErrorMessage.Format("Query failed - %s", sSQL);

				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				delete Rs;
				Rs = NULL;
				continue;
			}

			while (!Rs->IsEOF()) {
				Rs->GetFieldValue((short)0, s);
				aChannelIDList.Add(atoi(s));

				Rs->MoveNext();
			}

			Rs->Close();
			bSuccess = TRUE;
			break;
		}

		catch (CDBException* e) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
			LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
			e->Delete();
			try {
				if (Rs->IsOpen())
					Rs->Close();
				delete Rs;
				Rs = NULL;
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			::Sleep(g_prefs.m_QueryBackoffTime);
		}
	}

	if (Rs != NULL)
		delete Rs;


	return bSuccess;
}



BOOL CDatabaseManager::UpdateCRC(int nMasterCopySeparationSet, int nPDFtype, int nCRC, CString &sErrorMessage)
{
	CString sSQL;

	sErrorMessage = _T("");

	BOOL bSuccess = FALSE;
	int nRetries = g_prefs.m_nQueryRetries;

	if (nPDFtype == POLLINPUTTYPE_LOWRESPDF)
		sSQL.Format("UPDATE PageTable SET CheckSumPdfLowres=%d WHERE MasterCopySeparationSet=%d", nCRC, nMasterCopySeparationSet);
	else if (nPDFtype == POLLINPUTTYPE_PRINTPDF)
		sSQL.Format("UPDATE PageTable SET CheckSumPdfPrint=%d WHERE MasterCopySeparationSet=%d", nCRC, nMasterCopySeparationSet);
	else // if (nPDFtype == POLLINPUTTYPE_HIRESPDF)
		sSQL.Format("UPDATE PageTable SET CheckSumPdfHighRes=%d WHERE MasterCopySeparationSet=%d", nCRC, nMasterCopySeparationSet);


	while (!bSuccess && nRetries-- > 0) {

		if (InitDB(sErrorMessage) == FALSE) {
			Sleep(g_prefs.m_QueryBackoffTime);
			continue;
		}
		try {
			m_pDB->ExecuteSQL(sSQL);
			bSuccess = TRUE;
			break;
		}
		catch (CDBException* e) {
			sErrorMessage.Format("Update failed - %s", e->m_strError);
			LogprintfDB("Error - try %d : %s   (%s)", nRetries, e->m_strError, sSQL);
			try {
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
		}
	}

	return bSuccess;

}



BOOL CDatabaseManager::GetJobDetails(CJobAttributes *pJob, int nCopySeparationSet,  CString &sErrorMessage)
{
	CUtils util;
	CString sSQL, s;

	sSQL.Format("SELECT TOP 1 PublicationID,PubDate,EditionID,SectionID,PageName,Pagination,PageIndex,ColorID,Version,Comment,MiscString1,MiscString2,MiscString3,FileName,LocationID,ProductionID FROM PageTable WITH (NOLOCK) WHERE CopySeparationSet=%d AND Dirty=0 ORDER BY PageIndex", nCopySeparationSet);

	BOOL bSuccess = FALSE;
	CRecordset *Rs = NULL;

	pJob->m_copyflatseparationset = nCopySeparationSet;

	sErrorMessage = _T("");
	int nRetries = g_prefs.m_nQueryRetries;

	while (bSuccess == FALSE && nRetries-- > 0) {
		try {
			if (InitDB(sErrorMessage) == FALSE) {
				Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
				continue;
			}

			Rs = new CRecordset(m_pDB);

			if (!Rs->Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
				sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
				Sleep(g_prefs.m_QueryBackoffTime);
				delete Rs;
				Rs = NULL;
				continue;
			}

			int nFields = Rs->GetODBCFieldCount();

			if (!Rs->IsEOF()) {
				int fld = 0;
				Rs->GetFieldValue((short)fld++, s);
				pJob->m_publicationID = atoi(s);


				CDBVariant DBvar;
				try {
					Rs->GetFieldValue((short)fld++, DBvar, SQL_C_TIMESTAMP);
					CTime t(DBvar.m_pdate->year, DBvar.m_pdate->month, DBvar.m_pdate->day, DBvar.m_pdate->hour, DBvar.m_pdate->minute, DBvar.m_pdate->second);
					pJob->m_pubdate = t;
				}
				catch (CException *ex)
				{
				}

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_editionID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_sectionID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_pagename = s;

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_pagination = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_pageindex = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_colorname = _T("PDF");		// HARDCODED

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_version = atoi(s);


				Rs->GetFieldValue((short)fld++, s);
				pJob->m_comment = s;

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_miscstring1 = s;

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_miscstring2 = s;

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_miscstring3 = s;

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_filename = s;

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_locationID = atoi(s);

				Rs->GetFieldValue((short)fld++, s);
				pJob->m_productionID = atoi(s);
			}
			Rs->Close();
			bSuccess = TRUE;
		}
		catch (CDBException* e) {
			sErrorMessage.Format("Query failed - %s", e->m_strError);
			LogprintfDB("Error try %d : %s   (%s)", nRetries, e->m_strError, sSQL);
			e->Delete();

			try {
				if (Rs->IsOpen())
					Rs->Close();
				delete Rs;
				Rs = NULL;
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
		}
	}

	if (Rs != NULL)
		delete Rs;

	return bSuccess;
}

int CDatabaseManager::GetChannelCRC(int nMasterCopySeparationSet, int nInputMode, CString &sErrorMessage)
{
	sErrorMessage =  _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;

	CRecordset Rs(m_pDB);
	CString sSQL, s;

	int nCRC = 0;

	sSQL.Format("SELECT TOP 1 CheckSumPdfPrint FROM PageTable WITH (NOLOCK) WHERE MasterCopySeparationSet=%d", nMasterCopySeparationSet);
	
	if (nInputMode == POLLINPUTTYPE_HIRESPDF)
		sSQL.Format("SELECT TOP 1 CheckSumPdfHighRes FROM PageTable WITH (NOLOCK) WHERE MasterCopySeparationSet=%d", nMasterCopySeparationSet);
	else if (nInputMode == POLLINPUTTYPE_LOWRESPDF)
		sSQL.Format("SELECT TOP 1 CheckSumPdfLowRes FROM PageTable WITH (NOLOCK) WHERE MasterCopySeparationSet=%d", nMasterCopySeparationSet);


	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return -1;
		}

		if (!Rs.IsEOF()) {
			Rs.GetFieldValue((short)0, s);
			nCRC = atoi(s);
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return -1;
	}

	return nCRC;
}

int CDatabaseManager::GetChannelStatus(int nMasterCopySeparationSet, int nChannelID, CString &sErrorMessage)
{
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;

	CRecordset Rs(m_pDB);
	CString sSQL, s;

	int nStatus = -1;


	sSQL.Format("SELECT TOP 1 Status FROM ChannelStatus WITH (NOLOCK) WHERE MasterCopySeparationSet=%d AND ChannelID=%d", nMasterCopySeparationSet, nChannelID);

	try {
		if (!Rs.Open(CRecordset::snapshot | CRecordset::forwardOnly, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", sSQL);
			return -1;
		}

		if (!Rs.IsEOF()) {
			Rs.GetFieldValue((short)0, s);
			nStatus = atoi(s);
		}
		Rs.Close();
	}
	catch (CDBException* e) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", e->m_strError);
		LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
		e->Delete();
		Rs.Close();

		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch (CDBException* e) {
			// So what..! Go on!;
		}
		return -1;
	}

	return nStatus;
}


BOOL CDatabaseManager::UpdateConvertStatus(int nStatus, int nMasterCopySeparationSet, int nPDFmode, CString sMessage, CString sFileName, int nInstanceNumber, CString &sErrorMessage)
{
	CString sSQL, s;

	sErrorMessage = _T("");
	
	if (nPDFmode == POLLINPUTTYPE_LOWRESPDF)
		sSQL.Format("UPDATE PageTable SET StatusPdfLowres=%d,ImagingProcessID=%d WHERE MasterCopySeparationSet=%d", nStatus, nInstanceNumber,nMasterCopySeparationSet);
	else if (nPDFmode == POLLINPUTTYPE_HIRESPDF)
		sSQL.Format("UPDATE PageTable SET StatusPdfHighres=%d,InkProcessID=%d WHERE MasterCopySeparationSet=%d", nStatus, nInstanceNumber, nMasterCopySeparationSet);
	else // (nPDFmode == POLLINPUTTYPE_PRINTPDF)
		sSQL.Format("UPDATE PageTable SET StatusPdfPrint=%d,HardProofProcessID=%d WHERE MasterCopySeparationSet=%d", nStatus, nInstanceNumber, nMasterCopySeparationSet);
	BOOL bSuccess = FALSE;
	int nRetries = g_prefs.m_nQueryRetries;

	while (!bSuccess && nRetries-- > 0) {

		if (InitDB(sErrorMessage) == FALSE) {
			Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
			continue;
		}

		try {
			m_pDB->ExecuteSQL(sSQL);
			bSuccess = TRUE;
			break;
		}
		catch (CDBException* e) {
			sErrorMessage.Format("Insert failed - %s", e->m_strError);
			LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
			try {
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
		}
	}

	CUIntArray aChannelIDList;

	if (GetChannelsForMasterSet(nMasterCopySeparationSet, nPDFmode, aChannelIDList, sErrorMessage) == FALSE)
		return FALSE;
	for (int i = 0; i < aChannelIDList.GetCount(); i++) {
		if (UpdateChannelStatus(CHANNELSTATUSTYPE_PROCESSING, nMasterCopySeparationSet, 0, aChannelIDList[i], nStatus, sMessage, sFileName, sErrorMessage) == FALSE)
			return FALSE;
	}

	return bSuccess;
}


BOOL CDatabaseManager::UpdateConvertCRC(int nCRC, int nMasterCopySeparationSet, int nPDFmode,  CString &sErrorMessage)
{
	CString sSQL, s;

	sErrorMessage = _T("");

	if (nPDFmode == POLLINPUTTYPE_LOWRESPDF)
		sSQL.Format("UPDATE PageTable SET CheckSumPdfLowres=%d WHERE MasterCopySeparationSet=%d", nCRC, nMasterCopySeparationSet);
	else if (nPDFmode == POLLINPUTTYPE_HIRESPDF)
		sSQL.Format("UPDATE PageTable SET CheckSumPdfHighRes=%d WHERE MasterCopySeparationSet=%d", nCRC, nMasterCopySeparationSet);
	else // (nPDFmode == POLLINPUTTYPE_PRINTPDF)
		sSQL.Format("UPDATE PageTable SET CheckSumPdfPrint=%d WHERE MasterCopySeparationSet=%d", nCRC, nMasterCopySeparationSet);
	BOOL bSuccess = FALSE;
	int nRetries = g_prefs.m_nQueryRetries;

	while (!bSuccess && nRetries-- > 0) {

		if (InitDB(sErrorMessage) == FALSE) {
			Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
			continue;
		}

		try {
			m_pDB->ExecuteSQL(sSQL);
			bSuccess = TRUE;
			break;
		}
		catch (CDBException* e) {
			sErrorMessage.Format("Insert failed - %s", e->m_strError);
			LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
			try {
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
		}
	}


	return bSuccess;
}



BOOL CDatabaseManager::UpdateChannelStatus(int nStatusType, int nMasterCopySeparationSet, int nProductionID, int nChannelID, int nStatus, CString sMessage, CString sFileName, CString &sErrorMessage)
{

	CString sSQL;
	BOOL bSuccess = FALSE;
	int nRetries = g_prefs.m_nQueryRetries;

	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return FALSE;

	sSQL.Format("{ CALL spUpdateChannelStatus (%d, %d,%d,%d,%d,'%s','%s') }", nStatusType, nMasterCopySeparationSet, nProductionID, nChannelID, nStatus, sMessage, sFileName);

	while (!bSuccess && nRetries-- > 0) {

		if (InitDB(sErrorMessage) == FALSE) {
			Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
			continue;
		}

		try {
			m_pDB->ExecuteSQL(sSQL);
			bSuccess = TRUE;
			break;
		}
		catch (CDBException* e) {
			sErrorMessage.Format("Insert failed - %s", e->m_strError);
			LogprintfDB("Error try %d : %s   (%s)", nRetries,e->m_strError, sSQL);
			try {
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
		}
	}
	return bSuccess;

	return TRUE;
}

BOOL CDatabaseManager::InsertFlashJob(CString sJob, CString &sErrorMessage)
{
	CUtils util;
	CString sSQL;
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;

	//	sJob.Replace("_","!");

	if (g_prefs.m_flashtilequeue == 2)
		sSQL.Format("INSERT FlashPreviewQueue2 (FileName,EventTime) VALUES ('%s',GETDATE())", sJob);
	else
		sSQL.Format("INSERT FlashPreviewQueue (FileName,EventTime) VALUES ('%s',GETDATE())", sJob);
	BOOL bSuccess = FALSE;
	int nRetries = g_prefs.m_nQueryRetries;

	//	if (g_prefs.m_logToFile > 2)
	//		util.LogprintfResample("INFO: %s", sSQL);
	while (!bSuccess && nRetries-- > 0) {

		if (InitDB(sErrorMessage) == FALSE) {
			Sleep(g_prefs.m_QueryBackoffTime); // Time between retries
			continue;
		}

		try {
			m_pDB->ExecuteSQL(sSQL);
			bSuccess = TRUE;
			break;
		}
		catch (CDBException* e) {
			sErrorMessage.Format("Insert failed - %s", e->m_strError);
			LogprintfDB("Error : %s   (%s)", e->m_strError, sSQL);
			try {
				m_DBopen = FALSE;
				m_pDB->Close();
			}
			catch (CDBException* e) {
				// So what..! Go on!;
			}
			Sleep(g_prefs.m_QueryBackoffTime);
		}
	}
	return bSuccess;
}



int CDatabaseManager::FieldExists(CString sTableName, CString sFieldName , CString &sErrorMessage)
{
	CString sSQL, s;
	BOOL bFound = FALSE;
	sErrorMessage = _T("");

	if (InitDB(sErrorMessage) == FALSE)
		return -1;
	
	CRecordset Rs(m_pDB);
	sSQL.Format("{ CALL sp_columns ('%s') }", (LPCSTR)sTableName);

	try {		
		if(!Rs.Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", (LPCSTR)sSQL);
			return FALSE;
		}
		while (!Rs.IsEOF()) {
			
			Rs.GetFieldValue((short)3, s);

			if (s.CompareNoCase(sFieldName) == 0) {
				bFound = TRUE;
				break;
			}
			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch( CDBException* e ) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", (LPCSTR)e->m_strError);
		e->Delete();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch( CDBException* e ) {
			// So what..! Go on!;
		}
		return -1;
	}

	return bFound ? 1 : 0;
}

int CDatabaseManager::TableExists(CString sTableName, CString &sErrorMessage)
{
	CString sSQL, s;
	BOOL bFound = FALSE;
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;
	
	CRecordset Rs(m_pDB);
	sSQL.Format("{ CALL sp_tables ('%s') }", (LPCSTR)sTableName);

	try {		
		if(!Rs.Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", (LPCSTR)sSQL);
			return FALSE;
		}
		if (!Rs.IsEOF()) {
			
			bFound = TRUE;
		}
		Rs.Close();
	}
	catch( CDBException* e ) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", (LPCSTR)e->m_strError);
		e->Delete();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch( CDBException* e ) {
			// So what..! Go on!;
		}
		return -1;
	}

	return bFound ? 1 : 0;
}

int CDatabaseManager::StoredProcParameterExists(CString sSPname, CString sParameterName, CString &sErrorMessage)
{
	CString sSQL, s;
	BOOL bFound = FALSE;
	sErrorMessage = _T("");
	if (InitDB(sErrorMessage) == FALSE)
		return -1;
	
	CRecordset Rs(m_pDB);
	sSQL.Format("{ CALL sp_sproc_columns ('%s') }", (LPCSTR)sSPname);

	try {		
		if(!Rs.Open(CRecordset::snapshot, sSQL, CRecordset::readOnly)) {
			sErrorMessage.Format("Query failed - %s", (LPCSTR)sSQL);
			return FALSE;
		}
		while (!Rs.IsEOF()) {
			
			Rs.GetFieldValue((short)3, s);

			if (s.CompareNoCase(sParameterName) == 0) {
				bFound = TRUE;
				break;
			}
			Rs.MoveNext();
		}
		Rs.Close();
	}
	catch( CDBException* e ) {
		sErrorMessage.Format("ERROR (DATABASEMGR): Query failed - %s", (LPCSTR)e->m_strError);
		e->Delete();
		try {
			m_DBopen = FALSE;
			m_pDB->Close();
		}
		catch( CDBException* e ) {
			// So what..! Go on!;
		}
		return -1;
	}

	return bFound ? 1 : 0;
}

/////////////////////////////////////////////////
// Translate CTime to SQL Server DATETIME type
/////////////////////////////////////////////////

CString CDatabaseManager::TranslateDate(CTime tDate)
{
	CString dd,mm,yy,yysmall;

	dd.Format("%.2d",tDate.GetDay());
	mm.Format("%.2d",tDate.GetMonth());
	yy.Format("%.4d",tDate.GetYear());
	yysmall.Format("%.2d",tDate.GetYear());

	return mm + "/" + dd + "/" + yy;

}

CString CDatabaseManager::TranslateTime(CTime tDate)
{
	CString dd,mm,yy,yysmall, hour, min, sec;

	dd.Format("%.2d",tDate.GetDay());
	mm.Format("%.2d",tDate.GetMonth());
	yy.Format("%.4d",tDate.GetYear());
	yysmall.Format("%.2d",tDate.GetYear());

	hour.Format("%.2d",tDate.GetHour());
	min.Format("%.2d",tDate.GetMinute());
	sec.Format("%.2d",tDate.GetSecond());

	return mm + "/" + dd + "/" + yy + " " + hour + ":" + min + ":" + sec;
}

void CDatabaseManager::LogprintfDB(const TCHAR *msg, ...)
{
	TCHAR	szLogLine[16000], szFinalLine[16000];
	va_list	ap;
	DWORD	n, nBytesWritten;
	SYSTEMTIME	ltime;


	HANDLE hFile = ::CreateFile(g_prefs.m_logFilePath + _T("\\ProcessServiceDBerror.log"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	// Seek to end of file
	::SetFilePointer(hFile, 0, NULL, FILE_END);

	va_start(ap, msg);
	n = vsprintf(szLogLine, msg, ap);
	va_end(ap);
	szLogLine[n] = '\0';

	::GetLocalTime(&ltime);
	_stprintf(szFinalLine, "[%.2d-%.2d %.2d:%.2d:%.2d.%.3d] %s\r\n", (int)ltime.wDay, (int)ltime.wMonth, (int)ltime.wHour, (int)ltime.wMinute, (int)ltime.wSecond, (int)ltime.wMilliseconds, szLogLine);

	::WriteFile(hFile, szFinalLine, (DWORD)_tcsclen(szFinalLine), &nBytesWritten, NULL);

	::CloseHandle(hFile);

#ifdef _DEBUG
	TRACE(szFinalLine);
#endif
}

