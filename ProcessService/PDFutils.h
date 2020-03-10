#pragma once
#include "Defs.h"

#define PDFANNOTATIONTYPE_LINK		0
#define PDFANNOTATIONTYPE_NOTE		1
#define PDFANNOTATIONTYPE_BOOKMARK	2

typedef struct {
	int nMasterCopySeaprationSet = 0;
	double fXpos = 0.0;
	double fYpos = 0.0;
	double fXpos2 = 0.0;
	double fYpos2 = 0.0;
	int		nAnnotationType = PDFANNOTATIONTYPE_LINK;
	CString sAnnotationText = _T("");
	CString sAnnotationTooltip = _T("");
} PDFANNOTATION;

typedef CArray <PDFANNOTATION, PDFANNOTATION&> PDFANNOTATIONLIST;


int HasTrimBox(CString sInputFileName);
BOOL IsPDF(CString sFileName);
BOOL GetPDFinfo(CString sInputFileName, int &nPageCount, double &fSizeX, double &fSizeY);
int SplitPDF(CString sFullInputPath, CString sOutputFolder, CString sFileNameMask, int nPageNumberOffset, int &nPageCount, CString &sInfo, CString &sErrorMessage);

BOOL TrimPDF(CString sInputFileName, CString sOutputFileName, int nSnapMode, double fInputBBoxPosX, double fInputBBoxPosY,
				double fInputBBoxSizeX, double fInputBBoxSizeY,
				CString &sErrorMessage, int *nPageCount,  BOOL bCopyLinks);
BOOL GetPDFBox(CString sInputFileName, int nBoxType, PDFBOX &box);
int TrimPage(CString sInputName, CString sOutputName, CString &sErrorMessage);

