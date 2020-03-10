#include "stdafx.h"
#include "Defs.h"
#include "Utils.h"
#include "Prefs.h"
#include "math.h"
#include <iostream>
#include "pdflib.hpp"
#include "PDFutils.h"
//#include "DatabaseManager.h"

extern CPrefs g_prefs;
extern CUtils g_util;

static std::wstring PDF_LICENSE_NO = L"W900202-010092-132518-5CDVH2-68MH72"; // 29. Aug 2019

BOOL IsPDF(CString sFileName)
{
	FILE* fp;
	char data[8];

	TCHAR filename[MAX_PATH];
	strcpy(filename, sFileName);
	
	if ((fp = fopen( filename, "rb")) == 0) {
		return FALSE;
    }

//	%PDF-1.xx	

	if (fread(data, 1, 7, fp) != 7) {
		fclose(fp);
		return FALSE;
	}

	data[7] = 0;

	fclose(fp);

	if (!strcmp("%PDF-1.", data)) {
		return TRUE;
    }

	return FALSE;
}

// -1 - error
// 0: no trimbox
// 1: Has trimbox - must be trimmed
int HasTrimBox(CString sInputFileName)
{
	double fCurrentWidth = 0.0;
	double fCurrentHeight = 0.0;
	int nNumberOfPages = 1;
	if (GetPDFinfo(sInputFileName, nNumberOfPages, fCurrentWidth, fCurrentHeight) == FALSE) {
		g_util.LogprintfConvert(_T("ERROR: GetPDFinfo() returned FALSE for file %s"), sInputFileName);
		return -1;
	}

	PDFBOX box;
	PDFBOX mediabox;

	if (GetPDFBox(sInputFileName, PDFBOX_MEDIABOX, mediabox) == FALSE) {	// Will take bbox if no mediabox
		g_util.LogprintfConvert(_T("ERROR MEDIABOX not found PDF file %s"), sInputFileName);
		return -1;
	}
	g_util.LogprintfConvert(_T("INFO: Testing TRIMBOX.."));
	if (GetPDFBox(sInputFileName, PDFBOX_TRIMBOX, box) == FALSE) {
		g_util.LogprintfConvert(_T("WARNING: TRIMBOX test failed!"));
		if (GetPDFBox(sInputFileName, PDFBOX_ARTBOX, box) == FALSE) {
			g_util.LogprintfConvert(_T("WARNING: TRIMBOX test failed!"));
		}

		g_util.LogprintfConvert(_T("WARNING: NO trimbox found - using mediabox as trimbox.."));

		box.fXPos = mediabox.fXPos;
		box.fYPos = mediabox.fYPos;
		box.fXSize = mediabox.fXSize;
		box.fYSize = mediabox.fYSize;

	}

	g_util.LogprintfConvert(_T("INFO:  Found trimbox %.3f,  %.3f,  %.3f,  %.3f"), box.fXPos, box.fYPos, box.fXSize, box.fYSize);

	BOOL bTrimEqualsMediaBox = abs(mediabox.fXSize - box.fXSize) < 0.001 && abs(mediabox.fYSize - box.fYSize) < 0.001;
	
	return bTrimEqualsMediaBox ? 0 : 1;
}


BOOL GetPDFinfo(CString sInputFileName, int &nPageCount, double &fSizeX, double &fSizeY)// , PDFBOX &bleedbox, PDFBOX &trimbox, PDFBOX &cropbox, PDFBOX &mediabox)
{
	CUtils util;
	BOOL bRet = TRUE;
	TCHAR szErrorMessage[1024];

	TCHAR szFeature[200];
	_tcscpy(szFeature, "");

	/*	bleedbox.fXPos = 0.0;
		bleedbox.fYPos = 0.0;
		bleedbox.fXSize = 0.0;
		bleedbox.fYSize = 0.0;

		trimbox.fXPos = 0.0;
		trimbox.fYPos = 0.0;
		trimbox.fXSize = 0.0;
		trimbox.fYSize = 0.0;

		cropbox.fXPos = 0.0;
		cropbox.fYPos = 0.0;
		cropbox.fXSize = 0.0;
		cropbox.fYSize = 0.0;

		mediabox.fXPos = 0.0;
		mediabox.fYPos = 0.0;
		mediabox.fXSize = 0.0;
		mediabox.fYSize = 0.0;*/


	int doc = -1;
	int page = -1;
	char *docoptlist = (char *) "requiredmode=minimum";

	TCHAR szDummyFile[MAX_PATH];
	_stprintf(szDummyFile, "%s\\dummy.pdf", (LPCSTR)util.GetTempFolder());

	try {
		pdflib::PDFlib p;

		p.set_option(L"errorpolicy=return");
		p.set_option(L"license=" + PDF_LICENSE_NO);

		doc = p.open_pdi_document(util.charToWstring(sInputFileName), L"");
		if (doc == -1) {
			_stprintf(szErrorMessage, "Couldn't open PDF input file '%s'", (LPCSTR)sInputFileName);
			util.LogprintfConvert("ERROR: PDF_open_pdi_document failed - %s", util.WstringTochar(p.get_errmsg()));
			return FALSE;
		}

		int  numberofpages = (int)p.pcos_get_number(doc, L"/Root/Pages/Count");
		nPageCount = numberofpages;

		// Get documnet information from input file
	//	util.Logprintf("INFO: PDFlib PDF_open_pdi called");

	/*	page = PDF_open_pdi_page(p, doc, 1, _T(""));
		if (page == -1) {
			_stprintf(szErrorMessage,"Couldn't open page 1 of PDF file '%s'", sInputFileName);
			util.Logprintf("ERROR: PDF_open_pdi_page failed - %s", PDF_get_errmsg(p));
			PDF_delete(p);
			return FALSE;
		} */
		//	util.Logprintf("INFO: PDFlib PDF_open_pdi_page called");

		double width = p.pcos_get_number(doc, L"pages[0]/width");
		double height = p.pcos_get_number(doc, L"pages[0]/height");


		/*	double mediabox_llx = PDF_pcos_get_number(p, doc, "pages[0]/MediaBox[0]");
			double mediabox_lly = PDF_pcos_get_number(p, doc, "pages[0]/MediaBox[1]");
			double mediabox_urx = PDF_pcos_get_number(p, doc, "pages[0]/MediaBox[2]");
			double mediabox_ury = PDF_pcos_get_number(p, doc, "pages[0]/MediaBox[3]");

			if (mediabox_urx == 0.0 || mediabox_ury == 0.0) {
				mediabox_llx = 0.0;
				mediabox_lly = 0.0;
				mediabox_urx = width;
				mediabox_ury = height;
			}
			*/
			//		double xx = PDF_pcos_get_number(p, doc, "pages[0]/CropBox[3]");

				//	double cropbox_llx = PDF_get_pdi_value(p, "/CropBox/llx", doc, page, 0);
				//	double cropbox_lly = PDF_get_pdi_value(p, "/CropBox/lly", doc, page, 0);
				//	double cropbox_urx = PDF_get_pdi_value(p, "/CropBox/urx", doc, page, 0);
				//	double cropbox_ury = PDF_get_pdi_value(p, "/CropBox/ury", doc, page, 0);

				/*	double cropbox_llx = PDF_pcos_get_number(p, doc, "pages[0]/CropBox[0]");
					double cropbox_lly = PDF_pcos_get_number(p, doc, "pages[0]/CropBox[1]");
					double cropbox_urx = PDF_pcos_get_number(p, doc, "pages[0]/CropBox[2]");
					double cropbox_ury = PDF_pcos_get_number(p, doc, "pages[0]/CropBox[3]");

					if (cropbox_llx == 0.0 && cropbox_lly == 0.0 && cropbox_urx == 0.0 && cropbox_ury == 0.0) {*/
					//		double cropbox_llx = mediabox_llx;
					//		double cropbox_lly = mediabox_lly;
					//		double cropbox_urx = mediabox_urx;
					//		double cropbox_ury = mediabox_ury;
					//	}
				//		util.Logprintf("INFO:  Cropbox (%.2f,%.2f,%.2f,%.2f)", cropbox_llx,cropbox_lly, cropbox_urx,cropbox_ury);

						//double bleedbox_llx = PDF_get_pdi_value(p, "/BleedBox/llx", doc, page, 0);
						//double bleedbox_lly = PDF_get_pdi_value(p, "/BleedBox/lly", doc, page, 0);
						//double bleedbox_urx = PDF_get_pdi_value(p, "/BleedBox/urx", doc, page, 0);
						//double bleedbox_ury = PDF_get_pdi_value(p, "/BleedBox/ury", doc, page, 0);
					/*	double bleedbox_llx = PDF_pcos_get_number(p, doc, "pages[0]/BleedBox[0]");
						double bleedbox_lly = PDF_pcos_get_number(p, doc, "pages[0]/BleedBox[1]");
						double bleedbox_urx = PDF_pcos_get_number(p, doc, "pages[0]/BleedBox[2]");
						double bleedbox_ury = PDF_pcos_get_number(p, doc, "pages[0]/BleedBox[3]");
						if (bleedbox_llx == 0.0 && bleedbox_lly == 0.0 && bleedbox_urx == 0.0 && bleedbox_ury == 0.0) {
							bleedbox_llx = mediabox_llx;
							bleedbox_lly = mediabox_lly;
							bleedbox_urx = mediabox_urx;
							bleedbox_ury = mediabox_ury;
						}*/
						//		util.Logprintf("INFO:  Bleedbox (%.2f,%.2f,%.2f,%.2f)", bleedbox_llx,bleedbox_lly, bleedbox_urx,bleedbox_ury);


								//double trimbox_llx = PDF_get_pdi_value(p, "/TrimBox/llx", doc, page, 0);
								//double trimbox_lly = PDF_get_pdi_value(p, "/TrimBox/lly", doc, page, 0);
								//double trimbox_urx = PDF_get_pdi_value(p, "/TrimBox/urx", doc, page, 0);
								//double trimbox_ury = PDF_get_pdi_value(p, "/TrimBox/ury", doc, page, 0);
							/*	double trimbox_llx = PDF_pcos_get_number(p, doc, "pages[0]/TrimBox[0]");
								double trimbox_lly = PDF_pcos_get_number(p, doc, "pages[0]/TrimBox[1]");
								double trimbox_urx = PDF_pcos_get_number(p, doc, "pages[0]/TrimBox[2]");
								double trimbox_ury = PDF_pcos_get_number(p, doc, "pages[0]/TrimBox[3]");

								if (trimbox_llx == 0.0 && trimbox_lly == 0.0 && trimbox_urx == 0.0 && trimbox_ury == 0.0) {
									trimbox_llx = cropbox_llx;
									trimbox_lly = cropbox_lly;
									trimbox_urx = cropbox_urx;
									trimbox_ury = cropbox_ury;
								}	*/
								//		util.Logprintf("INFO:  Trimbox (%.2f,%.2f,%.2f,%.2f)", trimbox_llx,trimbox_lly, trimbox_urx,trimbox_ury);	


		fSizeX = width * 25.4 / 72.0;
		fSizeY = height * 25.4 / 72.0;


		/*	mediabox.fXPos = mediabox_llx*25.4/72.0;
			mediabox.fYPos = mediabox_lly*25.4/72.0;
			mediabox.fXSize = (mediabox_urx-mediabox_llx)*25.4/72.0;
			mediabox.fYSize = (mediabox_ury-mediabox_lly)*25.4/72.0;

			cropbox.fXPos = cropbox_llx*25.4/72.0;
			cropbox.fYPos = cropbox_lly*25.4/72.0;
			cropbox.fXSize = (cropbox_urx-cropbox_llx)*25.4/72.0;
			cropbox.fYSize = (cropbox_ury-cropbox_lly)*25.4/72.0;

			trimbox.fXPos = trimbox_llx*25.4/72.0;
			trimbox.fYPos = trimbox_lly*25.4/72.0;
			trimbox.fXSize = (trimbox_urx-trimbox_llx)*25.4/72.0;
			trimbox.fYSize = (trimbox_ury-trimbox_lly)*25.4/72.0;

			bleedbox.fXPos = bleedbox_llx*25.4/72.0;
			bleedbox.fYPos = bleedbox_lly*25.4/72.0;
			bleedbox.fXSize = (bleedbox_urx-bleedbox_llx)*25.4/72.0;
			bleedbox.fYSize = (bleedbox_ury-bleedbox_lly)*25.4/72.0;
	*/

	//	util.Logprintf("INFO: PDF size %.4f,%.4f", width, height);



	//	PDF_close_pdi_page(p, page);

		p.close_pdi_document(doc);

	}
	catch (pdflib::PDFlib::Exception &ex) {
		_stprintf(szErrorMessage, _T("PDFlib exception occurred (0): [%d] %s: %s"), ex.get_errnum(),
			(LPCSTR)util.WstringTochar(ex.get_apiname()), (LPCSTR)util.WstringTochar(ex.get_errmsg()));
		util.LogprintfConvert("ERROR %s", szErrorMessage);
		return FALSE;
	}
	return TRUE;

}


// bleedtocut - positive cut left side, negative cut right side
BOOL TrimPDF(CString sInputFileName, CString sOutputFileName, int nSnapMode, double fInputBBoxPosX, double fInputBBoxPosY,
	double fInputBBoxSizeX, double fInputBBoxSizeY,
	CString &sErrorMessage, int *nPageCount,  BOOL bCopyLinks)
{
	CUtils util;
	BOOL bRet = TRUE;
	*nPageCount = 0;

	TCHAR szFeature[200];

	CString sOptList = _T("");
	CString sAnn, sRect;

	_tcscpy(szFeature, "");
	sErrorMessage = _T("");

	fInputBBoxPosX = -1.0 * fInputBBoxPosX;
	fInputBBoxPosY = -1.0 * fInputBBoxPosY;



	int doc = -1;
	int page = -1;
	int numberofpages = 0;
	try {
		pdflib::PDFlib p;

		p.set_option(L"errorpolicy=return");
		p.set_option(L"license=" + PDF_LICENSE_NO);

		doc = p.open_pdi_document(util.charToWstring(sInputFileName), L"");
		if (doc == -1) {
			sErrorMessage.Format("Couldn't open PDF input file '%s'", (LPCSTR)sInputFileName);
			util.LogprintfConvert("ERROR %s", sErrorMessage);
			return FALSE;
		}

		// Get documnet information from input file

		numberofpages = (int)p.pcos_get_number(doc, L"/Root/Pages/Count");

		double version = p.pcos_get_number(doc, L"pdfversion");

		if (version < 13)
			version = 13;

		if (numberofpages == 0) {
			sErrorMessage.Format("Couldn't not get PDF page count from PDF file '%s'", (LPCSTR)sInputFileName);
			util.LogprintfConvert("ERROR %s", sErrorMessage);
			return FALSE;
		}


		TCHAR szOptions[100];
		_stprintf(szOptions, "compatibility=%.1f", version / 10.0);
		if (p.begin_document(util.charToWstring(sOutputFileName), util.charToWstring(szOptions)) == -1) {

			sErrorMessage.Format("%s\n", util.WstringTochar(p.get_errmsg()));
			util.LogprintfConvert("ERROR %s", sErrorMessage);
			//	PDF_close_pdi_page(p, page);
			return FALSE;
		}


		p.set_info(L"Creator", L"InfraLogic");
		p.set_info(L"Author", L"FileCenter");
		p.set_info(L"Title", util.charToWstring(util.GetFileName(sOutputFileName, TRUE)));

		for (int i = 1; i <= numberofpages; i++) {
			page = p.open_pdi_page(doc, i, L"");
			if (page == -1) {
				sErrorMessage.Format("Couldn't open page 1 of PDF file '%s'", (LPCSTR)sInputFileName);
				util.LogprintfConvert("ERROR: %s", sErrorMessage);
				::DeleteFile(sOutputFileName);
				return FALSE;
			}

			int infolen = 0;

			double thiswidth = p.pcos_get_number(doc, L"pages[0]/width");
			double thisheight = p.pcos_get_number(doc, L"pages[0]/height");

			double fBBoxSizeX = 72.0 * fInputBBoxSizeX / 25.4;
			double fBBoxSizeY = 72.0 * fInputBBoxSizeY / 25.4;
			util.LogprintfConvert("INFO:  PDFcrop: File size SizeX=%.2f, SizeY=%.2f", thiswidth, thisheight);

			if (fBBoxSizeX == 0)
				fBBoxSizeX = thiswidth;
			if (fBBoxSizeY == 0)
				fBBoxSizeY = thisheight;

			double fBBoxPosXsetup = 72.0 * fInputBBoxPosX / 25.4;
			double fBBoxPosYsetup = 72.0 * fInputBBoxPosY / 25.4;

			double fBBoxPosX = 0.0;
			double fBBoxPosY = 0.0;


			switch (nSnapMode) {
			case SNAP_CENTER:
				fBBoxPosX += (thiswidth - fBBoxSizeX) / 2.0;
				fBBoxPosY += (thisheight - fBBoxSizeY) / 2.0;
				break;
			case SNAP_UR:
				fBBoxPosX += (thiswidth - fBBoxSizeX);
				break;
			case SNAP_LL:
				fBBoxPosY += (thisheight - fBBoxSizeY);
				break;
			case SNAP_LR:
				fBBoxPosX += (thiswidth - fBBoxSizeX);
				fBBoxPosY += (thisheight - fBBoxSizeY);
				break;
			case SNAP_UC:
				fBBoxPosX += (thiswidth - fBBoxSizeX) / 2.0;
				break;
			case SNAP_LC:
				fBBoxPosX += (thiswidth - fBBoxSizeX) / 2.0;
				fBBoxPosY += (thisheight - fBBoxSizeY);
				break;
			case SNAP_CL:
				fBBoxPosY += (thisheight - fBBoxSizeY) / 2.0;
				break;
			case SNAP_CR:
				fBBoxPosX += (thiswidth - fBBoxSizeX);
				fBBoxPosY += (thisheight - fBBoxSizeY) / 2.0;
				break;
				// SNAP_UL - do nothing..
				break;
			}

			p.begin_page_ext(fBBoxSizeX, fBBoxSizeY, L"");

			double clipx = 0.0;
			double clipy = 0.0;

			// new clip calculations...
			if (fBBoxPosX == 0.0)
				clipx = 0.0;
			else if (fBBoxPosX == thiswidth - fBBoxSizeX)
				clipx = 100.0;
			else if (fBBoxPosX == (thiswidth - fBBoxSizeX) / 2.0)
				clipx = 50.0;

			// new clip calculations...
			if (fBBoxPosY == 0.0)
				clipy = 0.0;
			else if (fBBoxPosY == thisheight - fBBoxSizeY)
				clipy = 100.0;
			else if (fBBoxPosY == (thisheight - fBBoxSizeY) / 2.0)
				clipy = 50.0;

			clipy = 100 - clipy;

			util.LogprintfConvert("INFO:  PDFcrop: Final Snap adjust PosX=%.2f, PosY=%.2f", fBBoxPosX, fBBoxPosY);
			util.LogprintfConvert("INFO:  PDFcrop: Final SizeX=%.2f, SizeY=%.2f", fBBoxSizeX, fBBoxSizeY);
			util.LogprintfConvert("INFO:  PDFcrop: Final PosX=%.2f, PosY=%.2f", fBBoxPosXsetup, fBBoxPosYsetup);
			util.LogprintfConvert("INFO:  PDFcrop: Final clipx=%.2f, clipy=%.2f", clipx, clipy);

			_stprintf(szFeature, "boxsize {%.3f %.3f} position {%.3f %.3f} fitmethod clip", fBBoxSizeX, fBBoxSizeY, clipx, clipy);


			if (bCopyLinks) {
				int anncount = (int)p.pcos_get_number(doc, L"length:pages[0]/annots");

				PDFANNOTATIONLIST aList;
				aList.RemoveAll();

				if (anncount > 0) {
					for (int ann = 0; ann < anncount; ann++) {
						double xPos = -1.0, yPos = -1.0, xPos2 = -1.0, yPos2 = -1.0;
						CString sUri = "";
						sAnn.Format("pages[0]/annots[%d]", (LPCSTR)ann);
						CString subtype = util.WstringTochar(p.pcos_get_string(doc, util.charToWstring(sAnn + "/Subtype")));
						if (subtype == "Link") {
							sRect.Format("%s/Rect", (LPCSTR)sAnn);

							CString sType = util.WstringTochar(p.pcos_get_string(doc, util.charToWstring("type:" + sAnn + "/A")));
							if (sType == "dict") {
								CString action_type = util.WstringTochar(p.pcos_get_string(doc, util.charToWstring(sAnn + "/A/S")));
								if (action_type == "URI")
									sUri = util.WstringTochar(p.pcos_get_string(doc, util.charToWstring(sAnn + "/A/URI")));
							}

							sType = util.WstringTochar(p.pcos_get_string(doc, util.charToWstring("type:" + sRect)));
							if (sType == "array") {
								int nCnt = (int)p.pcos_get_number(doc, util.charToWstring("length:" + sRect));
								if (nCnt == 4) {
									xPos = p.pcos_get_number(doc, util.charToWstring(sRect + "[0]"));
									yPos = p.pcos_get_number(doc, util.charToWstring(sRect + "[1]"));
									xPos2 = p.pcos_get_number(doc, util.charToWstring(sRect + "[2]"));
									yPos2 = p.pcos_get_number(doc, util.charToWstring(sRect + "[3]"));
								}
							}
						}

						if (sUri != "" && xPos >= 0.0 && yPos >= 0.0 && xPos2 >= 0.0 && yPos2 >= 0.0) {
							util.LogprintfConvert("INFO:  PDFcrop: Found URI link %s", sUri);

							PDFANNOTATION* pItem = new PDFANNOTATION();
							pItem->fXpos = xPos;
							pItem->fYpos = yPos;
							pItem->fXpos2 = xPos2;
							pItem->fYpos2 = yPos2;
							pItem->nAnnotationType = PDFANNOTATIONTYPE_LINK;
							pItem->sAnnotationText = sUri;

							aList.Add(*pItem);
						}
					}
				}

				for (int lnk = 0; lnk < aList.GetCount(); lnk++) {

					sOptList.Format(_T("url={%s}"), (LPCSTR)aList[lnk].sAnnotationText);
					int action = p.create_action(L"URI", util.charToWstring(sOptList));

					sOptList.Format(_T("action={activate %d} linewidth=0"), action);

					p.create_annotation(aList[lnk].fXpos, aList[lnk].fYpos, aList[lnk].fXpos2, aList[lnk].fYpos2, L"Link", util.charToWstring(sOptList));
				}
			}
			p.fit_pdi_page(page, -1.0 * fBBoxPosXsetup, fBBoxPosYsetup, util.charToWstring(szFeature));
			p.close_pdi_page(page);
			p.end_page_ext(L"");
		}

		p.end_document(L"");
	}
	catch (pdflib::PDFlib::Exception &ex) {
		sErrorMessage.Format(_T("PDFlib exception occurred (0): [%d] %s: %s"), ex.get_errnum(),
			(LPCSTR)util.WstringTochar(ex.get_apiname()), (LPCSTR)util.WstringTochar(ex.get_errmsg()));

		util.LogprintfConvert("ERROR %s", sErrorMessage);
		::DeleteFile(sOutputFileName);
		return FALSE;
	}
	*nPageCount = numberofpages;

	return TRUE;
}

BOOL GetPDFBox(CString sInputFileName, int nBoxType, PDFBOX &box)
{
	CUtils util;
	BOOL bRet = TRUE;

	TCHAR szErrorMessage[1024];

	double fVals[4] = { 0.0 };

	box.fXPos = 0.0;
	box.fYPos = 0.0;
	box.fXSize = 0.0;
	box.fYSize = 0.0;

	int doc = -1;

	char *searchpath = (char *) "../data";
	const char *docoptlist = "requiredmode=minimum";

	//TCHAR szDummyFile[MAX_PATH];
	//_stprintf(szDummyFile, "%s\\dummy.pdf", (LPCSTR)util.GetTempFolder());

	try {
		pdflib::PDFlib p;

		p.set_option(L"errorpolicy=return");
		p.set_option(L"license=" + PDF_LICENSE_NO);

		doc = p.open_pdi_document(util.charToWstring(sInputFileName), L"");
		if (doc == -1) {
			_stprintf(szErrorMessage, "Couldn't open PDF input file '%s'", (LPCSTR)sInputFileName);
			util.LogprintfConvert("ERROR: PDF_open_pdi_document failed - %s", util.WstringTochar(p.get_errmsg()));
			return FALSE;
		}

		TCHAR szBoxName[200];

		for (int i = 0; i < 4; i++) {
			_stprintf(szBoxName, "pages[0]/MediaBox[%d]", i);

			if (nBoxType == PDFBOX_CROPBOX)
				_stprintf(szBoxName, "pages[0]/CropBox[%d]", i);

			else if (nBoxType == PDFBOX_TRIMBOX)
				_stprintf(szBoxName, "pages[0]/TrimBox[%d]", i);

			else if (nBoxType == PDFBOX_BLEEDBOX)
				_stprintf(szBoxName, "pages[0]/BleedBox[%d]", i);

			else if (nBoxType == PDFBOX_ARTBOX)
				_stprintf(szBoxName, "pages[0]/ArtBox[%d]", i);

			fVals[i] = p.pcos_get_number(doc, util.charToWstring(szBoxName));
		}



		box.fXPos = fVals[0] * 25.4 / 72.0;
		box.fYPos = fVals[1] * 25.4 / 72.0;
		box.fXSize = (fVals[2] - fVals[0]) *25.4 / 72.0;
		box.fYSize = (fVals[3] - fVals[1])*25.4 / 72.0;

		if (nBoxType == PDFBOX_MEDIABOX && (box.fXSize == 0.0 || box.fYSize == 0.0)) {
			box.fXSize = p.pcos_get_number(doc, L"pages[0]/width") * 25.4 / 72.0;
			box.fYSize = p.pcos_get_number(doc, L"pages[0]/height") * 25.4 / 72.0;
		}

		// Experimental..

/*		_stprintf(szBoxName, _T(""));
		if (nBoxType == PDFBOX_CROPBOX)
			_stprintf(szBoxName, "pdiusebox=crop");

		else if (nBoxType == PDFBOX_TRIMBOX)
			_stprintf(szBoxName, "pdiusebox=trim");

		else if (nBoxType == PDFBOX_BLEEDBOX)
			_stprintf(szBoxName, "pdiusebox=bleed");

		else if (nBoxType == PDFBOX_ARTBOX)
			_stprintf(szBoxName, "pdiusebox=art");

		int page = PDF_open_pdi_page(p, doc, 1, szBoxName);
		double w = PDF_info_pdi_page(p, 1, "pagewidth", _T(""));
		double h = PDF_info_pdi_page(p, 1, "pageheight", _T(""));

		util.Logprintf("INFO: PDFbox(1) %d %.3f %.3f ", nBoxType, w, h);
		w = p.pcos_get_number(doc, L"pages[0]/width");
		h = p.pcos_get_number(doc, L"pages[0]/height");
		util.Logprintf("INFO: PDFbox(2) %d %.3f %.3f ", nBoxType, w, h);
		p.close_pdi_page(1);


*/

		p.close_pdi_document(doc);
	}
	catch (pdflib::PDFlib::Exception &ex) {
		_stprintf(szErrorMessage, _T("PDFlib exception occurred (0): [%d] %s: %s"), ex.get_errnum(),
			(LPCSTR)util.WstringTochar(ex.get_apiname()), (LPCSTR)util.WstringTochar(ex.get_errmsg()));
		util.LogprintfConvert("ERROR %s", szErrorMessage);
		return FALSE;
	}

	return box.fXSize > 0 && box.fYSize > 0;
}



int SplitPDF(CString sFullInputPath, CString sOutputFolder, CString sFileNameMask, int nPageNumberOffset, int &nPageCount, CString &sInfo, CString &sErrorMessage)
{
	CUtils util;
	BOOL bRet = TRUE;

	sInfo = _T("");
	sErrorMessage = _T("");

	if (sFileNameMask == _T(""))
		sFileNameMask = util.GetFileName(sFullInputPath);

	if (nPageNumberOffset <= 0)
		nPageNumberOffset = 1;

	CString sFileNameMaskOrg = sFileNameMask;

	TCHAR szThisFilePath[MAX_PATH], szFeature[200];
	_tcscpy(szFeature, "");

	int doc = -1;
	int page = -1;
	nPageCount = 0;

	try {
		pdflib::PDFlib p;

		p.set_option(L"errorpolicy=return");
		p.set_option(L"license=" + PDF_LICENSE_NO);

		doc = p.open_pdi_document(util.charToWstring(sFullInputPath), L"");
		if (doc == -1) {
			sErrorMessage.Format("Couldn't open PDF input file '%s'", sFullInputPath);
			util.LogprintfConvert("ERROR: PDF_open_pdi_document failed - %s", util.WstringTochar(p.get_errmsg()));
			util.LogprintfConvert("ERROR: %s", sErrorMessage);
			return -1;
		}

		// Get documnet information from input file

		double version = p.pcos_get_number(doc, L"pdfversion");

		// Version 6.1 : int numberofpages  = PDF_get_pdi_value(p, "/Root/Pages/Count", doc, -1, 0);
		nPageCount = (int)p.pcos_get_number(doc, L"/Root/Pages/Count");

		if (nPageCount == 0) {
			sErrorMessage.Format("Couldn't not get PDF page count from PDF file '%s'", sFullInputPath);
			util.LogprintfConvert("ERROR: %s", sErrorMessage);
			return -1;
		}

		/*
		if (nPageCount == 1) {
			sInfo.Format("Pagecount is 1 - no splitting action performed", sFullInputPath);
			return 0;

		}*/

		util.LogprintfConvert("INFO:  Number of pages in document %d", nPageCount);

		BOOL useoffset = FALSE;
		int i = 0;
		BOOL gotend = FALSE;
		for (int i = 1; i <= nPageCount; i++) {
			sFileNameMask = sFileNameMaskOrg;
			util.LogprintfConvert("INFO:  Processing page %d..", i);
			// Form output name of this page

			CString sPageName;
			sPageName.Format("%d", i + nPageNumberOffset - 1);


			if (sFileNameMask.Find("###") != -1) {
				sPageName.Format("%.3d", i + nPageNumberOffset - 1);
				sFileNameMask.Replace("###", sPageName);
			}
			else if (sFileNameMask.Find("##") != -1) {
				sPageName.Format("%.2d", i + nPageNumberOffset - 1);
				sFileNameMask.Replace("##", sPageName);
			}
			else if (sFileNameMask.Find("#") != -1) {
				sPageName.Format("%d", i + nPageNumberOffset - 1);
				sFileNameMask.Replace("#", sPageName);
			}
			/*else  if (sFileNameMask.Find("@") != -1) {

				//  xyz-@23-zyz
				//   	4. x

				int ix = sFileNameMask.Find("@") + 1;
				int ix0 = ix;
				while (isdigit(sFileNameMask[ix]))
					ix++;
				sPageName.Format("%d", i + atoi(sFileNameMask.Mid(ix0)) - 1);
				sFileNameMask = sFileNameMask.Left(ix0 - 1) + sPageName + sFileNameMask.Mid(ix);
			} 	 */
			else
				sFileNameMask = util.GetFileName(sFileNameMask, TRUE) + _T("-") + sPageName + _T(".") + util.GetExtension(sFileNameMask);

			_stprintf(szThisFilePath, "%s\\%s.tmp", (LPCSTR)sOutputFolder, (LPCSTR)sFileNameMask);

			TCHAR szOptions[100];
			sprintf(szOptions, "compatibility=%.1f", version / 10.0);

			if (p.begin_document(util.charToWstring(szThisFilePath), util.charToWstring(szOptions)) == -1) {
				sErrorMessage.Format("%s", util.WstringTochar(p.get_errmsg()));
				p.close_pdi_page(page);
				util.LogprintfConvert("ERROR: %s", sErrorMessage);
				return -1;
			}

			p.set_info(L"Creator", L"InfraLogic");
			p.set_info(L"Author", L"FileCenter");
			p.set_info(L"Title", util.charToWstring(util.GetFileName(sFileNameMask, TRUE)));

			page = p.open_pdi_page(doc, i, L"cloneboxes");

			if (page == -1) {
				gotend = TRUE;
				break;
			}
			TCHAR szStr[40];
			sprintf(szStr, "pages[%d]/width", i - 1);
			double width = p.pcos_get_number(doc, util.charToWstring(szStr));
			sprintf(szStr, "pages[%d]/height", i - 1);
			double height = p.pcos_get_number(doc, util.charToWstring(szStr));

			//			util.Logprintf("INFO:  PDF version %.1f", version/10.0);

						//p.begin_page_ext(width, height, L"");
						// Dummy page size; will be adjusted later 
			p.begin_page_ext(10, 10, L"");

			//p.fit_pdi_page(page, 0, 0, L"");
			p.fit_pdi_page(page, 0, 0, L"cloneboxes");

			p.close_pdi_page(page);
			p.end_page_ext(L"");
			p.end_document(L"");

			CString sFinalName(szThisFilePath);
			sFinalName.Replace(".tmp", "");

			if (::MoveFileEx(szThisFilePath, sFinalName, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == FALSE) {
				::Sleep(500);
				if (::MoveFileEx(szThisFilePath, sFinalName, MOVEFILE_COPY_ALLOWED | MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH) == FALSE) {
					sErrorMessage.Format("Unable to rename file %s to %s", szThisFilePath, sFinalName);
					util.LogprintfConvert("ERROR: %s", sErrorMessage);
					return -1;
				}
			}
		}
	}

	catch (pdflib::PDFlib::Exception &ex) {
		sErrorMessage.Format(_T("PDFlib exception occurred (0): [%d] %s: %s"), ex.get_errnum(),
			util.WstringTochar(ex.get_apiname()), util.WstringTochar(ex.get_errmsg()));
		util.LogprintfConvert("ERROR %s", sErrorMessage);
		return -1;
	}

	sInfo.Format("PDF split in %d pages", nPageCount);
	return 1;
}


// -1 : error - no outpfile
// 0 : No trim done - no outputfile
// 1: Page trimmed - file in outputfile (and inputfile

int TrimPage(CString sInputName, CString sOutputName, CString &sErrorMessage)
{
	if (g_util.FileExist(sInputName) == FALSE)
		return -1;

	double fCurrentWidth = 0.0;
	double fCurrentHeight = 0.0;
	int nNumberOfPages = 1;
	if (GetPDFinfo(sInputName, nNumberOfPages, fCurrentWidth, fCurrentHeight) == FALSE) {
		g_util.LogprintfConvert(_T("ERROR: GetPDFinfo() returned FALSE for file %s"), sInputName);
		return -1;
	}

	PDFBOX box;
	PDFBOX mediabox;

	if (GetPDFBox(sInputName, PDFBOX_MEDIABOX, mediabox) == FALSE) {	// Will take bbox if no mediabox
		g_util.LogprintfConvert(_T("ERROR MEDIABOX not found PDF file %s"), sInputName);
		return -1;
	}
	g_util.LogprintfConvert(_T("INFO: Testing TRIMBOX.."));
	if (GetPDFBox(sInputName, PDFBOX_TRIMBOX, box) == FALSE) {
		g_util.LogprintfConvert(_T("WARNING: TRIMBOX test failed!"));
		if (GetPDFBox(sInputName, PDFBOX_ARTBOX, box) == FALSE) {
			g_util.LogprintfConvert(_T("WARNING: TRIMBOX test failed!"));
		}

		g_util.LogprintfConvert(_T("WARNING: NO trimbox found - using mediabox as trimbox.."));

		box.fXPos = mediabox.fXPos;
		box.fYPos = mediabox.fYPos;
		box.fXSize = mediabox.fXSize;
		box.fYSize = mediabox.fYSize;

	}

	g_util.LogprintfConvert(_T("INFO:  Found trimbox %.3f,  %.3f,  %.3f,  %.3f"), box.fXPos, box.fYPos, box.fXSize, box.fYSize);

	BOOL bTrimEqualsMediaBox = abs(mediabox.fXSize - box.fXSize) < 0.001 && abs(mediabox.fYSize - box.fYSize) < 0.001;
	if (bTrimEqualsMediaBox) {
		g_util.LogprintfConvert(_T("WARNING: TRIMBOX equal to MEDIABOX in PDF file %s"), sInputName);
		return 0;
	}

	double fCropPosX = box.fXPos;
	double fCropPosY = fCurrentHeight - (box.fYPos + box.fYSize);
	double fCropSizeX = box.fXSize;
	double fCropSizeY = box.fYSize;

	double fMaxBleedX = box.fXPos;
	double fMaxBleedY = box.fYPos;
	/*
	if (bTrimEqualsMediaBox == FALSE) {

		double finalBleedToKeepX = pActionSetup->m_pdfbleedtokeep <= fMaxBleedX ? pActionSetup->m_pdfbleedtokeep : fMaxBleedX;
		double finalBleedToKeepY = pActionSetup->m_pdfbleedtokeep <= fMaxBleedY ? pActionSetup->m_pdfbleedtokeep : fMaxBleedY;

		fCropPosX -= finalBleedToKeepX;
		fCropPosY -= finalBleedToKeepY;
		fCropSizeX += 2.0 * finalBleedToKeepX;
		fCropSizeY += 2.0 * finalBleedToKeepY;
		fBleedCut = 0; //not used..
		if (pActionSetup->m_pdfcutbleedinspine) {
			fCropSizeX -= finalBleedToKeepX;
			if (bIsOddPage)
				fCropPosX += finalBleedToKeepX;

		}

		util.Logprintf(_T("INFO:  Found trimbox adjusted %.3f,  %.3f,  %.3f,  %.3f (Oddpage = %d) "), fCropPosX, fCropPosY, fCropSizeX, fCropSizeY, bIsOddPage);
	}
	*/

	// TrimPage wants it like this...!:
	fCropPosX *= -1.0;
	fCropPosY *= -1.0;
	if (fCropSizeX == 0.0 || fCropSizeY == 0.0) {
		g_util.LogprintfConvert(_T("ERROR - no page size found in %s"), sInputName);
		return 0;
	}
	::DeleteFile(sOutputName);

	if (fCropPosX != 0.0 || fCropPosY != 0.0) {

		// In case of offset crop - do this first - then apply snap in 2. step...
		if (TrimPDF(sInputName, sOutputName, SNAP_UL, fCropPosX, fCropPosY, 0, 0,
			sErrorMessage, &nNumberOfPages, TRUE)) {
			if (::CopyFile(sOutputName, sInputName, FALSE) == FALSE) {
				::Sleep(1000);
				::CopyFile(sOutputName, sInputName, FALSE);
			}
			::DeleteFile(sOutputName);
			if (TrimPDF(sInputName, sOutputName, SNAP_UL, 0, 0, fCropSizeX, fCropSizeY,
				sErrorMessage, &nNumberOfPages, TRUE)) {
				g_util.LogprintfConvert(_T("Cropped PDF file to %.2f x %.2f  (%d pages)"), fCropSizeX, fCropSizeY, nNumberOfPages);
			}
			else {
				g_util.LogprintfConvert(_T("ERROR trimming PDF file (2) %s - %s"), sInputName, sErrorMessage);
				return -1;
			}
		}
		else {
			g_util.LogprintfConvert(_T("ERROR trimming PDF file (1) %s - %s"), sInputName, sErrorMessage);
			return -1;
		}
	}
	else {

		if (TrimPDF(sInputName, sOutputName, SNAP_UL, fCropPosX, fCropPosY, fCropSizeX, fCropSizeY, sErrorMessage, &nNumberOfPages, TRUE)) {
			g_util.LogprintfConvert(_T("Cropped PDF file to %.2f x %.2f  (%d pages)"), fCropSizeX, fCropSizeY, nNumberOfPages);
		}
		else {
			g_util.LogprintfConvert(_T("ERROR trimming PDF file (3) %s - %s"), sInputName, sErrorMessage);
			return -1;
		}
	}

	return 1;
}


