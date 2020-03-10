#pragma once
#include "Stdafx.h"
#include "DatabaseManager.h"

class CPrefs
{
public:
	CPrefs(void);
	~CPrefs(void);

	void	LoadIniFile(CString sIniFile);
	void	LoadIniFileConvert(CString sIniFile);
	BOOL	ConnectToDB(BOOL bLoadDBNames, CString &sErrorMessage);

	BOOL LoadPreferencesFromRegistry();

	CString GetPublicationName(CDatabaseManager &DB, int nID);
	CString GetPublicationName(int nID);
	CString GetPublicationNameNoReload(int nID);
	PUBLICATIONSTRUCT *GetPublicationStruct(int nID);
	PUBLICATIONSTRUCT *GetPublicationStructNoDbLookup(int nID);
	PUBLICATIONSTRUCT *GetPublicationStruct(CDatabaseManager &DB, int nID);
	CString  GetExtendedAlias(int nPublicationID);
	void FlushPublicationName(CString sName);

	void FlushAlias(CString sType, CString sLongName);

	CString GetEditionName(int nID);
	CString GetEditionName(CDatabaseManager &DB, int nID);
	int		GetEditionID(CString s);
	int		GetEditionID(CDatabaseManager &DB, CString s);
	CString GetSectionName(int nID);
	CString GetSectionName(CDatabaseManager &DB, int nID);
	int		GetSectionID(CString s);
	int		GetSectionID(CDatabaseManager &DB, CString s);

	CString GetPublisherName(int nID);
	CString GetPublisherName(CDatabaseManager &DB, int nID);

	CString  LookupAbbreviationFromName(CString sType, CString sLongName);
	CString  LookupNameFromAbbreviation(CString sType, CString sShortName);
	CString		GetChannelName(int nID);
	CHANNELSTRUCT	*GetChannelStruct(int nID);
	CHANNELSTRUCT	*GetChannelStructNoDbLookup(int nID);
	CHANNELSTRUCT	*GetChannelStruct(CDatabaseManager &DB, int nID);
	
	PROOFPROCESSSTRUCT *GetProofStruct(int nID);

	int		GetPageFormatIDFromPublication(int nPublicationID);
	int		GetPageFormatIDFromPublication(CDatabaseManager &DB, int nPublicationID, BOOL bForceReload);

	CString FormCCFilesName(int nPDFtype, int nMasterCopySeparationSet, CString sFileName);
	CString FormCcdataFileNameLowres(CJobAttributes *pJob);
	CString FormCcdataFileNamePrint(CJobAttributes *pJob);
	CString FormCcdataFileNameHires(CJobAttributes *pJob);

	PDFPROCESSTYPE *GetPdfProcessType(int nID);

	PROOFPROCESSLIST m_ProofProcessList;


	CString m_callastempfolder;

	CString m_setupXMLpath;
	CString m_setupEnableXMLpath;

	BOOL m_FieldExists_Log_FileName;
	BOOL m_FieldExists_Log_MiscString;

	CString	m_apppath;
	CString m_title;
	CString m_codeword;
	CString m_codeword2;

	CString m_workfolder;
	CString m_workfolder2;
	BOOL m_bypasspingtest;
	BOOL m_bypassreconnect;
	int m_lockcheckmode;
	DWORD m_maxlogfilesize;


	BOOL		m_logtodatabase;
	CString		m_DBserver;
	CString		m_Database;
	CString		m_DBuser;
	CString		m_DBpassword;
	BOOL		m_IntegratedSecurity;
	int			m_databaselogintimeout;
	int			m_databasequerytimeout;
	int			m_nQueryRetries;
	int			m_QueryBackoffTime;


	BOOL		m_persistentconnection;
	int		m_logToFile;
	int		m_logToFile2;
	CString m_logFilePath;

	int			m_scripttimeout;
	BOOL		m_updatelog;
	int			m_maxfilesperscan;
	BOOL		m_setlocalfiletime;

	BOOL m_enableinstancecontrol;
	int	m_instancenumber;

	BOOL	m_emailonfoldererror;
	BOOL	m_emailonfileerror;
	CString m_emailsmtpserver;
	int		m_mailsmtpport;
	CString m_mailsmtpserverusername;
	CString m_mailsmtpserverpassword;
	BOOL	m_mailusessl;
	CString m_emailfrom;
	CString m_emailto;
	CString m_emailcc;
	CString m_emailsubject;
	BOOL	m_emailpreventflooding;
	int		m_emailpreventfloodingdelay;
	int    m_emailtimeout;

	BOOL m_bypassdiskfreeteste;
	BOOL m_bypasscallback;

	BOOL m_firecommandonfolderreconnect;
	CString m_sFolderReconnectCommand;

	CString m_lowresPDFPath;
	CString m_hiresPDFPath;		// == CCFiles ServerShare
	CString m_printPDFPath;

	CString m_previewPath;
	CString m_thumbnailPath;
	CString m_serverName;
	CString m_serverShare;
	CString m_readviewpreviewPath;
	CString m_configPath;
	CString m_errorPath;
	CString m_webPath;
	BOOL   m_copyToWeb;
	int		m_mainserverusecussrentuser;
	CString m_mainserverusername;
	CString m_mainserverpassword;

	BOOL m_usecurrentuserweb;
	CString m_webFTPuser;
	CString m_webFTPpassword;

	PUBLICATIONLIST		m_PublicationList;
	ITEMLIST			m_EditionList;
	ITEMLIST			m_SectionList;
	FILESERVERLIST		m_FileServerList;
	ALIASLIST			m_AliasList;
	STATUSLIST			m_StatusList;
	STATUSLIST			m_ExternalStatusList;

	CHANNELLIST			m_ChannelList;
//	CHANNELGROUPLIST	m_ChannelGroupList;
	ITEMLIST			m_PublisherList;

	PDFPROCESSTYPELIST	m_PDFProcessList;
	BOOL m_ccdatafilenamemode;

	CDatabaseManager m_DBmaint;
	int m_minMBfreeCCDATA;
	int m_prooflocksystem;
	CString m_alivelogFilePath;
	int m_sleeptimebewteenresampling;

	BOOL m_bSortOnCreateTime;

	int m_pdfproofmethod;
	int m_pdfprinttimeout;
	//int m_pdfprintresolutionfactor;

	BOOL m_webversionpreviews;
	BOOL m_webversionthumbnails;

	int m_flashtilequeue;
	int m_flashtilesize;

	CString m_ghostscriptpath;
	CString m_ghostscriptfontpath;
	CString m_ghostscriptcommandfile;
	BOOL m_usedummypreviewonpdffail;
	CString m_dummypreview;
	CString m_dummythumbnail;

	BOOL m_usefullbuffertiffshift;

	CString m_pdfripinputfolder;
	CString m_pdfripoutputfolder;


	CString m_pdfripinputfolder2;
	CString m_pdfripoutputfolder2;

	CString m_convertprofilelowres;
	CString m_convertprofileprint;

	CString m_callascommandfilepath;
	CString m_callaskeyCode;
	CString m_callaskeyCode2;
	int m_processtimeout;


	CString m_callasProfileLowres;
	CString m_callasProfileCMYK;

	int m_externalProcessStableTime;

	BOOL m_trimpdf;
	BOOL m_extraConvertTread;

	BOOL m_generatestatusmessage;
	BOOL m_messageonsuccess;
	BOOL m_messageonerror;
	CString m_messageoutputfolder;
	CString m_messagetemplatesuccess;
	CString m_messagetemplateerror;

};

