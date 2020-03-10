#pragma once
#include "Stdafx.h"

UINT ConvertThread(LPVOID nothing);

int ConvertPDF(int nTryNo, int nPDFmode, CString sSourcePath, CString sDestPath, int nPdfProcessID, CString &sErrorMessage);

int RunCallasProfile(CString sConfigFile, CString sInputPath, CString sOutputPath, CString sReportPath, CString sLogPath,
	DWORD &dwExitCode, int &nLogStatusOK, CString &sLogFileErrorMessage, BOOL &bFileOutputOK);

int  RunExternalConvert(int nTryNo, CString sSourceFile, CString sDestinationFile, int nPdfProcessID, CString &sErrorMessage);