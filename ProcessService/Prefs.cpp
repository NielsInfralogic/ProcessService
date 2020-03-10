#include "stdafx.h"
#include "Defs.h"
#include "Prefs.h"
#include "Utils.h"
#include "Markup.h"
#include "ProcessService.h"
#include "Registry.h"

extern BOOL		g_BreakSignal;
extern CUtils g_util;
extern BOOL g_connected;

CPrefs::CPrefs(void)
{
	m_codeword = _T("");
	m_codeword2 = _T("");
	m_title = _T("");

	m_enableinstancecontrol = FALSE;
	m_sleeptimebewteenresampling = 2;
	m_instancenumber = 0;

	m_workfolder = _T("c:\\temp");
	m_workfolder2 = _T("c:\\temp2");

	m_bypasspingtest = TRUE;
	m_bypassreconnect = FALSE;
	m_lockcheckmode = LOCKCHECKMODE_READWRITE;


	m_logtodatabase = FALSE;
	m_DBserver = _T("");
	m_Database = _T("ControlCenterPDF");
	m_DBuser = _T("sa");
	m_DBpassword = _T("infra");

	m_logToFile = FALSE;
	m_logToFile2 = FALSE;

	m_scripttimeout = 60;

	m_prooflocksystem = TRUE;

	m_bSortOnCreateTime = FALSE;

	m_usecurrentuserweb = TRUE;
	m_webFTPuser = _T("");
	m_webFTPpassword = _T("");

	m_pdfproofmethod = PDFPROOFMETHOD_GHOSTSCRIPT;

	m_usefullbuffertiffshift = TRUE;

	m_emailtimeout = 60;

	m_externalProcessStableTime = 1;

	m_generatestatusmessage = TRUE;
	m_messageonsuccess = FALSE;
	m_messageonerror = TRUE;
	m_messageoutputfolder = _T("");
	m_messagetemplateerror = _T("");
	m_messagetemplatesuccess = _T("");


}

CPrefs::~CPrefs(void)
{
}

void CPrefs::LoadIniFileConvert(CString sIniFile)
{
	TCHAR Tmp[MAX_PATH];
	TCHAR Tmp2[MAX_PATH];
	
	::GetPrivateProfileString("System", "keyCode", "2BQFJLCV2UZKD85V25XP96ZR", Tmp, 255, sIniFile);
	m_callaskeyCode = Tmp;

	::GetPrivateProfileString("System", "keyCode2", "4ZDYE8PCVNC4RJEC4P28VJ28", Tmp, 255, sIniFile);
	m_callaskeyCode2 = Tmp;

	//::GetPrivateProfileString("System", "callasCommand", "4ZDYE8PCVNC4RJEC4P28VJ28", Tmp, 255, sIniFile);
	m_callascommandfilepath = m_apppath + _T("\\PDFEngine\\pdfToolbox.exe");


	::GetPrivateProfileString(_T("Setup"), _T("CallasTempFolder"), _T(""), Tmp, _MAX_PATH, sIniFile);
	m_callastempfolder = Tmp;
	

	::GetPrivateProfileString(_T("Setup"), _T("CommandFileTimeout"), _T("1200"), Tmp, _MAX_PATH, sIniFile);
	m_processtimeout = _tstoi(Tmp);

	::GetPrivateProfileString(_T("Setup"), _T("ProfileToLowRes"), _T("Online publishing.kfpx"), Tmp, _MAX_PATH, sIniFile);
	m_callasProfileLowres = Tmp;
	if (g_util.FileExist(m_callasProfileLowres) == FALSE)
		m_callasProfileLowres.Format("%s\\%s", m_apppath,Tmp);

	::GetPrivateProfileString(_T("Setup"), _T("ProfileToCMYK"), _T("Convert to CMYK only (ISOnewspaper 26 (IFRA)).kfpx"), Tmp, _MAX_PATH, sIniFile);
	m_callasProfileCMYK = Tmp;
	if (g_util.FileExist(m_callasProfileCMYK) == FALSE)
		m_callasProfileCMYK.Format("%s\\%s", m_apppath, Tmp);

	::GetPrivateProfileString(_T("Setup"), _T("TrimPDF"), _T("1"), Tmp, _MAX_PATH, sIniFile);
	m_trimpdf = _tstoi(Tmp);
	
	::GetPrivateProfileString(_T("Setup"), _T("ExtraConvertThread"), _T("1"), Tmp, _MAX_PATH, sIniFile);
	m_extraConvertTread = _tstoi(Tmp);


	::GetPrivateProfileString("Message", "GenerateMessage", "0", Tmp, 255, sIniFile);
	m_generatestatusmessage = atoi(Tmp);

	::GetPrivateProfileString("Message", "MessageOnSuccess", "0", Tmp, 255, sIniFile);
	m_messageonsuccess = atoi(Tmp);

	::GetPrivateProfileString("Message", "MessageOnError", "1", Tmp, 255, sIniFile);
	m_messageonerror = atoi(Tmp);

	::GetPrivateProfileString("Message", "OutputFolder", "c:\\temp", Tmp, 255, sIniFile);
	m_messageoutputfolder = Tmp;

	sprintf(Tmp2, "%s\\messagetemplates\\Success.jfm", (LPCSTR)m_apppath);
	::GetPrivateProfileString("Message", "TemplateOnSuccess", Tmp2, Tmp, 255, sIniFile);
	m_messagetemplatesuccess = Tmp;

	sprintf(Tmp2, "%s\\messagetemplates\\Error.jfm", (LPCSTR)m_apppath);
	::GetPrivateProfileString("Message", "TemplateOnError", Tmp2, Tmp, 255, sIniFile);
	m_messagetemplateerror = Tmp;

}


void CPrefs::LoadIniFile(CString sIniFile)
{
	TCHAR Tmp[MAX_PATH];
//	TCHAR Tmp2[MAX_PATH];

	::GetPrivateProfileString("System", "CcdataFilenameMode", "0", Tmp, 255, sIniFile);
	m_ccdatafilenamemode = _tstoi(Tmp);

	::GetPrivateProfileString("System", "ScriptTimeOut", "60", Tmp, _MAX_PATH, sIniFile);
	m_scripttimeout = _tstoi(Tmp);

	GetPrivateProfileString("Setup", "LockCheckMode", "2", Tmp, 255, sIniFile);
	m_lockcheckmode = _tstoi(Tmp);

	GetPrivateProfileString("Setup", "IgnoreLockCheck", "0", Tmp, 255, sIniFile);
	if (_tstoi(Tmp) > 0)
		m_lockcheckmode = 0;

	::GetPrivateProfileString("System", "MinMBfreeCCDATA", "0", Tmp, _MAX_PATH, sIniFile);
	m_minMBfreeCCDATA = _tstoi(Tmp);


	::GetPrivateProfileString("Setup", "SetLocalFileTime", "0", Tmp, 255, sIniFile);
	m_setlocalfiletime = _tstoi(Tmp);


	GetPrivateProfileString("Setup", "BypassPingTest", "1", Tmp, 255, sIniFile);
	m_bypasspingtest = _tstoi(Tmp);

	GetPrivateProfileString("Setup", "BypassReconnect", "0", Tmp, 255, sIniFile);
	m_bypassreconnect = _tstoi(Tmp);

	GetPrivateProfileString("Setup", "BypassDiskFreeTest", "0", Tmp, 255, sIniFile);
	m_bypassdiskfreeteste = _tstoi(Tmp);


	::GetPrivateProfileString("Setup", "WorkFolder", "c:\\temp", Tmp, _MAX_PATH, sIniFile);
	m_workfolder = Tmp;

	::GetPrivateProfileString("Setup", "WorkFolder2", "c:\\temp2", Tmp, _MAX_PATH, sIniFile);
	m_workfolder2 = Tmp;

	::GetPrivateProfileString("Setup", "MailOnFileError", "0", Tmp, _MAX_PATH, sIniFile);
	m_emailonfileerror = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MailOnFolderError", "0", Tmp, _MAX_PATH, sIniFile);
	m_emailonfoldererror = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MailSmtpServer", "", Tmp, _MAX_PATH, sIniFile);
	m_emailsmtpserver = Tmp;
	::GetPrivateProfileString("Setup", "MailSmtpPort", "25", Tmp, _MAX_PATH, sIniFile);
	m_mailsmtpport = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MailSmtpUsername", "", Tmp, _MAX_PATH, sIniFile);
	m_mailsmtpserverusername = Tmp;

	::GetPrivateProfileString("Setup", "MailSmtpPassword", "", Tmp, _MAX_PATH, sIniFile);
	m_mailsmtpserverpassword = Tmp;

	::GetPrivateProfileString("Setup", "MailUseSSL", "0", Tmp, _MAX_PATH, sIniFile);
	m_mailusessl = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MailFrom", "", Tmp, _MAX_PATH, sIniFile);
	m_emailfrom = Tmp;
	::GetPrivateProfileString("Setup", "MailTo", "", Tmp, _MAX_PATH, sIniFile);
	m_emailto = Tmp;
	::GetPrivateProfileString("Setup", "MailCc", "", Tmp, _MAX_PATH, sIniFile);
	m_emailcc = Tmp;
	::GetPrivateProfileString("Setup", "MailSubject", "", Tmp, _MAX_PATH, sIniFile);
	m_emailsubject = Tmp;

	::GetPrivateProfileString("Setup", "MailPreventFlooding", "0", Tmp, _MAX_PATH, sIniFile);
	m_emailpreventflooding = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MainPreventFloodingDelay", "10", Tmp, _MAX_PATH, sIniFile);
	m_emailpreventfloodingdelay = _tstoi(Tmp);

	::GetPrivateProfileString("Setup", "MaxLogFileSize", "10485760", Tmp, _MAX_PATH, sIniFile);
	m_maxlogfilesize = _tstoi(Tmp);

	::GetPrivateProfileString("System", "ProofLockSystem", "1", Tmp, 255, sIniFile);
	m_prooflocksystem = atoi(Tmp);


	sprintf(Tmp, "%s\\alivelogs", (LPCSTR)g_util.GetFilePath(m_logFilePath));
	::GetPrivateProfileString("System", "AliveLogFolder", Tmp, m_alivelogFilePath.GetBuffer(_MAX_PATH), _MAX_PATH, sIniFile);
	m_alivelogFilePath.ReleaseBuffer();
	::CreateDirectory(m_alivelogFilePath, NULL);

	GetPrivateProfileString("System", "SleeptimeBetweenResampling", "2", Tmp, _MAX_PATH, sIniFile);
	m_sleeptimebewteenresampling = atoi(Tmp);

	::GetPrivateProfileString("System", "PdfProofMethod", "0", Tmp, _MAX_PATH, sIniFile);
	m_pdfproofmethod = atoi(Tmp);

	::GetPrivateProfileString("System", "PdfPrintTimeout", "300", Tmp, _MAX_PATH, sIniFile);
	m_pdfprinttimeout = atoi(Tmp);


	GetPrivateProfileString("System", "WebVersionPreviews", "1", Tmp, 255, sIniFile);
	m_webversionpreviews = atoi(Tmp);
	GetPrivateProfileString("System", "WebVersionThumbnails", "1", Tmp, 255, sIniFile);
	m_webversionthumbnails = atoi(Tmp);

	::GetPrivateProfileString("System", "FlashTileSize", "512", Tmp, 255, sIniFile);
	m_flashtilesize = atoi(Tmp);

	::GetPrivateProfileString("System", "FlashTileQueueNumber", "1", Tmp, 255, sIniFile);
	m_flashtilequeue = atoi(Tmp);
	if (m_flashtilequeue < 1)
		m_flashtilequeue = 1;


	::GetPrivateProfileString("System", "GhostscriptPath", "C:\\Program Files\\gs\\gs9.04", Tmp, 255, sIniFile);
	m_ghostscriptpath = Tmp;


	::GetPrivateProfileString("System", "GhostscriptFontPath", "C:\\Program Files\\gs\\fonts", Tmp, 255, sIniFile);
	m_ghostscriptfontpath = Tmp;

	::GetPrivateProfileString("System", "GhostscriptCommandFile", "gswin64c.exe", Tmp, 255, sIniFile);
	m_ghostscriptcommandfile = Tmp;

	GetPrivateProfileString("System", "UseDummyPreviewOnPDFfail", "0", Tmp, 255, sIniFile);
	m_usedummypreviewonpdffail = atoi(Tmp);

	GetPrivateProfileString("System", "DummyPreview", "", Tmp, 255, sIniFile);
	m_dummypreview = Tmp;

	GetPrivateProfileString("System", "DummyThumbnail", "", Tmp, 255, sIniFile);
	m_dummythumbnail = Tmp;

	::GetPrivateProfileString("System", "PdfRipInputFolder", "", Tmp, 255, sIniFile);
	m_pdfripinputfolder = Tmp;

	::GetPrivateProfileString("System", "PdfRipOutputFolder", "", Tmp, 255, sIniFile);
	m_pdfripoutputfolder = Tmp;


	::GetPrivateProfileString("System", "PdfRipInputFolder2", "", Tmp, 255, sIniFile);
	m_pdfripinputfolder2 = Tmp;

	::GetPrivateProfileString("System", "PdfRipOutputFolder2", "", Tmp, 255, sIniFile);
	m_pdfripoutputfolder2 = Tmp;

	::GetPrivateProfileString("System", "ExternalProcessStableTime", "1", Tmp, 255, sIniFile);
	m_externalProcessStableTime = atoi(Tmp);

	


}

BOOL CPrefs::ConnectToDB(BOOL bLoadDBNames, CString &sErrorMessage)
{

	g_connected = m_DBmaint.InitDB(m_DBserver, m_Database, m_DBuser, m_DBpassword, m_IntegratedSecurity, sErrorMessage);

	if (g_connected) {
		if (m_DBmaint.RegisterService(sErrorMessage) == FALSE) {
			g_util.LogprintfConvert("ERROR: RegisterService() returned - %s", sErrorMessage);
		}
	}
	else
		g_util.LogprintfConvert("ERROR: InitDB() returned - %s", sErrorMessage);

	if (bLoadDBNames) {
		if (m_DBmaint.LoadAllPrefs(sErrorMessage) == FALSE)
			g_util.LogprintfConvert("ERROR: LoadAllPrefs() returned - %s", sErrorMessage);

	}

	return g_connected;
}


CString CPrefs::GetPublicationName(int nID)
{
	return GetPublicationName(m_DBmaint, nID);
}

CString CPrefs::GetPublicationName(CDatabaseManager &DB, int nID)
{
	CString sErrorMessage;

	if (nID == 0)
		return _T("");

	for (int i = 0; i < m_PublicationList.GetCount(); i++) {
		if (m_PublicationList[i].m_ID == nID)
			return m_PublicationList[i].m_name;
	}

	CString sPubName = DB.LoadNewPublicationName(nID, sErrorMessage);

	if (sPubName != "")
		DB.LoadSpecificAlias("Publication", sPubName, sErrorMessage);

	return sPubName;
}

CString CPrefs::GetPublicationNameNoReload(int nID)
{
	CString sErrorMessage;

	if (nID == 0)
		return _T("");

	for (int i = 0; i < m_PublicationList.GetCount(); i++) {
		if (m_PublicationList[i].m_ID == nID)
			return m_PublicationList[i].m_name;
	}


	return _T("");
}


void CPrefs::FlushPublicationName(CString sName)
{
	for (int i = 0; i < m_PublicationList.GetCount(); i++) {
		if (sName.CompareNoCase(m_PublicationList[i].m_name) == 0) {
			m_PublicationList.RemoveAt(i);
			return;
		}
	}
}

PUBLICATIONSTRUCT *CPrefs::GetPublicationStruct(int nID)
{
	return GetPublicationStruct(m_DBmaint, nID);
}

PUBLICATIONSTRUCT *CPrefs::GetPublicationStructNoDbLookup(int nID)
{
	if (nID == 0)
		return NULL;
	for (int i = 0; i < m_PublicationList.GetCount(); i++) {
		if (m_PublicationList[i].m_ID == nID)
			return &m_PublicationList[i];
	}

	return NULL;
}


PUBLICATIONSTRUCT *CPrefs::GetPublicationStruct(CDatabaseManager &DB, int nID)
{
	if (nID == 0)
		return NULL;
	for (int i = 0; i < m_PublicationList.GetCount(); i++) {
		if (m_PublicationList[i].m_ID == nID)
			return &m_PublicationList[i];
	}

	CString sErrorMessage;

	CString sPubName = DB.LoadNewPublicationName(nID, sErrorMessage);

	if (sPubName != "")
		DB.LoadSpecificAlias("Publication", sPubName, sErrorMessage);

	for (int i = 0; i < m_PublicationList.GetCount(); i++) {
		if (m_PublicationList[i].m_ID == nID)
			return &m_PublicationList[i];
	}

	return NULL;
}

CString CPrefs::GetExtendedAlias(int nPublicationID)
{
	for (int i = 0; i < m_PublicationList.GetCount(); i++) {
		if (m_PublicationList[i].m_ID == nPublicationID)
			return m_PublicationList[i].m_extendedalias;
	}

	return GetPublicationName(nPublicationID);
}

void CPrefs::FlushAlias(CString sType, CString sLongName)
{
	for (int i = 0; i < m_AliasList.GetCount(); i++) {
		if (m_AliasList[i].sType == sType && m_AliasList[i].sLongName.CompareNoCase(sLongName) == 0) {
			m_AliasList.RemoveAt(i);
			return;
		}
	}
}

CString CPrefs::GetEditionName(int nID)
{
	return GetEditionName(m_DBmaint, nID);
}

CString CPrefs::GetEditionName(CDatabaseManager &DB, int nID)
{
	CString sErrorMessage;

	if (nID == 0)
		return _T("");

	for (int i = 0; i < m_EditionList.GetCount(); i++) {
		if (m_EditionList[i].m_ID == nID)
			return m_EditionList[i].m_name;
	}

	CString sEdName = DB.LoadNewEditionName(nID, sErrorMessage);

	if (sEdName != "")
		DB.LoadSpecificAlias("Edition", sEdName, sErrorMessage);

	return sEdName;
}

int CPrefs::GetEditionID(CString s)
{
	return GetEditionID(m_DBmaint, s);
}

int CPrefs::GetEditionID(CDatabaseManager &DB, CString s)
{
	CString sErrorMessage;

	if (s.Trim() == "")
		return 0;

	for (int i = 0; i < m_EditionList.GetCount(); i++) {
		if (s.CompareNoCase(m_EditionList[i].m_name) == 0)
			return m_EditionList[i].m_ID;
	}
	int nID = DB.LoadNewEditionID(s, sErrorMessage);

	if (nID > 0)
		DB.LoadSpecificAlias("Edition", s, sErrorMessage);

	return nID;
}

CString CPrefs::GetSectionName(int nID)
{
	return GetSectionName(m_DBmaint, nID);
}

CString CPrefs::GetSectionName(CDatabaseManager &DB, int nID)
{
	CString sErrorMessage;

	if (nID == 0)
		return _T("");

	for (int i = 0; i < m_SectionList.GetCount(); i++) {
		if (m_SectionList[i].m_ID == nID)
			return m_SectionList[i].m_name;
	}

	CString sSecName = DB.LoadNewSectionName(nID, sErrorMessage);

	if (sSecName != "")
		DB.LoadSpecificAlias("Section", sSecName, sErrorMessage);

	return sSecName;
}

int CPrefs::GetSectionID(CString sName)
{
	return GetSectionID(m_DBmaint, sName);
}

int CPrefs::GetSectionID(CDatabaseManager &DB, CString s)
{
	CString sErrorMessage;

	if (s.Trim() == "")
		return 0;

	for (int i = 0; i < m_SectionList.GetCount(); i++) {
		if (s.CompareNoCase(m_SectionList[i].m_name) == 0)
			return m_SectionList[i].m_ID;
	}

	int nID = DB.LoadNewSectionID(s, sErrorMessage);

	if (nID > 0)
		DB.LoadSpecificAlias("Section", s, sErrorMessage);

	return nID;
}

CString CPrefs::GetPublisherName(int nID)
{
	return GetPublisherName(m_DBmaint, nID);
}

CString CPrefs::GetPublisherName(CDatabaseManager &DB, int nID)
{
	CString sErrorMessage;

	if (nID == 0)
		return _T("");

	for (int i = 0; i < m_PublisherList.GetCount(); i++) {
		if (m_PublisherList[i].m_ID == nID)
			return m_PublisherList[i].m_name;
	}

	CString sPublisherName = DB.LoadNewPublisherName(nID, sErrorMessage);

	return sPublisherName;
}



CString CPrefs::GetChannelName(int nID)
{
	for (int i = 0; i < m_ChannelList.GetCount(); i++) {
		if (m_ChannelList[i].m_channelID == nID) {
			return m_ChannelList[i].m_channelname;
		}
	}

	CHANNELSTRUCT *p = GetChannelStruct(nID);
	if (p != NULL)
		return p->m_channelname;

	return _T("");
}

PDFPROCESSTYPE *CPrefs::GetPdfProcessType(int nID)
{
	for (int i = 0; i < m_PDFProcessList.GetCount(); i++) {
		if (m_PDFProcessList[i].m_ProcessID == nID)
			return &m_PDFProcessList[i];
	}

	return NULL;
}

CHANNELSTRUCT	*CPrefs::GetChannelStruct(int nID)
{
	return GetChannelStruct(m_DBmaint, nID);
}

CHANNELSTRUCT	*CPrefs::GetChannelStructNoDbLookup(int nID)
{
	if (nID == 0)
		return NULL;
	for (int i = 0; i < m_ChannelList.GetCount(); i++) {
		if (m_ChannelList[i].m_channelID == nID) {
			return &m_ChannelList[i];
		}
	}

	return NULL;
}


CHANNELSTRUCT	*CPrefs::GetChannelStruct(CDatabaseManager &DB, int nID)
{
	if (nID == 0)
		return NULL;
	CString sErrorMessage = _T("");
	int nIndex = 0;
	for (int i = 0; i < m_ChannelList.GetCount(); i++) {
		if (m_ChannelList[i].m_channelID == nID) {
			return &m_ChannelList[i];
		}
	}


	DB.LoadNewChannel(nID, sErrorMessage);

	for (int i = 0; i < m_ChannelList.GetCount(); i++) {
		if (m_ChannelList[i].m_channelID == nID)
			return &m_ChannelList[i];
	}

	return NULL;
}

PROOFPROCESSSTRUCT *CPrefs::GetProofStruct(int nID)
{
	for (int i = 0; i < m_ProofProcessList.GetCount(); i++) {
		if (m_ProofProcessList[i].m_ID == nID)
			return &m_ProofProcessList[i];
	}
	return NULL;
}

CString CPrefs::LookupAbbreviationFromName(CString sType, CString sLongName)
{


	for (int i = 0; i < m_AliasList.GetCount(); i++) {
		ALIASSTRUCT a = m_AliasList[i];
		if (a.sType.CompareNoCase(sType) == 0 && a.sLongName.CompareNoCase(sLongName) == 0)
			return a.sShortName;
	}

	return sLongName;
}

CString CPrefs::LookupNameFromAbbreviation(CString sType, CString sShortName)
{
	for (int i = 0; i < m_AliasList.GetCount(); i++) {
		ALIASSTRUCT a = m_AliasList[i];
		if (a.sType.CompareNoCase(sType) == 0 && a.sShortName.CompareNoCase(sShortName) == 0)
			return a.sLongName;
	}

	return sShortName;
}

int CPrefs::GetPageFormatIDFromPublication(int nPublicationID)
{
	return GetPageFormatIDFromPublication(m_DBmaint, nPublicationID, false);
}

int CPrefs::GetPageFormatIDFromPublication(CDatabaseManager &DB, int nPublicationID, BOOL bForceReload)
{
	CString sErrorMessage;

	if (nPublicationID == 0)
		return 0;

	if (bForceReload == FALSE) {

		for (int i = 0; i < m_PublicationList.GetCount(); i++) {
			if (nPublicationID == m_PublicationList[i].m_ID) {
				return m_PublicationList[i].m_PageFormatID;
			}
		}
	}

	

	return 0;
}





BOOL CPrefs::LoadPreferencesFromRegistry()
{
	CRegistry pReg;

	// Set defaults
	m_logFilePath = _T("c:\\Temp");
	m_DBserver = _T(".");
	m_Database = _T("PDFHUB");
	m_DBuser = _T("sa");
	m_DBpassword = _T("Infra2Logic");
	m_IntegratedSecurity = FALSE;
	m_logToFile = 1;
	m_logToFile2 = 1;
	m_databaselogintimeout = 20;
	m_databasequerytimeout = 10;
	m_nQueryRetries = 20;
	m_QueryBackoffTime = 500;
	m_instancenumber = 1;

	if (pReg.OpenKey(CRegistry::localMachine, "Software\\InfraLogic\\ProcessService\\Parameters")) {
		CString sVal = _T("");
		DWORD nValue;

		if (pReg.GetValue("InstanceNumber", nValue))
			m_instancenumber = nValue;

		if (pReg.GetValue("IntegratedSecurity", nValue))
			m_IntegratedSecurity = nValue;

		if (pReg.GetValue("DBServer", sVal))
			m_DBserver = sVal;

		if (pReg.GetValue("Database", sVal))
			m_Database = sVal;

		if (pReg.GetValue("DBUser", sVal))
			m_DBuser = sVal;

		if (pReg.GetValue("DBpassword", sVal))
			m_DBpassword = sVal;

		if (pReg.GetValue("DBLoginTimeout", nValue))
			m_databaselogintimeout = nValue;

		if (pReg.GetValue("DBQueryTimeout", nValue))
			m_databasequerytimeout = nValue;

		if (pReg.GetValue("DBQueryRetries", nValue))
			m_nQueryRetries = nValue > 0 ? nValue : 20;

		if (pReg.GetValue("DBQueryBackoffTime", nValue))
			m_QueryBackoffTime = nValue >= 500 ? nValue : 500;

		if (pReg.GetValue("Logging", nValue))
			m_logToFile = nValue;

		if (pReg.GetValue("Logging2", nValue))
			m_logToFile2 = nValue;

		if (pReg.GetValue("LogFileFolder", sVal))
			m_logFilePath = sVal;

		pReg.CloseKey();

		return TRUE;
	}

	return FALSE;
}

CString CPrefs::FormCCFilesName(int nPDFtype, int nMasterCopySeparationSet, CString sFileName)
{
	sFileName = g_util.GetFileName(sFileName, TRUE); // Shave off extension

	CString sPath = _T("");
	if (nPDFtype == POLLINPUTTYPE_LOWRESPDF) {
		if (m_ccdatafilenamemode && sFileName != _T(""))
			sPath.Format("%s\\%s#%d.PDF", m_lowresPDFPath, sFileName, nMasterCopySeparationSet);
		else
			sPath.Format("%s%d.PDF", m_lowresPDFPath, nMasterCopySeparationSet);
	}
	else if (nPDFtype == POLLINPUTTYPE_HIRESPDF)
	{
		if (m_ccdatafilenamemode && sFileName != _T(""))
			sPath.Format("%s\\%s#%d.PDF", m_hiresPDFPath, sFileName, nMasterCopySeparationSet);
		else
			sPath.Format("%s%d.PDF", m_hiresPDFPath, nMasterCopySeparationSet);
	}
	else
	{
		if (m_ccdatafilenamemode && sFileName != _T(""))
			sPath.Format("%s\\%s#%d.PDF", m_printPDFPath, sFileName, nMasterCopySeparationSet);
		else
			sPath.Format("%s%d.PDF", m_printPDFPath, nMasterCopySeparationSet);
	}

	return sPath;
}


CString CPrefs::FormCcdataFileNameLowres(CJobAttributes *pJob)
{
	CString sDestPath;
	if (m_ccdatafilenamemode && pJob->m_ccdatafilename != _T(""))
		sDestPath.Format("%s\\%s_%d.PDF", m_lowresPDFPath, pJob->m_ccdatafilename, pJob->m_mastercopyseparationset);
	else
		sDestPath.Format("%s\\%d.PDF", m_lowresPDFPath, pJob->m_mastercopyseparationset);

	return sDestPath;
}

CString CPrefs::FormCcdataFileNameHires(CJobAttributes *pJob)
{
	CString sDestPath;
	if (m_ccdatafilenamemode && pJob->m_ccdatafilename != _T(""))
		sDestPath.Format("%s\\%s_%d.PDF", m_hiresPDFPath, pJob->m_ccdatafilename, pJob->m_mastercopyseparationset);
	else
		sDestPath.Format("%s\\%d.PDF", m_hiresPDFPath, pJob->m_mastercopyseparationset);

	return sDestPath;
}

CString CPrefs::FormCcdataFileNamePrint(CJobAttributes *pJob)
{
	CString sDestPath;
	if (m_ccdatafilenamemode && pJob->m_ccdatafilename != _T(""))
		sDestPath.Format("%s\\%s_%d.PDF", m_printPDFPath, pJob->m_ccdatafilename, pJob->m_mastercopyseparationset);
	else
		sDestPath.Format("%s\\%d.PDF", m_printPDFPath, pJob->m_mastercopyseparationset);

	return sDestPath;
}
