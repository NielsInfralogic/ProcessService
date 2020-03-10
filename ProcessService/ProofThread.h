#pragma once
#include "Stdafx.h"
#include "Defs.h"

BOOL StartAllThreads();

UINT ProofThread(LPVOID nothing);

int GhostPDF(CString sFile, CString sJPEGname, int nJpegQuality, int nPreviewResolution, int nThumbnailResolution,
	BOOL bBlocking, PROOFPROCESSSTRUCT * pProofTemplate, CString sTitle);

int PDF2RIP(CString sFile, CString sJPEGname, int nJpegQuality, int nPreviewResolution, int nThumbnailResolution,
	PROOFPROCESSSTRUCT * pProofTemplate, CString sTitle);

int ProofCallas(CString sFile, CString sJPEGname, int nJpegQuality, int nPreviewResolution, int nThumbnailResolution,
	PROOFPROCESSSTRUCT * pProofTemplate, CString sTitle, CString &sErrorMessage);

