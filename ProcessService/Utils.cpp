#include "StdAfx.h"
#include "Defs.h"
#include "Utils.h"
#include "Tiffio.h"
#include "Ping.h"
#include "Prefs.h"
#include "EmailClient.h"
#include <string.h> 
#include <direct.h>
#include <winnetwk.h>

#include <CkCrypt2.h>


#include <boost/regex.hpp>
#include <boost/regex/v4/fileiter.hpp>

# ifdef BOOST_MSVC
#  pragma warning(disable: 4244 4267)
#endif

extern CPrefs g_prefs;




CUtils::CUtils(void)
{
}

CUtils::~CUtils(void)
{
}

CString CUtils::Int2String(int n)
{
	CString s;
	s.Format("%d", n);
	return s;
}

CString CUtils::UInt2String(int un)
{
	CString s;
	s.Format("%u", un);
	return s;
}

CString CUtils::LimitErrorMessage(CString s)
{
	if (s.GetLength() >= MAX_ERRMSG)
		return s.Left(MAX_ERRMSG - 1);

	return s;
}

CString CUtils::LimitErrorMessage(TCHAR *szStr)
{
	CString s(szStr);
	return LimitErrorMessage(s);
}



int CUtils::CRC32(CString sFileName)
{
	return CRC32(sFileName, FALSE);
}

int CUtils::CRC32(CString sFileName, BOOL bFastMode)
{
	HANDLE	Hndl;
	BYTE	buf[8192];
	DWORD	nBytesRead;

	if ((Hndl = ::CreateFile(sFileName, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE)
		return 0;
	
	DWORD sum = 0;
	do {
		if (!::ReadFile(Hndl, buf, 8092, &nBytesRead, NULL)) {
			::CloseHandle(Hndl);
			return 0;
		}
		if (nBytesRead > 0) {
			for (int i=0; i<(int)nBytesRead; i++) {
				sum += buf[i];
				// just let it overflow..
			}
		}

		// one block only in fast mode...
		if (bFastMode)
			break;

	}
	while (nBytesRead > 0);

	::CloseHandle(Hndl);

	int nCRC = sum;
	return nCRC;
}


int CUtils::StringSplitter(CString sInput, CString sSeparators, CStringArray &sArr)
{
	sArr.RemoveAll();

	int nElements = 0;
	CString s = sInput;
	s.Trim();

	int v = s.FindOneOf(sSeparators);
	BOOL isLast = FALSE;
	do {
		nElements++;
		if (v != -1) 
			sArr.Add(s.Left(v));	
		else {
			sArr.Add(s);
			isLast = TRUE;
		}
		if (isLast == FALSE) {
			s = s.Mid(v+1);
			v = s.FindOneOf(sSeparators);
		}
	} while (isLast == FALSE);

	return nElements;
}

CString CUtils::LoadFile(CString sFilename)
{
	DWORD			BytesRead;
	TCHAR			readbuffer[4097];
	CString s = "";

	HANDLE			Hndl = CreateFile(sFilename, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);

	if (Hndl == INVALID_HANDLE_VALUE)
		return "";

	if (ReadFile(Hndl, readbuffer, 4096, &BytesRead, NULL)) {
		CloseHandle(Hndl);
		readbuffer[BytesRead] = 0;
		s = readbuffer;
		return s;
	} 
	
	CloseHandle(Hndl);

	return "";
}

BOOL CUtils::LockCheck(CString sFileToLock)
{
	if (g_prefs.m_lockcheckmode == LOCKCHECKMODE_NONE)
		return TRUE;

	// Appempt to lock and read from  file
	OVERLAPPED		Overlapped;
	DWORD			BytesRead, dwSizeHigh;
	TCHAR			readbuffer[4097];
	HANDLE			Hndl;

	if (g_prefs.m_lockcheckmode == LOCKCHECKMODE_READ)
		Hndl = CreateFile(sFileToLock, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, 0);
	else
		Hndl = CreateFile(sFileToLock, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, 0);

	if (Hndl == INVALID_HANDLE_VALUE)
		return FALSE;

	if (g_prefs.m_lockcheckmode == LOCKCHECKMODE_RANGELOCK) {
		DWORD dwSizeLow = ::GetFileSize (Hndl, &dwSizeHigh);
		if (dwSizeLow == -1) {
			CloseHandle(Hndl);
			return FALSE;
		}

		if (!LockFileEx(Hndl, LOCKFILE_FAIL_IMMEDIATELY | LOCKFILE_EXCLUSIVE_LOCK, 0, dwSizeLow, dwSizeHigh, &Overlapped)) {
			CloseHandle(Hndl);
			return FALSE;
		} 
		
		UnlockFileEx(Hndl, 0, dwSizeLow, dwSizeHigh, &Overlapped);
	}

	if (ReadFile(Hndl, readbuffer, 4096, &BytesRead, NULL)) {
		CloseHandle(Hndl);
		return TRUE;
	} 
	
	CloseHandle(Hndl);
	return FALSE;	
}


BOOL CUtils::StableTimeCheck(CString sFileToTest, int nStableTimeSec)
{
	HANDLE	Hndl;
	FILETIME WriteTime, LastWriteTime;
	DWORD dwFileSize,dwLastFileSize;

	if (nStableTimeSec == 0)
		return TRUE;

	Hndl = ::CreateFile(sFileToTest, GENERIC_READ , 0 /*  FILE_SHARE_READ*/, NULL, OPEN_EXISTING, 0, 0);
	if (Hndl == INVALID_HANDLE_VALUE)
		return FALSE;
	::GetFileTime( Hndl, NULL, NULL, &WriteTime);
	::GetFileSize(Hndl, &dwFileSize);
	
	::CloseHandle(Hndl);
	
	::Sleep(nStableTimeSec*1000);

	Hndl = CreateFile(sFileToTest, GENERIC_READ , 0 /*  FILE_SHARE_READ*/, NULL, OPEN_EXISTING, 0, 0);
	if (Hndl == INVALID_HANDLE_VALUE)
		return FALSE;
	GetFileTime( Hndl, NULL, NULL, &LastWriteTime);
	::GetFileSize(Hndl, &dwLastFileSize);
	
	CloseHandle(Hndl);
	
	return (WriteTime.dwLowDateTime == LastWriteTime.dwLowDateTime && WriteTime.dwHighDateTime == LastWriteTime.dwHighDateTime && dwFileSize == dwLastFileSize );
}

DWORD CUtils::GetFileSize(CString sFile)
{
	DWORD	dwFileSizeLow = 0, dwFileSizeHigh = 0;

	HANDLE	Hndl = ::CreateFile(sFile, GENERIC_READ ,0 /*  FILE_SHARE_READ*/, NULL, OPEN_EXISTING, 0, 0);
	if (Hndl == INVALID_HANDLE_VALUE)
		return 0;

	dwFileSizeLow = ::GetFileSize(Hndl, &dwFileSizeHigh);
	::CloseHandle(Hndl);

	return dwFileSizeLow;
}


CTime CUtils::GetWriteTime(CString sFile)
{
	FILETIME WriteTime, LocalWriteTime;
	SYSTEMTIME SysTime;
	CTime t(1975,1,1,0,0,0);
	BOOL ok = TRUE;

	HANDLE Hndl = ::CreateFile(sFile, GENERIC_READ , 0 /*  FILE_SHARE_READ*/, NULL, OPEN_EXISTING, 0, 0);
	if (Hndl == INVALID_HANDLE_VALUE)
		return t;

	ok = ::GetFileTime( Hndl, NULL, NULL, &WriteTime);

	CloseHandle(Hndl);
	if (ok) {
		::FileTimeToLocalFileTime( &WriteTime, &LocalWriteTime );
		::FileTimeToSystemTime(&LocalWriteTime, &SysTime);
		CTime t2((int)SysTime.wYear,(int)SysTime.wMonth,(int)SysTime.wDay,(int)SysTime.wHour,(int)SysTime.wMinute, (int)SysTime.wSecond); 
		t = t2;
	}

	return t;
}

BOOL CUtils::FileExist(CString sFile)
{
	HANDLE hFind;
	WIN32_FIND_DATA  ff32;
	BOOL ret = FALSE;

	hFind = ::FindFirstFile(sFile, &ff32);
	if (hFind != INVALID_HANDLE_VALUE) {
		if (!(ff32.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			ret = TRUE;
	}

	FindClose(hFind);
	return ret;
}


CString CUtils::FindFile(CString sFile)
{
	HANDLE hFind;
	WIN32_FIND_DATA  ff32;
	CString  sFileFound = "";

	hFind = ::FindFirstFile(sFile, &ff32);
	if (hFind != INVALID_HANDLE_VALUE) {
		if (!(ff32.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			sFileFound = ff32.cFileName;
	}

	FindClose(hFind);
	return sFileFound;
}

BOOL CUtils::CheckFolder(CString sFolder)
{
	TCHAR	szCurrentDir[_MAX_PATH];
	BOOL	ret = TRUE;

	::GetCurrentDirectory(_MAX_PATH, szCurrentDir);

	if (_tchdir(sFolder.GetBuffer(255)) == -1) 
		ret = FALSE;

	sFolder.ReleaseBuffer();

	::SetCurrentDirectory(szCurrentDir);
	
	return ret;
}

BOOL CUtils::CheckFolderWithPing(CString sFolder)
{
	TCHAR	szCurrentDir[_MAX_PATH];
	BOOL	ret = TRUE;
	CString sServerName = "";

	if (g_prefs.m_bypasspingtest)
		return CheckFolder(sFolder);

	// Resolve mapped drive connection
	if (sFolder.Mid(1,1) == _T(":")) {
		TCHAR szRemoteName[MAX_PATH];
		DWORD lpnLength = MAX_PATH;

		if (::WNetGetConnection(sFolder.Mid(0,2), szRemoteName, &lpnLength) == NO_ERROR)
			sFolder = szRemoteName;
	}


	if (sFolder.Mid(0,2) == _T("\\\\") )
		sServerName = sFolder.Mid(2);

	int n = sServerName.Find("\\");
	if (n != -1)
		sServerName = sServerName.Left(n);

	 BOOL 	   bSuccess = FALSE;
	if (sServerName != "") {
		WSADATA wsa;
		if (WSAStartup(MAKEWORD(2, 0), &wsa) != 0)
			return FALSE;

		CPing p;
		CPingReply pr;
		bSuccess = p.PingUsingWinsock(sServerName, pr, 30, 1000, 32, 0, FALSE);
		WSACleanup();
	}

	if (sServerName != "" && bSuccess == FALSE)
		return FALSE;

	::GetCurrentDirectory(_MAX_PATH, szCurrentDir);

	if ((_tchdir(sFolder.GetBuffer(255))) == -1) 
		ret = FALSE;

	sFolder.ReleaseBuffer();

	::SetCurrentDirectory(szCurrentDir);
	
	return ret;
}



BOOL CUtils::Reconnect(CString sFolder, CString sUser, CString sPW)
{
	if (g_prefs.m_bypassreconnect)
		return TRUE;

	//if (CheckFolderWithPing(sFolder) == FALSE)
	//	return FALSE;
	NETRESOURCE nr; 
	nr.dwScope = NULL; 
	nr.dwDisplayType = RESOURCEDISPLAYTYPE_GENERIC;
	nr.dwUsage = RESOURCEDISPLAYTYPE_GENERIC;  //RESOURCEUSAGE_CONNECTABLE 
	nr.dwType = RESOURCETYPE_DISK;
	nr.lpLocalName = NULL;
	nr.lpComment = NULL;
	nr.lpProvider = NULL;
	nr.lpRemoteName = sFolder.GetBuffer(260);
	DWORD dwResult = WNetAddConnection2(&nr, // NETRESOURCE from enumeration 
									sUser != "" ? sPW.GetBuffer(200) : NULL,                  // no password 
									sUser != "" ? (LPSTR) sUser.GetBuffer(200) : NULL,                  // logged-in user 
										CONNECT_UPDATE_PROFILE);       // update profile with connect information 
	sFolder.ReleaseBuffer();
	sUser.ReleaseBuffer();
	sPW.ReleaseBuffer();

	// Try alternative connection method...
	if (sPW == "")
		sPW = "\"\"";
	if (dwResult != NO_ERROR) {
		CString commandLine = "NET USE \\\\"+sFolder + " " + sPW + " /USER:" + sUser;
		return	DoCreateProcess(commandLine, 300);
	}

	return CheckFolderWithPing(sFolder);
}

int CUtils::DoCreateProcess(CString sCmdLine, int nTimeout, BOOL bBlocking, DWORD &dwExitCode, DWORD &dwProcessID)
{

	STARTUPINFO			si;
	PROCESS_INFORMATION pi;
	dwExitCode = 0;
	dwProcessID = 0;


	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));


	si.dwFlags = STARTF_USESHOWWINDOW;

	si.wShowWindow = SW_HIDE;


	// Start the child process. 
	TCHAR szCmdLine[MAX_PATH * 10];
	strcpy(szCmdLine, sCmdLine);
	if (!::CreateProcess(NULL,	// No module name (use command line). 
		szCmdLine,				// Command line. 
		NULL,					// Process handle not inheritable. 
		NULL,					// Thread handle not inheritable. 
		FALSE,					// Set handle inheritance to FALSE. 
		0,						// No creation flags. 
		NULL,					// Use parent's environment block. 
		NULL,					// Use parent's starting directory. 
		&si,					// Pointer to STARTUPINFO structure.
		&pi)) {				// Pointer to PROCESS_INFORMATION structure.
		CString sMsg;
		sMsg.Format("Create Process for external script file failed. (%s)", szCmdLine);
		//AfxMessageBox(sMsg);
		//LogprintfResample("%s", sMsg);
		return FALSE;
	}

	dwProcessID = pi.dwProcessId;

	DWORD retWaitForInputIdle = 0;
	DWORD retWaitForSingleObject = 0;
	if (bBlocking) {

		retWaitForInputIdle = ::WaitForInputIdle(pi.hProcess, nTimeout > 0 ? nTimeout * 1000 : INFINITE);
		retWaitForSingleObject = ::WaitForSingleObject(pi.hProcess, nTimeout > 0 ? nTimeout * 1000 : INFINITE);
		::GetExitCodeProcess(pi.hProcess, &dwExitCode);
	}

	int ret = TRUE;
	if (nTimeout > 0 && (retWaitForInputIdle == WAIT_TIMEOUT || retWaitForSingleObject == WAIT_TIMEOUT))
		ret = -1;

	::CloseHandle(pi.hProcess);
	::CloseHandle(pi.hThread);
	return ret;
}


CString CUtils::GetComputerName()
{
	TCHAR szComputer[255];
	DWORD dwLen = 255;
	::GetComputerName(szComputer, &dwLen);

	CString s(szComputer);
	return s;
}

BOOL CUtils::DoCreateProcess(CString sCmdLine, int nTimeout)
{
    STARTUPINFO			si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

    // Start the child process. 
	TCHAR szCmdLine[MAX_PATH*4];
	strcpy(szCmdLine, sCmdLine);
	if( !::CreateProcess( NULL,	// No module name (use command line). 
						szCmdLine,				// Command line. 
						NULL,					// Process handle not inheritable. 
						NULL,					// Thread handle not inheritable. 
						FALSE,					// Set handle inheritance to FALSE. 
						0,						// No creation flags. 
						NULL,					// Use parent's environment block. 
						NULL,					// Use parent's starting directory. 
						&si,					// Pointer to STARTUPINFO structure.
						&pi ) )	{				// Pointer to PROCESS_INFORMATION structure.
		
		LogprintfResample("Create Process for external script file failed. (%s)", szCmdLine);
		
	   return FALSE;
    }

	::WaitForInputIdle(pi.hProcess, nTimeout>0 ? nTimeout*1000 : INFINITE);
	::WaitForSingleObject( pi.hProcess, nTimeout>0 ? nTimeout*1000 : INFINITE );
	::CloseHandle( pi.hProcess );
	::CloseHandle( pi.hThread );
	return TRUE;
}

CString CUtils::GetExtension(CString sFileName)
{
	int n = sFileName.ReverseFind('.');
	if (n == -1)
		return _T("");
	return sFileName.Mid(n+1);
}

CString CUtils::GetFileName(CString sFullName)
{
	int n = sFullName.ReverseFind('\\');
	if (n == -1)
		return sFullName;
	return sFullName.Mid(n+1);
}

CString CUtils::GetFilePath(CString sFullName)
{
	int n = sFullName.ReverseFind('\\');
	if (n == -1)
		return sFullName;
	return sFullName.Left(n);
}


CString CUtils::GetFileName(CString sFullName, BOOL bExcludeExtension)
{
	if (sFullName == _T(""))
		return sFullName;

	if (bExcludeExtension) {
		int m = sFullName.ReverseFind('.');
		if (m != -1)
			sFullName = sFullName.Left(m);
	}
	int n = sFullName.ReverseFind('\\');
	if (n == -1)
		return sFullName;
	return sFullName.Mid(n+1);
}


int CUtils::GetFileAgeHours(CString sFileName)
{
	if (FileExist(sFileName) == FALSE)
		return 100000;
	CTime t1 = GetWriteTime(sFileName);
	CTime t2 = CTime::GetCurrentTime();
	CTimeSpan ts = t2 - t1;
	if (ts.GetTotalHours() > 0)
		return ts.GetTotalHours();
	
	return -1;
}



int CUtils::GetFileAgeMinutes(CString sFileName)
{
	if (FileExist(sFileName) == FALSE)
		return -1;
	CTime t1 = GetWriteTime(sFileName);
	CTime t2 = CTime::GetCurrentTime();
	CTimeSpan ts = t2 - t1;
	if (ts.GetTotalMinutes() > 0)
		return ts.GetTotalMinutes();
	
	return -1;
}


CString  CUtils::GetFirstFile(CString sSpoolFolder, CString sSearchMask)
{
	WIN32_FIND_DATA	fdata;
	HANDLE			fHandle;
	TCHAR			szNameSearch[MAX_PATH];		

	CString sFoundFile = "";
	if (CheckFolderWithPing(sSpoolFolder) == FALSE)
		return "";
	
	if (sSpoolFolder.Right(1) != "\\")
		sSpoolFolder += _T("\\");

	_stprintf(szNameSearch, "%s%s",(LPCTSTR)sSpoolFolder, (LPCTSTR)sSearchMask);
	fHandle = ::FindFirstFile(szNameSearch, &fdata);
	if (fHandle == INVALID_HANDLE_VALUE) {
		// All empty
		return "";
	}

	do {
		if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )) {
			sFoundFile.Format( "%s\\%s", (LPCTSTR)sSpoolFolder, fdata.cFileName);	
			break;
		} 
		if (!::FindNextFile(fHandle, &fdata)) 
			break;
	} while (TRUE);

	::FindClose(fHandle);
	
	return sFoundFile;
}

int CUtils::GetFolderList(CString sSpoolFolder, CStringArray &sList)
{
	WIN32_FIND_DATA	fdata;
	HANDLE			fHandle;
	TCHAR			NameSearch[MAX_PATH];		
	int				nFolders=0;
	
	sList.RemoveAll();
	
	if (CheckFolderWithPing(sSpoolFolder) == FALSE)
		return -1;

	if (sSpoolFolder.Right(1) != "\\")
		sSpoolFolder += _T("\\");

	_stprintf(NameSearch, "%s\\*.*",(LPCTSTR)sSpoolFolder);
	fHandle = ::FindFirstFile(NameSearch, &fdata);
	if (fHandle == INVALID_HANDLE_VALUE) {
		// All empty
		return 0;
	}

	// Got something
	do {
			
		CString sFolder = fdata.cFileName;

		if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) && sFolder != _T(".") && sFolder != _T("..")) {  
			
			sList.Add(sFolder);
			nFolders++;
		}

		if (!::FindNextFile(fHandle, &fdata)) 
			break;

	} while (TRUE);

	::FindClose(fHandle);

	return nFolders;
}

int CUtils::ScanDir(CString sSpoolFolder, CString sSearchMask, FILEINFOSTRUCT aFileList[], int nMaxFiles, CString sIgnoreMask,
																					BOOL bIgnoreHidden, BOOL bKillZeroByteFiles)
{
	return ScanDir(sSpoolFolder, sSearchMask, aFileList, nMaxFiles, sIgnoreMask,
									bIgnoreHidden, bKillZeroByteFiles, FALSE, 0, NULL);
}

int CUtils::ScanDir(CString sSpoolFolder, CString sSearchMask, FILEINFOSTRUCT aFileList [], int nMaxFiles, CString sIgnoreMask, 
					BOOL bIgnoreHidden, BOOL bKillZeroByteFiles, BOOL bUseRegex, int nRegex, REGEXPSTRUCT pRegex [])
{
	WIN32_FIND_DATA	fdata;
	HANDLE			fHandle;
	TCHAR			NameSearch[MAX_PATH], FoundFile[MAX_PATH];		
	int				nFiles=0;
	FILETIME		WriteTime, CreateTime, LocalWriteTime;
	SYSTEMTIME		SysTime;
	
	if (CheckFolderWithPing(sSpoolFolder) == FALSE)
		return -1;
	
	if (sSpoolFolder.Right(1) != "\\")
		sSpoolFolder += _T("\\");

	_stprintf(NameSearch, "%s%s",(LPCTSTR)sSpoolFolder, (LPCTSTR)sSearchMask);
	fHandle = ::FindFirstFile(NameSearch, &fdata);
	if (fHandle == INVALID_HANDLE_VALUE) {
		// All empty
		return 0;
	}

	// Got something
	do {

		if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )) {


			if ( bIgnoreHidden == FALSE || 
				(bIgnoreHidden && (fdata.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) == 0) ||
				(bIgnoreHidden && (fdata.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) == 0)) {

				BOOL bFilterOK = TRUE;

				if (sIgnoreMask != "" && sIgnoreMask != "*" && sIgnoreMask != "*.*") {
					CString s1 = GetExtension(fdata.cFileName);
					CString s2 = GetExtension(sIgnoreMask);
					if (s1.CompareNoCase(s2) == 0)
						bFilterOK  = FALSE;
				}

				CString sFile = fdata.cFileName;
				if ( sFile.CompareNoCase(".DS_Store") == 0 || sFile.CompareNoCase("Thumbs.db") == 0)
					bFilterOK  = FALSE;

				if (bFilterOK && bUseRegex) {
					bFilterOK = FALSE;
					
					
					for (int reg=0; reg <nRegex; reg++) {
						bFilterOK = TryMatchExpression(pRegex[reg].m_matchExpression, sFile, pRegex[reg].m_partialmatch);
						if (bFilterOK)
							break;
					}
					if (bFilterOK == FALSE) {
						sFile.MakeUpper();
						for (int reg=0; reg <nRegex; reg++) {
							bFilterOK = TryMatchExpression(pRegex[reg].m_matchExpression, sFile, pRegex[reg].m_partialmatch);
							if (bFilterOK)
								break;
						}
					}
					if (bFilterOK == FALSE) {
						sFile.MakeLower();
						for (int reg=0; reg <nRegex; reg++) {
							bFilterOK = TryMatchExpression(pRegex[reg].m_matchExpression, sFile, pRegex[reg].m_partialmatch);
							if (bFilterOK)
								break;
						}
					}


				}

				if (bFilterOK) {

					_stprintf(FoundFile, "%s\\%s", (LPCTSTR)sSpoolFolder, fdata.cFileName);	
					HANDLE Hndl = INVALID_HANDLE_VALUE;
					if (g_prefs.m_lockcheckmode == LOCKCHECKMODE_READ)
						Hndl = ::CreateFile(FoundFile, GENERIC_READ , FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
					else if (g_prefs.m_lockcheckmode != LOCKCHECKMODE_NONE)
						Hndl = ::CreateFile(FoundFile, GENERIC_READ , FILE_SHARE_READ |FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
					if (Hndl != INVALID_HANDLE_VALUE || g_prefs.m_lockcheckmode == LOCKCHECKMODE_NONE) {
			
						int ok = TRUE;
						if (g_prefs.m_lockcheckmode != LOCKCHECKMODE_NONE) {
							ok = ::GetFileTime( Hndl, &CreateTime, NULL, &WriteTime);
							::CloseHandle(Hndl);
						} else {
							CreateTime.dwHighDateTime = fdata.ftCreationTime.dwHighDateTime;
							CreateTime.dwLowDateTime = fdata.ftCreationTime.dwLowDateTime;
							WriteTime.dwHighDateTime = fdata.ftLastWriteTime.dwHighDateTime;
							WriteTime.dwLowDateTime = fdata.ftLastWriteTime.dwLowDateTime;
						}

						if (bKillZeroByteFiles) {
							if (fdata.nFileSizeLow == 0 && fdata.nFileSizeHigh == 0 && GetFileAgeMinutes(FoundFile) > 5) {
								::DeleteFile(FoundFile);
								ok  = FALSE;
							}
						}

						if (ok && fdata.nFileSizeLow == 0 && fdata.nFileSizeHigh == 0)
							ok  = FALSE;

						if (ok) {
							aFileList[nFiles].sFolder = sSpoolFolder;
							aFileList[nFiles].sFileName = fdata.cFileName;
							aFileList[nFiles].nFileSize = fdata.nFileSizeLow;

							// Store create time for FIFO sorting

							FileTimeToLocalFileTime( g_prefs.m_bSortOnCreateTime ? &CreateTime :  &WriteTime, &LocalWriteTime );
							FileTimeToSystemTime(&LocalWriteTime, &SysTime);
							CTime t((int)SysTime.wYear,(int)SysTime.wMonth,(int)SysTime.wDay,(int)SysTime.wHour,(int)SysTime.wMinute, (int)SysTime.wSecond); 
							aFileList[nFiles].tJobTime = t;	
		

							// Store last write time for stable time checker

							FileTimeToLocalFileTime( &WriteTime, &LocalWriteTime );
							FileTimeToSystemTime(&LocalWriteTime, &SysTime);
							CTime t2((int)SysTime.wYear,(int)SysTime.wMonth,(int)SysTime.wDay,(int)SysTime.wHour,(int)SysTime.wMinute, (int)SysTime.wSecond); 
							aFileList[nFiles++].tWriteTime = t2;
						}
					}	
				}
			} 
		}

		if (!::FindNextFile(fHandle, &fdata)) 
			break;

	} while (nFiles < nMaxFiles);

	::FindClose(fHandle);

	// Sort found files on create-time
	CTime		tTmpTimeValueCTime;
	CString 	sTmpFileName;
	DWORD		nTmpFileSize;
	for (int i=0;i<nFiles-1;i++) {
		for (int j=i+1;j<nFiles;j++) {
			if (aFileList[i].tJobTime > aFileList[j].tJobTime) {

				// Swap elements
				sTmpFileName = aFileList[i].sFileName; 
				aFileList[i].sFileName = aFileList[j].sFileName;
				aFileList[j].sFileName = sTmpFileName;				

				sTmpFileName = aFileList[i].sFolder;
				aFileList[i].sFolder = aFileList[j].sFolder;
				aFileList[j].sFolder = sTmpFileName;

				tTmpTimeValueCTime = aFileList[i].tJobTime;
				aFileList[i].tJobTime = aFileList[j].tJobTime;
				aFileList[j].tJobTime = tTmpTimeValueCTime;

				nTmpFileSize = aFileList[i].nFileSize;
				aFileList[i].nFileSize = aFileList[j].nFileSize;
				aFileList[j].nFileSize = nTmpFileSize;

				tTmpTimeValueCTime = aFileList[i].tWriteTime;
				aFileList[i].tWriteTime = aFileList[j].tWriteTime;
				aFileList[j].tWriteTime = tTmpTimeValueCTime;
			}
		}
	}

	return nFiles;
}

CTime CUtils::GetLogFileTime(CString sLogFileName)
{
	HANDLE	Hndl;
	char	infbuf[257];
	DWORD	nBytesRead;

	CTime tm(1975, 1, 1, 0, 0, 0);

	if ((Hndl = ::CreateFile(sLogFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE)
		return tm;

	if (!::ReadFile(Hndl, infbuf, 256, &nBytesRead, NULL)) {
		::CloseHandle(Hndl);
		return tm;
	}

	::CloseHandle(Hndl);

	infbuf[nBytesRead == 256 ? 255 : nBytesRead] = 0;

	// [212312313] 2009-01-02 19:28:12

	CString s = infbuf;
	int n = s.Find("] ");
	if (n != -1)
		s = s.Mid(n + 2).Trim();

	//	LogprintfPoll("INFO:  InputTime from error log file %s", s);

	if (atoi(s) > 2000)
		return String2Time(s);

	return tm;
}


CTime CUtils::String2Time(CString s)
{
	// 2010-01-22 23:24:25

	CTime tm(1975, 1, 1, 0, 0, 0);

	try {

		if (s.GetLength() < 19)
			return tm;

		int y = atoi(s.Mid(0, 4));
		int m = atoi(s.Mid(5, 2));
		int d = atoi(s.Mid(8, 2));
		int h = atoi(s.Mid(11, 2));
		int mi = atoi(s.Mid(14, 2));
		int sec = atoi(s.Mid(17, 2));


		CTime tm2(y, m, d, h, mi, sec);
		tm = tm2;
	}
	catch (CException *ex)
	{
	}
	catch (...)
	{
	}

	return tm;
}

void CUtils::ErrorFilePrintf(CString sFileName, char *msg, ...)
{
	TCHAR	szLogLine[16000], szFinalLine[16000];
	va_list	ap;
	DWORD	n, nBytesWritten;
	SYSTEMTIME	ltime;

	HANDLE hFile = ::CreateFile(sFileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	// Seek to end of file
	::SetFilePointer(hFile, 0, NULL, FILE_END);

	va_start(ap, msg);
	n = vsprintf(szLogLine, msg, ap);
	va_end(ap);
	szLogLine[n] = '\0';

	::GetLocalTime(&ltime);
	_stprintf(szFinalLine, "[%.2d-%.2d %.2d:%.2d:%.2d.%.3d]  %s\r\n", (int)ltime.wDay, (int)ltime.wMonth, (int)ltime.wHour, (int)ltime.wMinute, (int)ltime.wSecond, (int)ltime.wMilliseconds, szLogLine);

	::WriteFile(hFile, szFinalLine, (DWORD)_tcsclen(szFinalLine), &nBytesWritten, NULL);

	::CloseHandle(hFile);
}




BOOL CUtils::GetCCDATAFreeSpaceMB(DWORD &i32FreeMB)
{
	ULARGE_INTEGER	i64FreeBytesToCaller;
	ULARGE_INTEGER	i64TotalBytes;
	ULARGE_INTEGER	i64FreeBytes;

	BOOL fResult = GetDiskFreeSpaceEx(g_prefs.m_hiresPDFPath,
		(PULARGE_INTEGER)&i64FreeBytesToCaller,
		(PULARGE_INTEGER)&i64TotalBytes,
		(PULARGE_INTEGER)&i64FreeBytes);

	if (fResult == FALSE)
		return FALSE;

	ULONGLONG i64FreeMB = i64FreeBytes.QuadPart / (1024 * 1024);
	i32FreeMB = (DWORD)i64FreeMB;
	return TRUE;
}

CTime CUtils::Systime2CTime(SYSTEMTIME SysTime)
{
	CTime t = CTime(1975, 1, 1, 0, 0, 0);
	try {
		t = CTime((int)SysTime.wYear, (int)SysTime.wMonth, (int)SysTime.wDay, (int)SysTime.wHour, (int)SysTime.wMinute, (int)SysTime.wSecond);
	}
	catch (CException *ex)
	{
		;
	}

	return t;
}

CString CUtils::GenerateTimeStamp()
{
	CTime t = CTime::GetCurrentTime();

	CString s;
	s.Format("%.4d%.2d%.2d%.2d%.2d%.2d", t.GetYear(), t.GetMonth(), t.GetDay(), t.GetHour(), t.GetMinute(), t.GetSecond());

	return s;

}


int CUtils::TokenizeName(CString sName, CString sSeparators, CString sv[], CString sp[])
{
	int nCount = 0;
	CString s = sName;
	while (s.GetLength() > 0) {

		int en = s.FindOneOf(sSeparators);
		if (en == -1) {
			sp[nCount] = "";
			sv[nCount++] = s;	
			break;
		} 
		sp[nCount] = s[en];
		sv[nCount++] = s.Left(en);

		s = s.Mid(en+1);
	}

	return nCount;
}	

BOOL CUtils::TryMatchExpression(CString sMatchExpression, CString sInputString, BOOL bPartialMatch)
{
	boost::regex	e;
	BOOL			ret;
	
	TCHAR szMatch[1024], szInput[1024];
	strcpy(szMatch, sMatchExpression);
	strcpy(szInput, sInputString);

	try {
		e.assign(szMatch);
		ret = boost::regex_match(szInput, e, bPartialMatch ? boost::match_partial|boost::match_default : boost::match_default);
	}
	catch(std::exception& e)
    {
         return FALSE;
    }
	
	return ret;
}

CString CUtils::FormatExpression(CString sMatchExpression, CString sFormatExpression, CString sInputString, BOOL bPartialMatch)
{
	boost::regex	e;
//	unsigned int				match_flag = bPartialMatch ? boost::match_partial : boost::match_default;
// 	unsigned int				flags = match_flag | boost::format_perl;
	CString			retSt = sInputString;
	
	TCHAR szMatch[1024], szInput[1024], szFormat[1024];
	strcpy(szMatch, sMatchExpression);
	strcpy(szInput, sInputString);
	strcpy(szFormat, sFormatExpression);
	
	std::string in = szInput;

	try {
		e.assign(szMatch);
	}
	catch(std::exception& e)
    {
         return retSt;
    }

	try {
		BOOL ret = boost::regex_match(in, e, bPartialMatch ? boost::match_partial|boost::match_default : boost::match_default);
		
		if (ret == TRUE) {
			std::string r = boost::regex_merge(in, e, szFormat, boost::format_perl | (bPartialMatch ? boost::match_partial|boost::match_default : boost::match_default));
			retSt = r.data();
		}
	}
	catch(std::exception& e)
    {
         return retSt;
    }
	return retSt;
}

CString CUtils::GetRawMaskEx(CString sOrgMask)
{
	int n = 0; //, nIndex = -1;
	CString sNamingMask = sOrgMask.MakeLower();
	CString sResult = _T("");

	while (n < sNamingMask.GetLength()) {

		if (sNamingMask[n] != _TCHAR('%')) {
			sResult += sNamingMask.Mid(n,1);
			n++;
			continue;
		}
		
		if (n + 1 < sNamingMask.GetLength())
			n++;

		// See if we have repeater instruction (digits)
		int reps = _tstoi(sNamingMask.Mid(n));
		if (reps > 0) {
			// Skip over digits
			while (n < sNamingMask.GetLength()) {
				if (isdigit((int)(sNamingMask[n])))
					n++;
				else
					break;
			}
		} else {
			reps = 1;
		}
		
		if (n >= sNamingMask.GetLength())
			break; // no more string..

		// Store id character	
		CString id = sNamingMask.Mid(n,1);
		for (int i=0; i<reps; i++)
			sResult += id.MakeUpper();

		// Skip over id
		n++;
		
		//	See if if have special instructions in brackets
		if (n  >= sNamingMask.GetLength()) 
			break;
		
	}
	return sResult;
}

BOOL CUtils::IsolateReference(CString sNameFormat, CString sFileName, CString &sColor, CString &sPageNumber) 
{
	int n=0;
	sPageNumber = _T("");
	sColor = _T("");
	if (sFileName == "")
		return FALSE;

	sNameFormat.MakeUpper();
	if (sNameFormat.Find("%P") == -1 && sNameFormat.Find("%C") == -1)
		return FALSE;

	CString sRawMask = GetRawMaskEx(sNameFormat);

	BOOL bIsFullyFixed = TRUE;
	if (sRawMask.FindOneOf("-_.") != -1) 
		bIsFullyFixed = FALSE;

	if (bIsFullyFixed) {
		// Standard fixed size naming scheme
		while (n < sRawMask.GetLength()) {
			switch (sRawMask[n]) {
			case 'P':
				sPageNumber += sFileName[n];
				break;
			case 'C':
				sColor += sFileName[n];
				break;
			}
			n++;
		}
	} else {
		CString sIsolatedName;
		
		int m = 0, mm = 0;
		while (n < sRawMask.GetLength() && m < sFileName.GetLength()) {

			// Isolate next id (until next delimiter)
			mm = sFileName.Mid(m).FindOneOf("-_.");	// Get next delimiter in job name						
			if (mm != -1)
				sIsolatedName = sFileName.Mid(m,mm-m+m);
			else
				sIsolatedName = sFileName.Mid(m);
				
			switch (sRawMask[n]) {
				case 'P':
					sPageNumber = sIsolatedName;
					break;
				case 'C':
					sColor = sIsolatedName;
					break;
				default:
					// Don't care character
					break;
			}
			m += mm+1;
			n++;
			if (sRawMask.Mid(n).FindOneOf("-_.") != -1) 
				n++;
		}		
	}

	
	return TRUE;
}



CString CUtils::ReadTiffImageDescription(CString sFullInputPath)
{
	/*TIFF *tif;
	char *szTag;
	CString s = "";

	if ((tif = TIFFOpen(sFullInputPath, "r")) == NULL) 
		return "";
	TIFFGetFieldDefaulted(tif, TIFFTAG_IMAGEDESCRIPTION, &szTag);

	if (szTag != NULL)
		s = szTag;
	TIFFClose(tif);

	return s;*/
	return "";
}


BOOL CUtils::DoCreateProcess(CString sCmdLine)
{
    STARTUPINFO			si;
    PROCESS_INFORMATION pi;

    ZeroMemory( &si, sizeof(si) );
    si.cb = sizeof(si);

	si.dwFlags = STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_HIDE;

//    HANDLE  hStdOutput; 
//    HANDLE  hStdError; 

    // Start the child process. 
	TCHAR szCmdLine[MAX_PATH];
	strcpy(szCmdLine, sCmdLine);
	if( !::CreateProcess( NULL,	// No module name (use command line). 
						szCmdLine,				// Command line. 
						NULL,					// Process handle not inheritable. 
						NULL,					// Thread handle not inheritable. 
						FALSE,					// Set handle inheritance to FALSE. 
						0,						// No creation flags. 
						NULL,					// Use parent's environment block. 
						NULL,					// Use parent's starting directory. 
						&si,					// Pointer to STARTUPINFO structure.
						&pi ) )	{				// Pointer to PROCESS_INFORMATION structure.
		LogprintfConvert("ERROE: Create Process for external script file failed. (%s)", szCmdLine);
	   return FALSE;
    }
	
	::WaitForInputIdle(pi.hProcess, g_prefs.m_scripttimeout>0 ? g_prefs.m_scripttimeout*1000 : INFINITE);
    // Wait until child process exits.
	::WaitForSingleObject( pi.hProcess, g_prefs.m_scripttimeout>0 ? g_prefs.m_scripttimeout*1000 : INFINITE );

    // Close process and thread handles. 
	::CloseHandle( pi.hProcess );
	::CloseHandle( pi.hThread );
	return TRUE;
}

int CUtils::DeleteFiles(CString sSpoolFolder, CString sSearchMask)
{
	WIN32_FIND_DATA	fdata;
	HANDLE			fHandle;
	TCHAR			NameSearch[MAX_PATH], FoundFile[MAX_PATH];		
	int				nFiles=0;
	CStringArray	arr;

	if (CheckFolderWithPing(sSpoolFolder) == FALSE)
		return -1;
	
	if (sSpoolFolder.Right(1) != "\\")
		sSpoolFolder += _T("\\");

	_stprintf(NameSearch, "%s%s",(LPCTSTR)sSpoolFolder, (LPCTSTR)sSearchMask);
	fHandle = ::FindFirstFile(NameSearch, &fdata);
	if (fHandle == INVALID_HANDLE_VALUE) {
		// All empty
		return 0;
	}

	// Got something
	do {
		if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )) {
			_stprintf(FoundFile, "%s\\%s", (LPCTSTR)sSpoolFolder, fdata.cFileName);	
			HANDLE Hndl = CreateFile(FoundFile, GENERIC_READ , FILE_SHARE_READ |FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			if (Hndl != INVALID_HANDLE_VALUE) {
				CloseHandle(Hndl);
				arr.Add(fdata.cFileName);
				nFiles++;
			}			
		} 
		
		if (!::FindNextFile(fHandle, &fdata)) 
			break;

	} while (TRUE);

	::FindClose(fHandle);

	for (int i=0; i<arr.GetCount(); i++)
		DeleteFile(sSpoolFolder + arr.ElementAt(i));

	return nFiles;
}

int CUtils::DeleteFiles(CString sSpoolFolder, CString sSearchMask, int sMaxAgeHours)
{
	WIN32_FIND_DATA	fdata;
	HANDLE			fHandle;
	TCHAR			NameSearch[MAX_PATH], FoundFile[MAX_PATH];		
	int				nFiles=0;
	CStringArray	arr;
	FILETIME ftm ;
	FILETIME ltm;
	SYSTEMTIME  stm;

	CTime tNow = CTime::GetCurrentTime();

	if (CheckFolder(sSpoolFolder) == FALSE)
		return -1;
	
	if (sSpoolFolder.Right(1) != "\\")
		sSpoolFolder += _T("\\");

	_stprintf(NameSearch, "%s%s",(LPCTSTR)sSpoolFolder, (LPCTSTR)sSearchMask);
	fHandle = ::FindFirstFile(NameSearch, &fdata);
	if (fHandle == INVALID_HANDLE_VALUE) {
		// All empty
		return 0;
	}

	
	do {
		if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )) {
			_stprintf(FoundFile, "%s\\%s", (LPCTSTR)sSpoolFolder, fdata.cFileName);	
			HANDLE Hndl = CreateFile(FoundFile, GENERIC_READ , FILE_SHARE_READ |FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			if (Hndl != INVALID_HANDLE_VALUE) {
				CloseHandle(Hndl);
				ftm = fdata.ftLastWriteTime;
				::FileTimeToLocalFileTime( &ftm, &ltm );
				::FileTimeToSystemTime(&ltm, &stm);

				CTime tFileAge((int)stm.wYear,(int)stm.wMonth,(int)stm.wDay,(int)stm.wHour,(int)stm.wMinute, (int)stm.wSecond); 
				CTimeSpan ts = tNow - tFileAge;
				if (ts.GetTotalHours() >=sMaxAgeHours) {
					arr.Add(fdata.cFileName);
					nFiles++;
				}
			}			
		} 
		
		if (!::FindNextFile(fHandle, &fdata)) 
			break;

	} while (TRUE);

	::FindClose(fHandle);

	for (int i=0; i<arr.GetCount(); i++)
		DeleteFile(sSpoolFolder + arr.ElementAt(i));

	return nFiles;
}

BOOL CUtils::MoveFileWithRetry(CString sSourceFile, CString sDestinationFile, CString &sErrorMessage)
{
	if (::MoveFileEx(sSourceFile, sDestinationFile, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == FALSE) {
		::Sleep(1000);
		if (::MoveFileEx(sSourceFile, sDestinationFile, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == FALSE) {
			sErrorMessage = GetLastWin32Error();
			return FALSE;
		}
	}

	return TRUE;
}

BOOL CUtils::CopyFileWithRetry(CString sSourceFile, CString sDestinationFile, CString &sErrorMessage)
{
	int bCopySuccess = FALSE;
	int nRetries =5;
	if (nRetries <= 1)
		nRetries = 2;
	while (--nRetries >= 0 && bCopySuccess == FALSE) {
		bCopySuccess = ::CopyFile(sSourceFile, sDestinationFile, 0);
		if (bCopySuccess)
			break;
		::Sleep(1000);
		Reconnect(GetFilePath(sDestinationFile), "", "");
	}
	if (bCopySuccess == FALSE)
		sErrorMessage = GetLastWin32Error();
	return bCopySuccess;
}


int CUtils::MoveFiles(CString sSpoolFolder, CString sSearchMask, CString sOutputFolder, BOOL bIgnoreZeroLength)
{
	WIN32_FIND_DATA	fdata;
	HANDLE			fHandle;
	TCHAR			NameSearch[MAX_PATH], FoundFile[MAX_PATH];		
	int				nFiles=0;
	CStringArray	arr;
//	FILETIME ftm ;
//	FILETIME ltm;
//	SYSTEMTIME  stm;

	CTime tNow = CTime::GetCurrentTime();

	if (CheckFolder(sSpoolFolder) == FALSE)
		return -1;
	if (CheckFolder(sOutputFolder) == FALSE)
		return -1;
	
	if (sSpoolFolder.Right(1) != "\\")
		sSpoolFolder += _T("\\");

	if (sOutputFolder.Right(1) != "\\")
		sOutputFolder += _T("\\");


	_stprintf(NameSearch, "%s%s",(LPCTSTR)sSpoolFolder, (LPCTSTR)sSearchMask);
	fHandle = ::FindFirstFile(NameSearch, &fdata);
	if (fHandle == INVALID_HANDLE_VALUE) {
		// All empty
		return 0;
	}

	
	do {
		if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )) {
			_stprintf(FoundFile, "%s\\%s", (LPCTSTR)sSpoolFolder, fdata.cFileName);	

			HANDLE Hndl = CreateFile(FoundFile, GENERIC_READ , FILE_SHARE_READ |FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			if (Hndl != INVALID_HANDLE_VALUE) {
				CloseHandle(Hndl);
				if (bIgnoreZeroLength == FALSE || (bIgnoreZeroLength && fdata.nFileSizeLow>0)) {
					arr.Add(fdata.cFileName);
					nFiles++;
				}
			}			
		} 
		
		if (!::FindNextFile(fHandle, &fdata)) 
			break;

	} while (TRUE);

	::FindClose(fHandle);

	for (int i=0; i<arr.GetCount(); i++)
		::MoveFileEx(sSpoolFolder + arr.ElementAt(i), sOutputFolder + arr.ElementAt(i), MOVEFILE_COPY_ALLOWED|MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH );

	return nFiles;
}


int CUtils::ScanDirCount(CString sSpoolFolder, CString sSearchMask)
{
	WIN32_FIND_DATA	fdata;
	HANDLE			fHandle;
	TCHAR			NameSearch[MAX_PATH];
	int				nFiles=0;
	
	if (CheckFolderWithPing(sSpoolFolder) == FALSE)
		return -1;
	
	if (sSpoolFolder.Right(1) != "\\")
		sSpoolFolder += _T("\\");

	_stprintf(NameSearch, "%s%s",(LPCTSTR)sSpoolFolder, (LPCTSTR)sSearchMask);
	fHandle = ::FindFirstFile(NameSearch, &fdata);
	if (fHandle == INVALID_HANDLE_VALUE) {
		return 0;
	}

	// Got something
	do {
		if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )) {
			CString sFileName(fdata.cFileName);
			if (sFileName != "." && sFileName != ".." && (fdata.nFileSizeLow > 0 || fdata.nFileSizeHigh > 0))
				nFiles++; 
		}
			
		if (!::FindNextFile(fHandle, &fdata)) 
			break;
	} while (TRUE);
	
	::FindClose(fHandle);

	return nFiles;
}

void CUtils::TruncateLogFile(CString sFile, DWORD dwMaxSize)
{
	DWORD dwCurrentSize = GetFileSize(sFile);

	if (dwCurrentSize == 0)
		return;

	if (dwCurrentSize > dwMaxSize) {
		::CopyFile(sFile,sFile + "2",FALSE);
		::DeleteFile(sFile);

	}
}

BOOL CUtils::SetFileToCurrentTime(CString sFile)
{
    FILETIME ft;
    SYSTEMTIME st;
    BOOL ret ;

	HANDLE hFile = ::CreateFile(sFile, FILE_WRITE_ATTRIBUTES , 0, NULL, OPEN_EXISTING, 0, 0);

	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	::GetSystemTime(&st);              // gets current time
	::SystemTimeToFileTime(&st, &ft);  // converts to file time format
	
	ret = ::SetFileTime(hFile, &ft, (LPFILETIME) NULL, &ft);

	::CloseHandle(hFile);

    return ret;
}

void CUtils::LogprintfResample(const TCHAR *msg, ...)
{
	TCHAR	szLogLine[64000], szFinalLine[64000];
	va_list	ap;
	DWORD	n, nBytesWritten;
	SYSTEMTIME	ltime;

	if (g_prefs.m_logToFile == FALSE)
		return;


	TruncateLogFile(g_prefs.m_logFilePath + _T("\\ProofProcess.log"), g_prefs.m_maxlogfilesize);

	HANDLE hFile = ::CreateFile(g_prefs.m_logFilePath + +_T("\\ProofProcess.log"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	// Seek to end of file
	::SetFilePointer(hFile, 0, NULL, FILE_END);

	va_start(ap, msg);
	n = vsprintf(szLogLine, msg, ap);
	va_end(ap);
	szLogLine[n] = '\0';

	::GetLocalTime(&ltime);
	_stprintf(szFinalLine, "[%.2d-%.2d-%.4d %.2d:%.2d:%.2d.%.3d] %s\r\n", (int)ltime.wDay, (int)ltime.wMonth, (int)ltime.wYear, (int)ltime.wHour, (int)ltime.wMinute, (int)ltime.wSecond, (int)ltime.wMilliseconds, szLogLine);

	::WriteFile(hFile, szFinalLine, (DWORD)_tcsclen(szFinalLine), &nBytesWritten, NULL);

	::CloseHandle(hFile);
}



void CUtils::LogprintfConvert(const TCHAR *msg,...)
{
	TCHAR	szLogLine[64000], szFinalLine[64000];
	va_list	ap;
	DWORD	n, nBytesWritten;
	SYSTEMTIME	ltime;

	if (g_prefs.m_logToFile2 == FALSE)
		return;

	//if (g_prefs.m_logToFile2 == FALSE && strstr(msg,"RESPONSE") != NULL)
	//	return;

	TruncateLogFile(g_prefs.m_logFilePath + _T("\\ConvertProcess.log"), g_prefs.m_maxlogfilesize);

	HANDLE hFile = ::CreateFile(g_prefs.m_logFilePath + _T("\\ConvertProcess.log"), GENERIC_WRITE, FILE_SHARE_READ |FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE) 
		return;

	// Seek to end of file
	::SetFilePointer (hFile, 0, NULL, FILE_END); 
	
	va_start(ap, msg);
	n = vsprintf(szLogLine, msg, ap);
	va_end(ap);
	szLogLine[n] = '\0';

	::GetLocalTime(&ltime);
	_stprintf(szFinalLine, "[%.2d-%.2d-%.4d %.2d:%.2d:%.2d.%.3d] %s\r\n", (int)ltime.wDay, (int)ltime.wMonth, (int)ltime.wYear,(int)ltime.wHour, (int)ltime.wMinute, (int)ltime.wSecond, (int)ltime.wMilliseconds, szLogLine);

	::WriteFile(hFile, szFinalLine, (DWORD)_tcsclen(szFinalLine), &nBytesWritten, NULL);

	::CloseHandle(hFile);
}

void CUtils::AliveResampling()
{
	TCHAR	szFinalLine[100];
	DWORD	nBytesWritten;
	SYSTEMTIME	ltime;
	HANDLE hFile;

	hFile = ::CreateFile(g_prefs.m_alivelogFilePath + _T("\\ProcessService_ProofThread.log"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	::GetLocalTime(&ltime);
	_stprintf(szFinalLine, "%.2d-%.2d %.2d:%.2d:%.2d.%.3d\r\n", (int)ltime.wDay, (int)ltime.wMonth, (int)ltime.wHour, (int)ltime.wMinute, (int)ltime.wSecond, (int)ltime.wMilliseconds);
	::WriteFile(hFile, szFinalLine, (DWORD)_tcsclen(szFinalLine), &nBytesWritten, NULL);
	::CloseHandle(hFile);
}

void CUtils::AliveConverting()
{
	TCHAR	szFinalLine[100];
	DWORD	nBytesWritten;
	SYSTEMTIME	ltime;
	HANDLE hFile;

	hFile = ::CreateFile(g_prefs.m_alivelogFilePath + _T("\\ProcessService_ConvertThread.log"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	::GetLocalTime(&ltime);
	_stprintf(szFinalLine, "%.2d-%.2d %.2d:%.2d:%.2d.%.3d\r\n", (int)ltime.wDay, (int)ltime.wMonth, (int)ltime.wHour, (int)ltime.wMinute, (int)ltime.wSecond, (int)ltime.wMilliseconds);
	::WriteFile(hFile, szFinalLine, (DWORD)_tcsclen(szFinalLine), &nBytesWritten, NULL);
	::CloseHandle(hFile);
}



BOOL CUtils::TryMatchExpressionEx(CString sMatchExpression, CString sInputString, BOOL bPartialMatch, CString &sErrorMessage)
{
	sErrorMessage = _T("");

	boost::regex	e;
//	unsigned int	match_flag = bPartialMatch ? boost::match_partial : boost::match_default;
	BOOL			ret;
	
	TCHAR szMatch[1024], szInput[1024];
	strcpy(szMatch, sMatchExpression);
	strcpy(szInput, sInputString);

	try {
		e.assign(szMatch);
	}
	catch(std::exception& e)
    {
		 sErrorMessage =  _T("ERROR in regular expression ");
		 sErrorMessage += e.what();
		 LogprintfConvert("ERROR in regular expression %s",e.what());
         return FALSE;
    }
	try {
		ret = boost::regex_match(szInput, e, bPartialMatch ? boost::match_partial|boost::match_default : boost::match_default);
	}
	catch(std::exception& e)
    {

		 sErrorMessage = _T("ERROR in regular expression ");
		 sErrorMessage += e.what();
		 LogprintfConvert("ERROR in regular expression %s",e.what());
         return FALSE;
    }
	
	return ret;
}

CString CUtils::FormatExpressionEx(CString sMatchExpression, CString sFormatExpression, CString sInputString, BOOL bPartialMatch,  CString &sErrorMessage, BOOL *bMatched)
{
	sErrorMessage = _T("");
	*bMatched = TRUE;
	boost::regex	e;
//	unsigned int				match_flag = bPartialMatch ? boost::match_partial : boost::match_default;
//	unsigned int				flags = match_flag | boost::format_perl;
	CString			retSt = sInputString;
	
	TCHAR szMatch[1024], szInput[1024], szFormat[1024];
	strcpy(szMatch, sMatchExpression);
	strcpy(szInput, sInputString);
	strcpy(szFormat, sFormatExpression);
	
	std::string in = szInput;

	try {
		e.assign(szMatch);
	}
	catch(std::exception& e)
    {
		sErrorMessage.Format("ERROR in regular expression %s", e.what());
		 *bMatched = FALSE;
         return retSt;
    }

	try {
		BOOL ret = boost::regex_match(in, e, bPartialMatch ? boost::match_partial|boost::match_default : boost::match_default);
		*bMatched = ret;
		if (ret == TRUE) {
			std::string r = boost::regex_merge(in, e, szFormat, boost::format_perl | (bPartialMatch ? boost::match_partial|boost::match_default : boost::match_default));
			retSt = r.data();
		}
	}
	catch(std::exception& e)
    {
		sErrorMessage.Format("ERROR in regular expression %s", e.what());
		  *bMatched = FALSE;
         return retSt;
    }

	return retSt;
}

int CUtils::FreeDiskSpaceMB(CString sPath)
{
	ULARGE_INTEGER	i64FreeBytesToCaller;
	ULARGE_INTEGER	i64TotalBytes;
	ULARGE_INTEGER	i64FreeBytes;
		
	BOOL fResult = GetDiskFreeSpaceEx (sPath,
					(PULARGE_INTEGER)&i64FreeBytesToCaller,
					(PULARGE_INTEGER)&i64TotalBytes,
					(PULARGE_INTEGER)&i64FreeBytes);

	if (fResult) {
//		double ft = i64TotalBytes.HighPart; 
//		ft *= MAXDWORD;
//		ft += i64TotalBytes.LowPart;

		double ff = i64FreeBytes.HighPart;
		ff *= MAXDWORD;
		ff += i64FreeBytes.LowPart;
			
		double fMBfree = ff/(1024*1024);
//		TCHAR sz[30];
//		sprintf(sz, "%d MB free",(DWORD)fMBfree);
//		double ratio = 100 - 100.0 * ff/ft;

		return (int)fMBfree;
	}
	
	return -1;

}

CTime g_lastMailFolderError(1975,1,1,0,0,0);
CTime g_lastMailFileError(1975,1,1,0,0,0);
CTime g_lastMailDatabaseError(1975, 1, 1, 0, 0, 0);

BOOL CUtils::SendMail(int nMailType, CString sMessage)
{
	if (g_prefs.m_emailsmtpserver == "")
		return TRUE;

	CString sMailBody = _T("");
	CString sErrorType = _T("");
	CTimeSpan ts;
	CTime tNow = CTime::GetCurrentTime();

	if (nMailType == MAILTYPE_FILEERROR) {
		sErrorType = _T("File processing error");
		ts = tNow - g_lastMailFileError;
	}
	else if (nMailType == MAILTYPE_DBERROR) {
		sErrorType = _T("Database processing error");
		ts = tNow - g_lastMailDatabaseError;
	}
	else if (nMailType == MAILTYPE_PROOFINGERROR) {
		sErrorType = _T("File convert error");
		ts = tNow - g_lastMailDatabaseError;
	}
	else {
		sErrorType = _T("Folder access error");
		ts = tNow - g_lastMailFolderError;
	}

	sMailBody = sErrorType + _T(":\r\n") + sMessage;

	if (g_prefs.m_emailpreventflooding && ts.GetTotalMinutes() < g_prefs.m_emailpreventfloodingdelay)
		return TRUE;

	CEmailClient email;

	BOOL ret = SendMail(g_prefs.m_emailsubject + _T(" - ") + sErrorType, sMailBody);

	if (nMailType == MAILTYPE_FILEERROR)
		g_lastMailFileError = CTime::GetCurrentTime();
	else if (nMailType == MAILTYPE_DBERROR)
		g_lastMailDatabaseError = CTime::GetCurrentTime();
	else
		g_lastMailFolderError = CTime::GetCurrentTime();

	return ret;

}

BOOL CUtils::SendMail(CString sMailSubject, CString sMailBody)
{
	return 	SendMail(sMailSubject, sMailBody, _T(""));
}


BOOL CUtils::SendMail(CString sMailSubject, CString sMailBody, CString sAttachMent)
{
	CEmailClient email;

	email.m_SMTPuseSSL = g_prefs.m_mailusessl;
	return email.SendMailAttachment(g_prefs.m_emailsmtpserver, g_prefs.m_mailsmtpport, g_prefs.m_mailsmtpserverusername, g_prefs.m_mailsmtpserverpassword,
		g_prefs.m_emailfrom,
		g_prefs.m_emailto, g_prefs.m_emailcc, sMailSubject, sMailBody, false, sAttachMent);
}





CString CUtils::GetLastWin32Error()
{
    TCHAR szBuf[4096]; 
    LPVOID lpMsgBuf;
    DWORD dw = GetLastError(); 

    FormatMessage(	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
					NULL,
					dw,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
					(LPTSTR) &lpMsgBuf,
					0, NULL );

    wsprintf(szBuf, "%s (errorcode %d)",lpMsgBuf, dw); 
 
	CString s(szBuf);
    LocalFree(lpMsgBuf);
 
	return s;
}

CString CUtils::CurrentTime2String()
{
	CString s;
	CTime tDate(CTime::GetCurrentTime());
	s.Format("%.2d-%.2d-%.4d %.2d:%.2d:%.2d", tDate.GetDay(), tDate.GetMonth(), tDate.GetYear(), tDate.GetHour(), tDate.GetMinute(), tDate.GetSecond());

	return s;
}

CTime CUtils::GetNextDay(CTime dt)
{
	CTimeSpan ts = CTimeSpan(1, 0, 0, 0);
	dt += ts;
	return dt;
}

CTime CUtils::ParseDate(CString sDate, CString sDateFormat)
{
	int y = 0;
	int m = 0;
	int d = 0;
	if (sDateFormat == "YYMMDD" && sDate.GetLength() == 6)
	{
		y = 2000 + atoi(sDate.Mid(0, 2));
		m = atoi(sDate.Mid(2, 2));
		d = atoi(sDate.Mid(4, 2));
	}
	else if (sDateFormat == "YYYYMMDD" && sDate.GetLength() == 8)
	{
		y = atoi(sDate.Mid(0, 4));
		m = atoi(sDate.Mid(4, 2));
		d = atoi(sDate.Mid(5, 2));
	}
	else if (sDateFormat == "DDMMYY" && sDate.GetLength() == 6)
	{
		d = atoi(sDate.Mid(0, 2));
		m = atoi(sDate.Mid(2, 2));
		y = 2000 + atoi(sDate.Mid(4, 2));
	}
	else if (sDateFormat == "DDMMYYYY" && sDate.GetLength() == 8)
	{
		d = atoi(sDate.Mid(0, 2));
		m = atoi(sDate.Mid(2, 2));
		y = atoi(sDate.Mid(4, 4));
	}
	if (y > 0 && m > 0 && d > 0)
		return CTime(y, m, d, 0, 0, 0);

	return CTime(1975, 1, 1, 0, 0, 0);
}


CString CUtils::Date2String(CTime tDate, CString sDateFormat, BOOL bAllowSeparators, int nWeekNumber)
{
	COleDateTime dtt(tDate.GetYear(), tDate.GetMonth(), tDate.GetDay(), tDate.GetHour(), tDate.GetMinute(), tDate.GetSecond());

	CTimeSpan ts1day = CTimeSpan(1, 0, 0, 0);
	CString sDate = "";
	CString s;

	CTime tDate1 = tDate + ts1day;

	if (sDateFormat == "DD1") {
		s.Format("%.2d", tDate1.GetDay());
		return s;
	}

	if (sDateFormat == "DD1MM") {
		s.Format("%.2d%.2d", tDate1.GetDay(), tDate1.GetMonth());
		return s;
	}
	if (sDateFormat == "DD1MMYY") {
		s.Format("%.2d%.2d%.2d", tDate1.GetDay(), tDate1.GetMonth(), tDate1.GetYear() - 2000);
		return s;
	}
	if (sDateFormat == "DD1MMYYYY") {
		s.Format("%.2d%.2d%.4d", tDate1.GetDay(), tDate1.GetMonth(), tDate1.GetYear());
		return s;
	}
	if (sDateFormat == "DDMMYY") {
		s.Format("%.2d%.2d%.2d", tDate.GetDay(), tDate.GetMonth(), tDate.GetYear() - 2000);
		return s;
	}
	if (sDateFormat == "DDMMYYYY") {
		s.Format("%.2d%.2d%.4d", tDate.GetDay(), tDate.GetMonth(), tDate.GetYear());
		return s;
	}
	if (sDateFormat == "YYMMDD") {
		s.Format("%.2d%.2d%.2d", tDate.GetYear() - 2000, tDate.GetMonth(), tDate.GetDay());
		return s;
	}
	if (sDateFormat == "YYYYMMDD") {
		s.Format("%.4d%.2d%.2d", tDate.GetYear(), tDate.GetMonth(), tDate.GetDay());
		return s;
	}


	int h = 0;
	while (h < sDateFormat.GetLength()) {
		int hbefore = h;

		if (sDateFormat.Find("J") == h && sDateFormat.Find("JJJ") == -1) {


			s.Format("%d", dtt.GetDayOfYear());
			sDate += s;
			h += s.GetLength();
		}

		if (sDateFormat.Find("JJJ") == h && sDateFormat.Find("JJJJ") == -1) {
			s.Format("%.3d", dtt.GetDayOfYear());
			sDate += s;
			h += s.GetLength();
		}
		if (sDateFormat.Find("JJJJ") == h) {
			s.Format("%.4d", dtt.GetDayOfYear());
			sDate += s;
			h += s.GetLength();
		}
		if (sDateFormat.Find("HH") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetHour());
			sDate += s;
		}
		if (sDateFormat.Find("hh") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetHour());
			sDate += s;
		}
		if (sDateFormat.Find("mm") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetMinute());
			sDate += s;
		}
		if (sDateFormat.Find("ss") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetSecond());
			sDate += s;
		}
		if (sDateFormat.Find("SS") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetSecond());
			sDate += s;
		}

		if (sDateFormat.Find("DD") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetDay());
			sDate += s;
		}
		if (sDateFormat.Find("MM") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetMonth());
			sDate += s;
		}
		if (sDateFormat.Find("WW") == h) {
			h += 2;
			s.Format("%.2d", nWeekNumber);
			if (nWeekNumber > 0)
				sDate += s;
		}

		if (sDateFormat.Find("YY") == h && sDateFormat.Find("YYYY") == -1) {
			h += 2;
			s.Format("%.2d", tDate.GetYear() - 2000);
			sDate += s;
		}
		if (sDateFormat.Find("YYYY") == h) {
			h += 4;
			s.Format("%.4d", tDate.GetYear());
			sDate += s;
		}
		if (h == hbefore) {
			if (bAllowSeparators) {
				sDate += sDateFormat[h];
			}
			h++;
		}
	}
	return sDate;
}



CString CUtils::Time2String(CTime tm)
{
	CString s;
	s.Format("%.4d-%.2d-%.2d %.2d:%.2d:%.2d", tm.GetYear(), tm.GetMonth(), tm.GetDay(), tm.GetHour(), tm.GetMinute(), tm.GetSecond());

	return s;
}

CString CUtils::Time2String(CTime tDate, CString sDateFormat)
{
	////////////////////////////
	// Form output date format
	////////////////////////////

	CString sDate = "";
	CString s;
	COleDateTime dtt(tDate.GetYear(), tDate.GetMonth(), tDate.GetDay(), tDate.GetHour(), tDate.GetMinute(), tDate.GetSecond());

	int h = 0;
	while (h < sDateFormat.GetLength()) {
		int hbefore = h;

		if (sDateFormat.Find("J") == h && sDateFormat.Find("JJJ") == -1) {
			int nDayNumber = dtt.GetDayOfYear();

			s.Format("%d", dtt.GetDayOfYear());
			sDate += s;
			h += s.GetLength();
		}

		if (sDateFormat.Find("JJJ") == h && sDateFormat.Find("JJJJ") == -1) {
			s.Format("%.3d", dtt.GetDayOfYear());
			sDate += s;
			h += s.GetLength();
		}
		if (sDateFormat.Find("JJJJ") == h) {
			s.Format("%.4d", dtt.GetDayOfYear());
			sDate += s;
			h += s.GetLength();
		}

		if (sDateFormat.Find("DD") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetDay());
			sDate += s;
		}
		if (sDateFormat.Find("MM") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetMonth());
			sDate += s;
		}
		if (sDateFormat.Find("YY") == h && sDateFormat.Find("YYYY") == -1) {
			h += 2;
			s.Format("%.2d", tDate.GetYear() - 2000);
			sDate += s;
		}
		if (sDateFormat.Find("YYYY") == h) {
			h += 4;
			s.Format("%.4d", tDate.GetYear());
			sDate += s;
		}

		if (sDateFormat.Find("hh") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetHour());
			sDate += s;
		}

		if (sDateFormat.Find("mm") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetMinute());
			sDate += s;
		}

		if (sDateFormat.Find("ss") == h) {
			h += 2;
			s.Format("%.2d", tDate.GetSecond());
			sDate += s;
		}

		if (h == hbefore) {
			sDate += sDateFormat[h];
			h++;
		}
	}

	return sDate;
}

BOOL CUtils::SaveFile(CString sFileName, CString sContent)
{
	DWORD	nBytesWritten;

	HANDLE hFile = ::CreateFile(sFileName, GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return FALSE;

	BOOL ok = ::WriteFile(hFile, sContent.GetBuffer(255), sContent.GetLength(), &nBytesWritten, NULL);
	sContent.ReleaseBuffer();
	::CloseHandle(hFile);
	if (ok == FALSE) {
		::DeleteFile(sFileName);
		return FALSE;
	}
	return TRUE;
}

void CUtils::AlivePoll()
{
	TCHAR	szFinalLine[100];
	DWORD	nBytesWritten;
	SYSTEMTIME	ltime;
	HANDLE hFile;

	hFile = ::CreateFile(g_prefs.m_alivelogFilePath + _T("\\FileDistributorExport.log"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	::GetLocalTime(&ltime);
	_stprintf(szFinalLine, "%.2d-%.2d %.2d:%.2d:%.2d.%.3d\r\n", (int)ltime.wDay, (int)ltime.wMonth, (int)ltime.wHour, (int)ltime.wMinute, (int)ltime.wSecond, (int)ltime.wMilliseconds);
	::WriteFile(hFile, szFinalLine, (DWORD)_tcsclen(szFinalLine), &nBytesWritten, NULL);
	::CloseHandle(hFile);
}


void CUtils::AliveTX()
{
	TCHAR	szFinalLine[100];
	DWORD	nBytesWritten;
	SYSTEMTIME	ltime;
	HANDLE hFile;

	hFile = ::CreateFile(g_prefs.m_alivelogFilePath + _T("\\FileDistributorTX.log"), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	::GetLocalTime(&ltime);
	_stprintf(szFinalLine, "%.2d-%.2d %.2d:%.2d:%.2d.%.3d\r\n", (int)ltime.wDay, (int)ltime.wMonth, (int)ltime.wHour, (int)ltime.wMinute, (int)ltime.wSecond, (int)ltime.wMilliseconds);
	::WriteFile(hFile, szFinalLine, (DWORD)_tcsclen(szFinalLine), &nBytesWritten, NULL);
	::CloseHandle(hFile);
}

CString CUtils::EncodeBlowFish(CString sInput)
{
	CkCrypt2 crypt;

	if (crypt.UnlockComponent("INFRAL.CB1092019_4czEsHZS615x") == false)
		return sInput;

	//  Attention: use "blowfish2" for the algorithm name:
	crypt.put_CryptAlgorithm("blowfish2");

	//  CipherMode may be "ecb", "cbc", or "cfb"
	crypt.put_CipherMode("cbc");

	//  KeyLength (in bits) may be a number between 32 and 448.
	//  128-bits is usually sufficient.  The KeyLength must be a
	//  multiple of 8.
	crypt.put_KeyLength(128);

	//  The padding scheme determines the contents of the bytes
	//  that are added to pad the result to a multiple of the
	//  encryption algorithm's block size.  Blowfish has a block
	//  size of 8 bytes, so encrypted output is always
	//  a multiple of 8.
	crypt.put_PaddingScheme(0);

	//  EncodingMode specifies the encoding of the output for
	//  encryption, and the input for decryption.
	//  It may be "hex", "url", "base64", or "quoted-printable".
	crypt.put_EncodingMode("hex");

	//  An initialization vector is required if using CBC or CFB modes.
	//  ECB mode does not use an IV.
	//  The length of the IV is equal to the algorithm's block size.
	//  It is NOT equal to the length of the key.
	const char *ivHex = "0001020304050607";
	crypt.SetEncodedIV(ivHex, "hex");

	//  The secret key must equal the size of the key.  For
	//  256-bit encryption, the binary secret key is 32 bytes.
	//  For 128-bit encryption, the binary secret key is 16 bytes.
	const char *keyHex = "000102030405060708090A0B0C0D0E0F";
	crypt.SetEncodedKey(keyHex, "hex");

	const char *encStr = crypt.encryptStringENC(sInput);
	CString sOutput(encStr);

	return sOutput;
}


CString CUtils::DecodeBlowFish(CString sInput)
{
	CkCrypt2 crypt;

	if (crypt.UnlockComponent("INFRAL.CB1092019_4czEsHZS615x") == false)
		return sInput;

	//  Attention: use "blowfish2" for the algorithm name:
	crypt.put_CryptAlgorithm("blowfish2");

	//  CipherMode may be "ecb", "cbc", or "cfb"
	crypt.put_CipherMode("cbc");

	//  KeyLength (in bits) may be a number between 32 and 448.
	//  128-bits is usually sufficient.  The KeyLength must be a
	//  multiple of 8.
	crypt.put_KeyLength(128);

	//  The padding scheme determines the contents of the bytes
	//  that are added to pad the result to a multiple of the
	//  encryption algorithm's block size.  Blowfish has a block
	//  size of 8 bytes, so encrypted output is always
	//  a multiple of 8.
	crypt.put_PaddingScheme(0);

	//  EncodingMode specifies the encoding of the output for
	//  encryption, and the input for decryption.
	//  It may be "hex", "url", "base64", or "quoted-printable".
	crypt.put_EncodingMode("hex");

	//  An initialization vector is required if using CBC or CFB modes.
	//  ECB mode does not use an IV.
	//  The length of the IV is equal to the algorithm's block size.
	//  It is NOT equal to the length of the key.
	const char *ivHex = "0001020304050607";
	crypt.SetEncodedIV(ivHex, "hex");

	//  The secret key must equal the size of the key.  For
	//  256-bit encryption, the binary secret key is 32 bytes.
	//  For 128-bit encryption, the binary secret key is 16 bytes.
	const char *keyHex = "000102030405060708090A0B0C0D0E0F";
	crypt.SetEncodedKey(keyHex, "hex");

	const char *decStr = crypt.decryptStringENC(sInput);
	CString sOutput(decStr);

	return sOutput;
}


CString CUtils::GetTempFolder()
{
	TCHAR lpTempPathBuffer[MAX_PATH];

	if (GetTempPath(MAX_PATH, lpTempPathBuffer) == 0)
		return  _T("c:\\temp");
	CString s(lpTempPathBuffer);

	return s;
}

BOOL CUtils::Load(CString  csFileName, CString &csDoc)
{

	CFile file;
	if (!file.Open(csFileName, CFile::modeRead))
		return FALSE;
	int nLength = (int)file.GetLength();

#if defined(_UNICODE)
	// Allocate Buffer for UTF-8 file data
	unsigned char* pBuffer = new unsigned char[nLength + 1];
	nLength = file.Read(pBuffer, nLength);
	pBuffer[nLength] = '\0';

	// Convert file from UTF-8 to Windows UNICODE (AKA UCS-2)
	int nWideLength = MultiByteToWideChar(CP_UTF8, 0, (const char*)pBuffer, nLength, NULL, 0);
	nLength = MultiByteToWideChar(CP_UTF8, 0, (const char*)pBuffer, nLength,
		csDoc.GetBuffer(nWideLength), nWideLength);
	ASSERT(nLength == nWideLength);
	delete[] pBuffer;
#else
	nLength = file.Read(csDoc.GetBuffer(nLength), nLength);
#endif
	csDoc.ReleaseBuffer(nLength);
	file.Close();

	return TRUE;
}

BOOL CUtils::Save(CString  csFileName, CString csDoc)
{
	int nLength = csDoc.GetLength();
	CFile file;
	if (!file.Open(csFileName, CFile::modeWrite | CFile::modeCreate))
		return FALSE;
#if defined( _UNICODE )
	int nUTF8Len = WideCharToMultiByte(CP_UTF8, 0, csDoc, nLength, NULL, 0, NULL, NULL);
	char* pBuffer = new char[nUTF8Len + 1];
	nLength = WideCharToMultiByte(CP_UTF8, 0, csDoc, nLength, pBuffer, nUTF8Len + 1, NULL, NULL);
	file.Write(pBuffer, nLength);
	delete pBuffer;
#else
	file.Write((LPCTSTR)csDoc, nLength);
#endif
	file.Close();
	return TRUE;
}




DWORD CUtils::GenerateGUID(int nSpooler, int nTick)
{
	CTime tNow = CTime::GetCurrentTime();
	return nSpooler + 100 * tNow.GetSecond() + 10000 * tNow.GetMinute() + 1000000 * tNow.GetHour() + 100000000 * nTick;
}






BYTE g_CRCbufPoll[8192];

int CUtils::CRC32Poll(CString sFileName)
{
	HANDLE	Hndl;
	//BYTE	buf[8192];
	DWORD	nBytesRead;

	if ((Hndl = ::CreateFile(sFileName, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE)
		return 0;

	DWORD sum = 0;
	do {
		if (!::ReadFile(Hndl, g_CRCbufPoll, 8092, &nBytesRead, NULL)) {
			::CloseHandle(Hndl);
			return 0;
		}
		if (nBytesRead > 0) {
			for (int i = 0; i < nBytesRead; i++) {
				sum += g_CRCbufPoll[i];
				// just let it overflow..
			}
		}
	} while (nBytesRead > 0);

	::CloseHandle(Hndl);

	int nCRC = sum;
	return nCRC;
}

int CUtils::CRC32Convert(CString sFileName)
{
	HANDLE	Hndl;
	DWORD	nBytesRead;
	int nRetries = 5;
	BOOL bSuccess = FALSE;
	DWORD sum = 0;

	while (--nRetries >= 0 && bSuccess == FALSE) {

		if ((Hndl = ::CreateFile(sFileName, GENERIC_READ, FILE_SHARE_WRITE | FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0)) == INVALID_HANDLE_VALUE) {

			LogprintfConvert("ERROR: CRC32Convert could not open file %s", sFileName);
			::Sleep(1000);
			continue;
		}


		BOOL bGotError = FALSE;
		do {
			if (!::ReadFile(Hndl, g_CRCbufPoll, 8092, &nBytesRead, NULL)) {
				::CloseHandle(Hndl);
				LogprintfConvert("ERROR: CRC32Convert could not read from file %s", sFileName);
				::Sleep(1000);
				bGotError = TRUE;
				break;
			}
			if (nBytesRead > 0) {
				for (int i = 0; i < nBytesRead; i++) {
					sum += g_CRCbufPoll[i]; // just let it overflow..
				}
			}
		} while (nBytesRead > 0);

		if (bGotError == FALSE)
			bSuccess = TRUE;

		::CloseHandle(Hndl);
	}

	int nCRC = sum;
	return nCRC;
}



// Function name   : GetOwner
// Description     : Determines the 'Owner' of a given file or folder.
// Return type     : UINT is S_OK if successful; A Win32 'ERROR_' value otherwise.
// Argument        : LPCWSTR szFileOrFolderPathName is the fully qualified path of the file or folder to examine.
// Argument        : LPWSTR pUserNameBuffer is a pointer to a buffer used to contain the resulting 'Owner' string.
// Argument        : int nSizeInBytes is the size of the buffer.
UINT CUtils::GetOwner(LPCSTR szFileOrFolderPathName, LPSTR pUserNameBuffer, int nSizeInBytes)
{
	// 1) Validate the path:
	// 1.1) Length should not be 0.
	// 1.2) Path must point to an existing file or folder.
	if (!lstrlen(szFileOrFolderPathName) || !PathFileExists(szFileOrFolderPathName))
		return ERROR_INVALID_PARAMETER;

	// 2) Validate the buffer:
	// 2.1) Size must not be 0.
	// 2.2) Pointer must not be NULL.
	if (nSizeInBytes <= 0 || pUserNameBuffer == NULL)
		return ERROR_INVALID_PARAMETER;

	// 3) Convert the path to UNC if it is not already UNC so that we can extract a machine name from it:
	// 3.1) Use a big buffer... some OS's can have a path that is 32768 chars in length.
	TCHAR szUNCPathName[32767] = { 0 };
	// 3.2) If path is not UNC...
	if (!PathIsUNC(szFileOrFolderPathName)) {
		// 3.3) Mask the big buffer into a UNIVERSAL_NAME_INFO.
		DWORD dwUniSize = 32767 * sizeof(TCHAR);
		UNIVERSAL_NAME_INFO* pUNI = (UNIVERSAL_NAME_INFO*)szUNCPathName;
		// 3.4) Attempt to get the UNC version of the path into the big buffer.
		if (!WNetGetUniversalName(szFileOrFolderPathName, UNIVERSAL_NAME_INFO_LEVEL, pUNI, &dwUniSize)) {
			// 3.5) If successful, copy the UNC version into the buffer.
			lstrcpy(szUNCPathName, pUNI->lpUniversalName); // You must extract from this offset as the buffer has UNI overhead.
		}
		else {
			// 3.6) If not successful, copy the original path into the buffer.
			lstrcpy(szUNCPathName, szFileOrFolderPathName);
		}
	}
	else {
		// 3.7) Path is already UNC, copy the original path into the buffer.
		lstrcpy(szUNCPathName, szFileOrFolderPathName);
	}

	// 4) If path is UNC (will not be the case for local physical drive paths) we want to extract the machine name:
	// 4.1) Use a buffer bug enough to hold a machine name per Win32.
	TCHAR szMachineName[MAX_COMPUTERNAME_LENGTH + 1] = { 0 };
	// 4.2) If path is UNC...
	if (PathIsUNC(szUNCPathName)) {
		// 4.3) Use PathFindNextComponent() to skip past the double backslashes.
		LPSTR lpMachineName = PathFindNextComponent(szUNCPathName);
		// 4.4) Walk the the rest of the path to find the end of the machine name.
		int nPos = 0;
		LPSTR lpNextSlash = lpMachineName;
		while ((lpNextSlash[0] != L'\\') && (lpNextSlash[0] != L'\0')) {
			nPos++;
			lpNextSlash++;
		}
		// 4.5) Copyt the machine name into the buffer.
		lstrcpyn(szMachineName, lpMachineName, nPos + 1);
	}

	// 5) Derive the 'Owner' by getting the owner's Security ID from a Security Descriptor associated with the file or folder indicated in the path.
	// 5.1) Get a security descriptor for the file or folder that contains the Owner Security Information.
	// 5.1.1) Use GetFileSecurity() with some null params to get the required buffer size.
	// 5.1.2) We don't really care about the return value.
	// 5.1.3) The error code must be ERROR_INSUFFICIENT_BUFFER for use to continue.
	unsigned long   uSizeNeeded = 0;
	GetFileSecurity(szUNCPathName, OWNER_SECURITY_INFORMATION, 0, 0, &uSizeNeeded);
	UINT uRet = GetLastError();
	if (uRet == ERROR_INSUFFICIENT_BUFFER && uSizeNeeded) {
		uRet = S_OK; // Clear the ERROR_INSUFFICIENT_BUFFER

		// 5.2) Allocate the buffer for the Security Descriptor, check for out of memory
		LPBYTE lpSecurityBuffer = (LPBYTE)malloc(uSizeNeeded * sizeof(BYTE));
		if (!lpSecurityBuffer) {
			return ERROR_NOT_ENOUGH_MEMORY;
		}

		// 5.2) Get the Security Descriptor that contains the Owner Security Information into the buffer, check for errors
		if (!GetFileSecurity(szUNCPathName, OWNER_SECURITY_INFORMATION, lpSecurityBuffer, uSizeNeeded, &uSizeNeeded)) {
			free(lpSecurityBuffer);
			return GetLastError();
		}

		// 5.3) Get the the owner's Security ID (SID) from the Security Descriptor, check for errors
		PSID pSID = NULL;
		BOOL bOwnerDefaulted = FALSE;
		if (!GetSecurityDescriptorOwner(lpSecurityBuffer, &pSID, &bOwnerDefaulted)) {
			free(lpSecurityBuffer);
			return GetLastError();
		}

		// 5.4) Get the size of the buffers needed for the owner information (domain and name)
		// 5.4.1) Use LookupAccountSid() with buffer sizes set to zero to get the required buffer sizes.
		// 5.4.2) We don't really care about the return value.
		// 5.4.3) The error code must be ERROR_INSUFFICIENT_BUFFER for use to continue.
		LPSTR			pName = NULL;
		LPSTR			pDomain = NULL;
		unsigned long   uNameLen = 0;
		unsigned long   uDomainLen = 0;
		SID_NAME_USE    sidNameUse = SidTypeUser;
		LookupAccountSid(szMachineName, pSID, pName, &uNameLen, pDomain, &uDomainLen, &sidNameUse);
		uRet = GetLastError();
		if ((uRet == ERROR_INSUFFICIENT_BUFFER) && uNameLen && uDomainLen) {
			uRet = S_OK; // Clear the ERROR_INSUFFICIENT_BUFFER

			// 5.5) Allocate the required buffers, check for out of memory
			pName = (LPSTR)malloc(uNameLen * sizeof(TCHAR));
			pDomain = (LPSTR)malloc(uDomainLen * sizeof(TCHAR));
			if (!pName || !pDomain) {
				free(lpSecurityBuffer);
				return ERROR_NOT_ENOUGH_MEMORY;
			}

			// 5.6) Get domain and username
			if (!LookupAccountSid(szMachineName, pSID, pName, &uNameLen, pDomain, &uDomainLen, &sidNameUse)) {
				free(pName);
				free(pDomain);
				free(lpSecurityBuffer);
				return GetLastError();
			}

			// 5.7) Build the owner string from the domain and username if there is enough room in the buffer.
			if (nSizeInBytes > ((uNameLen + uDomainLen + 2) * sizeof(TCHAR))) {
				lstrcpy(pUserNameBuffer, pDomain);
				//	lstrcat(pUserNameBuffer, L"\\");
				lstrcat(pUserNameBuffer, _T("\\"));
				lstrcat(pUserNameBuffer, pName);
			}
			else {
				uRet = ERROR_INSUFFICIENT_BUFFER;
			}

			// 5.8) Release memory
			free(pName);
			free(pDomain);
		}
		// 5.9) Release memory
		free(lpSecurityBuffer);
	}
	return uRet;
}


CString CUtils::GetProcessOwner(CString sFileOrFolderPathName)
{
	CString sBuf = "";
	CString sCmd;
	DWORD dwBytesWritten;
	CString sTxtFile = GetTempFolder() + _T("\\processinfo.txt");

	sCmd.Format("\"%s\\Handle.exe\" -a \"%s\" > \"%s\"", g_prefs.m_apppath, sFileOrFolderPathName, sTxtFile);

	HANDLE hFile = ::CreateFile(GetTempFolder() + _T("\\gethandle.bat"),
		GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return "";

	::WriteFile(hFile, sCmd, sCmd.GetLength(), &dwBytesWritten, NULL);
	::CloseHandle(hFile);

	DoCreateProcess(GetTempFolder() + _T("\\gethandle.bat"), 120);

	for (int i = 0; i < 500; i++) {
		::Sleep(100);
		if (FileExist(sTxtFile) && GetFileSize(sTxtFile) > 0)
			break;
	}

	Load(sTxtFile, sBuf);

	return sBuf;
}

CString CUtils::GetFilePID(CString sFileOrFolderPathName)
{
	CString sBuf = "";
	CString sCmd;
	DWORD dwBytesWritten;

	CString sTxtFile = GetTempFolder() + _T("\\processinfo.txt");
	::DeleteFile(sTxtFile);
	sCmd.Format("\"%s\\Handle.exe\" \"%s\" > \"%s\"" , g_prefs.m_apppath, sFileOrFolderPathName, sTxtFile);

	HANDLE hFile = CreateFile(GetTempFolder() + _T("\\gethandle.bat"),
		GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
		return "";

	WriteFile(hFile, sCmd, sCmd.GetLength(), &dwBytesWritten, NULL);
	CloseHandle(hFile);


	DoCreateProcess(GetTempFolder() + _T("\\gethandle.bat"), 120);

	for (int i = 0; i < 500; i++) {
		Sleep(100);
		if (FileExist(sTxtFile) && GetFileSize(sTxtFile) > 0)
			break;
	}

	Load(sTxtFile, sBuf);
	return sBuf;
}

void CUtils::KillProcess(DWORD dwProcessID)
{
	CString sKillCommand;
	if (dwProcessID > 0)
		sKillCommand.Format("taskkill /f /im %d", dwProcessID);
	else
		sKillCommand.Format("taskkill /f /im \"%s\"", GetFileName(g_prefs.m_callascommandfilepath));

	DoCreateProcess(sKillCommand, 120);
	::Sleep(10 * 1000);
}

BOOL CUtils::ForceHandlesClose(CString sFilename)
{
	DWORD dwBytesWritten;

	if (FileExist(sFilename) == FALSE)
		return TRUE;

	CString sFileNameLocalDrive;

	/*if (g_prefs.m_ccdatalocaldriveletter != "" && sFilename[0] == '\\' && sFilename[1] == '\\') {
		sFilename = sFilename.Mid(2);
		int n = sFilename.Find("\\");
		if (n != -1)
			sFilename = sFilename.Mid(n);
		sFilename = g_prefs.m_ccdatalocaldriveletter + sFilename;
	}*/
	

	// Attempt 1 - directly at child process (may fail)
	CString sCmd;
	sCmd.Format("openfiles.exe /disconnect /a * /OP \"%s\"", sFilename);
	DoCreateProcess(sCmd, 60);

	// Attempt 2 - via batch file
	::DeleteFile("c:\\temp\\ForceClose.bat");
	HANDLE hFile = ::CreateFile(_T("c:\\temp\\ForceClose.bat"),
		GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE) {
		::WriteFile(hFile, sCmd, sCmd.GetLength(), &dwBytesWritten, NULL);
		::CloseHandle(hFile);
		DoCreateProcess("c:\\temp\\ForceClose.bat", 10);
	}

	// Attempt 3 - use remote server name
	CString sServer = GetFilePath(g_prefs.m_hiresPDFPath);
	if (sServer[0] == '\\')
		sServer == sServer.Mid(1);
	if (sServer[0] == '\\')
		sServer == sServer.Mid(1);

	int n = sServer.Find("\\");
	if (n != -1)
		sServer = sServer.Left(n);

	sCmd.Format("openfiles.exe /disconnect /s \"%s\" /a * /OP \"%s\"", sServer, sFilename);
	DoCreateProcess(sCmd, 10);

	::DeleteFile("c:\\temp\\ForceClose2.bat");
	hFile = CreateFile(_T("c:\\temp\\ForceClose2.bat"),
		GENERIC_WRITE, FILE_SHARE_READ,
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile != INVALID_HANDLE_VALUE) {
		WriteFile(hFile, sCmd, sCmd.GetLength(), &dwBytesWritten, NULL);
		CloseHandle(hFile);
		DoCreateProcess("c:\\temp\\ForceClose2.bat", 60);
	}

	CString sPID = GetFilePID(sFilename);
	if (sPID != "") {
		CString pid = "";
		int m = sPID.Find("pid: ");
		if (m != -1) {
			pid = sPID.Mid(m + 5, 6);
			pid.Trim();
		}
		CString id = "";
		m = sPID.Find("type: File");
		if (m != -1) {
			id = sPID.Mid(m + 11);
			id.Trim();
			int m2 = id.Find(":");
			if (m2 != -1)
				id = id.Left(m2);
		}
		if (id != "" && pid != "") {
			sCmd.Format("\"%s\\handle.exe\" -c %s -y -p %s", g_prefs.m_apppath, id, pid);
			LogprintfResample("INFO:  %s", sCmd);
			::DeleteFile("c:\\temp\\ForceClose3.bat");
			hFile = CreateFile(_T("c:\\temp\\ForceClose3.bat"),
				GENERIC_WRITE, FILE_SHARE_READ,
				NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

			if (hFile != INVALID_HANDLE_VALUE) {
				WriteFile(hFile, sCmd, sCmd.GetLength(), &dwBytesWritten, NULL);
				CloseHandle(hFile);
				DoCreateProcess("c:\\temp\\ForceClose3.bat", 60);
			}
		}
	}

	LogprintfResample("INFO:  ForceHandlesClose called on file %s", sFilename);

	return TRUE;
}

int CUtils::CopyFiles(CString sSpoolFolder, CString sSearchMask, CString sDestFolder)
{
	WIN32_FIND_DATA	fdata;
	HANDLE			fHandle;
	TCHAR			NameSearch[MAX_PATH], FoundFile[MAX_PATH], DestFoundFile[MAX_PATH];
	int nFiles = 0;

	if (CheckFolder(sSpoolFolder) == FALSE)
		return -1;

	if (sSpoolFolder.Right(1) != "\\")
		sSpoolFolder += _T("\\");
	if (sDestFolder.Right(1) != "\\")
		sDestFolder += _T("\\");


	_stprintf(NameSearch, "%s%s", (LPCTSTR)sSpoolFolder, (LPCTSTR)sSearchMask);
	fHandle = ::FindFirstFile(NameSearch, &fdata);
	if (fHandle == INVALID_HANDLE_VALUE) {
		// All empty
		return 0;
	}

	// Got something
	do {
		if (!(fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			_stprintf(FoundFile, "%s%s", (LPCTSTR)sSpoolFolder, fdata.cFileName);
			HANDLE Hndl = CreateFile(FoundFile, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, 0);
			BOOL ok = FALSE;
			if (Hndl != INVALID_HANDLE_VALUE) {
				CloseHandle(Hndl);
				ok = TRUE;

			}
			if (ok) {
				_stprintf(DestFoundFile, "%s%s", (LPCTSTR)sDestFolder, fdata.cFileName);
				CopyFile(FoundFile, DestFoundFile, FALSE);
				nFiles++;
			}
		}

		if (!::FindNextFile(fHandle, &fdata))
			break;

	} while (TRUE);

	::FindClose(fHandle);

	return nFiles;
}

void CUtils::DeleteOldWebFiles(int nMasterCopySeparationSet, CString sFileName, BOOL bGenerateForFlash) 
{
	CString sFileToDelete;
	CString sWebRoot = g_prefs.m_webPath;

	if (sWebRoot.Right(1) != "/" || sWebRoot.Right(1) != "\\")
		sWebRoot += _T("/");

	Reconnect(g_prefs.m_webPath, g_prefs.m_usecurrentuserweb ? g_prefs.m_webFTPuser : _T(""), g_prefs.m_usecurrentuserweb ? g_prefs.m_webFTPpassword : _T(""));

	sFileToDelete.Format(_T("%sCCpreviews\\%d.jpg"), sWebRoot, nMasterCopySeparationSet);
	::DeleteFile(sFileToDelete);
	sFileToDelete.Format(_T("%sCCthumbnails\\%d.jpg"), sWebRoot, nMasterCopySeparationSet);
	::DeleteFile(sFileToDelete);
	if (bGenerateForFlash) {
		sFileToDelete.Format(_T("%sCCpreviews\\%d\\TileGroup0"), sWebRoot, nMasterCopySeparationSet);
		DeleteFiles(sFileToDelete, "*.*");
		_rmdir(sFileToDelete.GetBuffer(256));
		sFileToDelete.ReleaseBuffer();
		sFileToDelete.Format(_T("%sCCpreviews\\%d\\ImageProperties.xml"), sWebRoot, nMasterCopySeparationSet);
		::DeleteFile(sFileToDelete);
		sFileToDelete.Format(_T("%sCCpreviews\\%d"), sWebRoot, nMasterCopySeparationSet);
		DeleteFiles(sFileToDelete, "*.*");
		_rmdir(sFileToDelete.GetBuffer(256));
		sFileToDelete.ReleaseBuffer();

		sFileToDelete.Format(_T("%sCCpreviews\\%d-*\\TileGroup0"), sWebRoot, nMasterCopySeparationSet);
		DeleteFiles(sFileToDelete, "*.*");
		_rmdir(sFileToDelete.GetBuffer(256));
		sFileToDelete.ReleaseBuffer();
		sFileToDelete.Format(_T("%sCCpreviews\\%d-*\\ImageProperties.xml"), sWebRoot, nMasterCopySeparationSet);
		::DeleteFile(sFileToDelete);
		sFileToDelete.Format(_T("%sCCpreviews\\%d-*"), sWebRoot, nMasterCopySeparationSet);
		DeleteFiles(sFileToDelete, "*.*");
		_rmdir(sFileToDelete.GetBuffer(256));
		sFileToDelete.ReleaseBuffer();
	}
}

int  CUtils::GetLogFileStatus(CString sFileName, CString &sErrorMessage)
{
	sErrorMessage = _T("");

	CString sBuffer;
	if (Load(sFileName, sBuffer) == FALSE)
		return -1;


	if (sBuffer.Find("DONE") != -1)
		return TRUE;

	int n = sBuffer.Find("ERROR:");
	if (n != -1) {
		int m = sBuffer.Find("\n", n);
		if (m != -1)
			sErrorMessage = sBuffer.Mid(n, m - n + 1);

		sErrorMessage.Replace("\n", "");
		sErrorMessage.Replace("\r", "");
		return FALSE;
	}

	if (sBuffer.Find("DONE") != -1)
		return TRUE;

	return -1;
}




int  CUtils::GetLogFileStatus(CString sFileName, CString &sErrorMessage, BOOL &bHasErrorSeverityHit, CString &sHitErrorMessage)
{
	sErrorMessage = _T("");
	sHitErrorMessage = _T("");

	bHasErrorSeverityHit = FALSE;

	CString sBuffer;
	if (Load(sFileName, sBuffer) == FALSE)
		return -1;

	int n = sBuffer.Find("Hit with severity \"Error\"");
	if (n != -1) {
		bHasErrorSeverityHit = TRUE;

		int m = sBuffer.Find("\n", n);
		if (m != -1)
			sHitErrorMessage = sBuffer.Mid(n, m - n + 1);

		sHitErrorMessage.Replace("\n", "");
		sHitErrorMessage.Replace("\r", "");
	}

	if (sBuffer.Find("DONE") != -1)
		return TRUE;

	n = sBuffer.Find("ERROR:");
	if (n != -1) {
		int m = sBuffer.Find("\n", n);
		if (m != -1)
			sErrorMessage = sBuffer.Mid(n, m - n + 1);

		sErrorMessage.Replace("\n", "");
		sErrorMessage.Replace("\r", "");
		return FALSE;
	}

	if (sBuffer.Find("DONE") != -1)
		return TRUE;

	return -1;
}


CString CUtils::GetModuleLoadPath()
{
	TCHAR ModName[_MAX_PATH];
	_tcscpy(ModName, _T(""));

	if (!::GetModuleFileName(NULL, ModName, sizeof(ModName)))
		return CString(_T(""));


	CString LoadPath(ModName);

	int Idx = LoadPath.ReverseFind(_T('\\'));

	if (Idx == -1)
		return CString(_T(""));

	//if (LoadPath.Find("exe") != -1) {
   //    return CString(_T(""));
	//}


	LoadPath = LoadPath.Mid(0, Idx);


	return LoadPath;
}

std::wstring CUtils::charToWstring(TCHAR *szStr)
{
	std::wstring wstr(szStr, szStr + strlen(szStr));

	return wstr;
}

std::wstring CUtils::charToWstring(CString sStr)
{
	TCHAR szStr[1024];
	strcpy(szStr, sStr);
	std::wstring wstr(szStr, szStr + strlen(szStr));

	return wstr;
}

CString  CUtils::WstringTochar(std::wstring wstr)
{
	//std::string str(wstr.begin(), wstr.end());
	std::string str(CW2A(wstr.c_str()));
	CString cs(str.c_str());
	return cs;
}

int CUtils::IssueMessage(CString sOrgFileName, int nStatus, CString sTargetFolder, int nTypeNumber, CString &sErrorMessage)
{
	TCHAR buf[4097];
	DWORD dwBytesRead;
	DWORD nBytesWritten;
	HANDLE hndl;

	if (sOrgFileName != _T("")) {
		int nn = sOrgFileName.Find("#");
		if (nn != -1)
			sOrgFileName = sOrgFileName.Left(nn) + GetExtension(sOrgFileName);
	}


	sErrorMessage = _T("");

	if (sTargetFolder == _T(""))
		return TRUE;

	BOOL bSuccessStatus = (nStatus == CONVERTSTATUSNUMBER_CONVERTED);
	BOOL bErrorStatus = (nStatus == CONVERTSTATUSNUMBER_CONVERTERROR);

	if (bSuccessStatus && (g_prefs.m_messageonsuccess == FALSE || FileExist(g_prefs.m_messagetemplatesuccess) == FALSE))
		return TRUE;
	if (bErrorStatus && (g_prefs.m_messageonerror == FALSE || FileExist(g_prefs.m_messagetemplateerror) == FALSE))
		return TRUE;

	LogprintfConvert("INFO: Preparing ack-file generation..");

	if (bSuccessStatus) {
		hndl = CreateFile(g_prefs.m_messagetemplatesuccess, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
		if (hndl == INVALID_HANDLE_VALUE) {
			sErrorMessage.Format("Message template file not found (%s)", g_prefs.m_messagetemplatesuccess);
			LogprintfConvert("ERROR: %s\r\n", sErrorMessage);
			return FALSE;
		}
		if (!ReadFile(hndl, buf, 4096, &dwBytesRead, NULL)) {
			CloseHandle(hndl);
			sErrorMessage.Format("Error reading message template file (%s)", g_prefs.m_messagetemplatesuccess);
			LogprintfConvert("ERROR: %s\r\n", sErrorMessage);
			return FALSE;
		}
		buf[dwBytesRead] = 0;
		CloseHandle(hndl);
	}
	if (bErrorStatus) {
		hndl = ::CreateFile(g_prefs.m_messagetemplateerror, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0);
		if (hndl == INVALID_HANDLE_VALUE) {
			sErrorMessage.Format("Message template file not found (%s)", g_prefs.m_messagetemplateerror);
			LogprintfConvert("ERROR: %s\r\n", sErrorMessage);
			return FALSE;
		}
		if (!ReadFile(hndl, buf, 4096, &dwBytesRead, NULL)) {
			CloseHandle(hndl);
			sErrorMessage.Format("Error reading message template file (%s)", g_prefs.m_messagetemplateerror);
			LogprintfConvert("ERROR: %s\r\n", sErrorMessage);
			return FALSE;
		}
		buf[dwBytesRead] = 0;
		CloseHandle(hndl);
	}

	CString sContent(buf);

	CString sNewContent = sContent;
	CString sTime; // 2019-10-10T10:25:01+2:00
	CTime tNow = CTime::GetCurrentTime();
	int nGmt = 2;

	sTime.Format("%.4d-%.2d-%.2dT%.2d:%.2d:%.2d+%d:00", tNow.GetYear(), tNow.GetMonth(), tNow.GetDay(), tNow.GetHour(), tNow.GetMinute(), tNow.GetSecond(), nGmt);

	CString sF = sOrgFileName;
	sF.Replace(".pdf", "");
	sF.Replace(".PDF", "");
	sNewContent.Replace("%f", sF);
	sNewContent.Replace("%t", sTime);
	sNewContent.Replace("%e", sTime);
	sNewContent.Replace("%n", Int2String(nTypeNumber));

	CString sFileName = GetFileName(sOrgFileName, TRUE) + _T("-") + Int2String(nTypeNumber) + ".xml";// util.FormName(g_prefs.m_messagefilenamemask, g_prefs.m_messagedateformat, g_prefs.m_messagedateformat2, Job, g_prefs.m_messageusealiases, 0);

	sFileName.Replace("/", "");
	sFileName.Replace(":", "");
	sFileName.Replace("*", "");
	sFileName.Replace("?", "");
	sFileName.Replace("|", "");
	sFileName.Replace("%", "");
	sFileName.Replace("<", "");
	sFileName.Replace(">", "");
	sFileName.Replace("\"", "");
	sFileName.Replace("\\", "");

	CString sFullFileName = sTargetFolder + _T("\\") + sFileName;
	LogprintfConvert("INFO: Preparing ack-file generation %s", sFullFileName);

	::DeleteFile(sFullFileName);
	hndl = ::CreateFile(sFullFileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hndl == INVALID_HANDLE_VALUE) {
		sErrorMessage.Format("Cannot create message file (%s)", sFullFileName);
		LogprintfConvert("ERROR: %s\r\n", sErrorMessage);
		return FALSE;
	}
	if (::WriteFile(hndl, sNewContent.GetBuffer(4096), sNewContent.GetLength(), &nBytesWritten, NULL) == FALSE) {
		sErrorMessage.Format("Error writing message file (%s)", sFullFileName);
		sNewContent.ReleaseBuffer();
		::CloseHandle(hndl);
		LogprintfConvert("ERROR: %s\r\n", sErrorMessage);
		return FALSE;
	}
	sNewContent.ReleaseBuffer();
	::CloseHandle(hndl);
	LogprintfConvert("INFO: ack-file %s written.", sFullFileName);
	LogprintfConvert("%s", sNewContent);

	return TRUE;
}