#pragma once
#include <afxdb.h>
#include "Defs.h"

class CDatabaseManager
{
public:
	CDatabaseManager(void);
	virtual ~CDatabaseManager(void);

	BOOL	InitDB(CString sDBserver, CString sDatabase, CString sDBuser, CString sDBpassword, BOOL bIntegratedSecurity, CString &sErrorMessage);
	BOOL	InitDB(CString &sErrorMessage);
	void	ExitDB();

	BOOL	IsOpen();
	BOOL	LoadConfigIniFile(int nInstanceNumber, CString &sFileName, CString &sFileName2, CString &sFileName3, CString &sErrorMessage);
	BOOL	RegisterService(CString &sErrorMessage);
	BOOL	UpdateService(int nCurrentState, CString sCurrentJob, CString sLastError, CString &sErrorMessage);
	BOOL	UpdateService(int nCurrentState, CString sCurrentJob, CString sLastError, CString sAddedLogData, CString &sErrorMessage);
	BOOL	InsertLogEntry(int nEvent, CString sSource, CString sFileName, CString sMessage, int nMasterCopySeparationSet, int nVersion, int nMiscInt, CString sMiscString, CString &sErrorMessage);
	BOOL	LoadSTMPSetup(CString &sErrorMessage);

	//BOOL	UpdatePrepollStatus(int nMasterCopySeparationSet, int serviceType, int nEventCode, CString sMessage, CString &sErrorMessage);
	BOOL	UpdateConvertStatus(int nStatus, int nMasterCopySeparationSet, int nPDFmode, CString sMessage, CString sFileName, int nInstanceNumber, CString &sErrorMessage);
	int		ReleaseProofLock(CString &sErrorMessage);
	int		AcuireProofLock(CString &sErrorMessage);
	int		SetProofLock(int nLockType, CString &sErrorMessage);

	int		ReleaseConvertLock(int nConvertType, int nInstanceNumber, CString &sErrorMessage);
	int		AcuireConvertLock(int nConvertType, int nInstanceNumber, CString &sErrorMessage);
	int		SetConvertLock(int nConvertType, int nLockType, int nInstanceNumber, CString &sErrorMessage);

	BOOL    LoadPublicationList(CString &sErrorMessage);
	BOOL	LoadEditionNameList(CString &sErrorMessage);
	BOOL	LoadSectionNameList(CString &sErrorMessage);
	BOOL	LoadAliasList(CString &sErrorMessage);
	BOOL    LoadStatusList(CString sIDtable, STATUSLIST &v, CString &sErrorMessage);
	BOOL	LoadPublicationChannels(int nID, CString &sErrorMessage);
	BOOL	LoadFileServerList(CString &sErrorMessage);
	int     LoadPublisherList(CString &sErrorMessage);
	int		LoadChannelList(CString &sErrorMessage);
	BOOL	LoadPDFProcessList(CString &sErrorMessage);
	BOOL	LoadAllPrefs(CString &sErrorMessage);
	int		LoadGeneralPreferences(CString &sErrorMessage);

	CString LoadNewPublicationName(int nID, CString &sErrorMessage);
	CString LoadNewEditionName(int nID, CString &sErrorMessage);
	int		LoadNewEditionID(CString sName, CString &sErrorMessage);
	CString LoadNewSectionName(int nID, CString &sErrorMessage);
	int		LoadNewSectionID(CString sName, CString &sErrorMessage);
	BOOL    LoadNewChannel(int nChannelID, CString &sErrorMessage);
	CString LoadNewPublisherName(int nID, CString &sErrorMessage);



	BOOL	LoadProofProcessList(CString &sErrorMessage);
	BOOL	UpdateCRC(int nMasterCopySeparationSet, int nPDFtype, int nCRC, CString &sErrorMessage);

	CTime	GetDatabaseTime(CString &sErrorMessage);

	CString  LoadSpecificAlias(CString sType, CString sLongName, CString &sErrorMessage);

	BOOL	ResetProofStatus(CString &sErrorMessage);
	BOOL	ResetConvertStatus(int nPDFmode, int nInstanceNumber, CString &sErrorMessage);

	BOOL	LookupProofJob(CJobAttributes &Job, BOOL &bUnplannedJob, CString &sErrorMessage);
	BOOL	UpdateStatusProof(int nMasterCopySeparationSet, int nProofStatus, CString sErr, BOOL &bUpdateOK, CString &sErrorMessage);
	BOOL	UpdateStatusProofReadView(int nMasterCopySeparationSetLeft, int nMasterCopySeparationSetRight, int nProofStatus, CString sErr, BOOL &bUpdateOK, CString &sErrorMessage);

	BOOL	LookupConvertJob(CJobAttributes &Job, int nToPDFType, CString &sErrorMessage);


	BOOL	GetJobDetails(CJobAttributes *pJob, int nCopySeparationSet, CString &sErrorMessage);
	BOOL	GetPolledStatus(int nMasterCopySeparationSet, BOOL &bIsPolled, CString &sErrorMessage);
	BOOL	InsertFlashJob(CString sJob, CString &sErrorMessage);

	int     GetChannelCRC(int nMasterCopySeparationSet, int nInputMode, CString &sErrorMessage);
	int		GetChannelStatus(int nMasterCopySeparationSet, int nChannelID, CString &sErrorMessage);
	BOOL	UpdateChannelStatus(int nStatusType, int nMasterCopySeparationSet, int nProductionID, int nChannelID, int nStatus, CString sMessage, CString sFileName, CString &sErrorMessage);
	BOOL	UpdateConvertCRC(int nCRC, int nMasterCopySeparationSet, int nPDFmode, CString &sErrorMessage);
	BOOL	GetChannelsForMasterSet(int nMasterCopySeparationSet, int nPDFType, CUIntArray &aChannelIDList, CString &sErrorMessage);


	int		StoredProcParameterExists(CString sSPname, CString sParameterName, CString &sErrorMessage);
	int		TableExists(CString sTableName, CString &sErrorMessage);
	int		FieldExists(CString sTableName, CString sFieldName , CString &sErrorMessage);


	CString TranslateDate(CTime tDate);
	CString TranslateTime(CTime tDate);
	void	LogprintfDB(const TCHAR *msg, ...);


private:
	BOOL	m_DBopen;
	CDatabase *m_pDB;

	BOOL	m_IntegratedSecurity;
	CString m_DBserver;
	CString m_Database;
	CString m_DBuser;
	CString m_DBpassword;
	BOOL	m_PersistentConnection;
};
