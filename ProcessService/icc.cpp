
// ICC.cpp implementation

#include "stdafx.h"
#include "ProcessService.h"
#include <math.h>
#include "lcms2.h"
#include "Utils.h"
#include "icc.h"

extern CUtils g_util;

#define ALARM_R	255
#define ALARM_G	128
#define ALARM_B	255

int	ICCTransformFlag = 0; // cmsFLAGS_NOTPRECALC;

void MyICCLogErrorHandler(cmsContext ContextID, cmsUInt32Number ErrorCode, const char *Text)
{
	g_util.LogprintfResample("ERROR:  LMCS error %s", Text);
}
void LogICCdata(cmsHPROFILE hProfile)
{
	//	g_util.Logprintf("ICC info:  Product name       : %s\r\n", cmsTakeProductName(hProfile));
	//	g_util.Logprintf("ICC info:  Product description: %s\r\n", cmsTakeProductDesc(hProfile));

}

/*

void LogICCmode(CJob *pJob)
{
	cmsSetLogErrorHandler(MyICCLogErrorHandler);
	CString rend[4] = { "Perceptual", "Relative colorimetric", "Saturation", "Absolute colorimetric" };
	CString mode[3] = { "Input-Output transform", "Device-link transform", "Input-Output-Proof transform" };

	int ICCTransformMode = ICC_INPUT_OUTPUT_TRANSFORM;
	if (pJob->m_sICCInputProfile == "")
		ICCTransformMode = ICC_DEVICE_LINK_TRANSFORM;
	if (pJob->m_bUseICCProofProfile)
		ICCTransformMode = ICC_INPUT_OUTPUT_PROOF_TRANSFORM;

	g_util.Logprintf("INFO:  ICC Input profile is %s\r\n", pJob->m_sICCInputProfile);
	g_util.Logprintf("INFO:  ICC Output profile for monitor is %s\r\n", pJob->m_sICCMonitorProfile);
	g_util.Logprintf("INFO:  ICC Output profile for printer is %s\r\n", pJob->m_sICCPrinterProfile);
	if (pJob->m_bUseICCProofProfile)
		Logprintf("INFO:  ICC Proof profile for emulation is %s\r\n", pJob->m_sICCProofProfile);

	g_util.Logprintf("INFO:  ICC transformation is %s\r\n", mode[ICCTransformMode - 1]);
	g_util.Logprintf("INFO:  ICC rendering intent is %s\r\n", rend[pJob->m_nICCRenderingIntent]);
	if (pJob->m_bUseICCProofProfile)
		Logprintf("INFO:  ICC proof rendering intent is %s\r\n", rend[pJob->m_nICCProofRenderingIntent]);
}
*/

int GetICCinfo(char *sICCfile, char *buf, int Intent, int UsedDirection)
{
	CString rend[4] = { "Perceptual", "Relative colorimetric", "Saturation", "Absolute colorimetric" };

	cmsHPROFILE hProfile = cmsOpenProfileFromFile(sICCfile, "r");

	TCHAR *tmp = buf;

	wchar_t wszBuffer[1024];
	TCHAR szBuffer[1024];
	cmsGetProfileInfo(hProfile, cmsInfoManufacturer, cmsNoLanguage, cmsNoCountry, wszBuffer, 1024);
	::WideCharToMultiByte(CP_ACP, 0, wszBuffer, -1, szBuffer, 1024, NULL, NULL);

	strcpy(tmp, "Product Name:  ");
	strcat(tmp, szBuffer);
	cmsGetProfileInfo(hProfile, cmsInfoDescription, cmsNoLanguage, cmsNoCountry, wszBuffer, 1024);
	::WideCharToMultiByte(CP_ACP, 0, wszBuffer, -1, szBuffer, 1024, NULL, NULL);
	strcat(tmp, "\r\n\r\nProduct Description:  ");
	strcat(tmp, szBuffer);

	strcat(tmp, "\r\n\r\nRendering intent supported:  ");

	BOOL bHasPerceptual = FALSE;
	BOOL bHasRelativeColormetric = FALSE;
	BOOL bHasAbsoluteColormetric = FALSE;
	BOOL bHasSaturation = FALSE;

	for (int i = LCMS_USED_AS_INPUT; i < LCMS_USED_AS_PROOF + 1; i++) {
		if (cmsIsIntentSupported(hProfile, INTENT_PERCEPTUAL, i))
			bHasPerceptual = TRUE;
		if (cmsIsIntentSupported(hProfile, INTENT_RELATIVE_COLORIMETRIC, i))
			bHasRelativeColormetric = TRUE;
		if (cmsIsIntentSupported(hProfile, INTENT_ABSOLUTE_COLORIMETRIC, i))
			bHasAbsoluteColormetric = TRUE;
		if (cmsIsIntentSupported(hProfile, INTENT_SATURATION, i))
			bHasSaturation = TRUE;
	}
	if (bHasPerceptual)
		strcat(tmp, "Perceptual");
	if (bHasRelativeColormetric)
		strcat(tmp, "  Relative Colormetric");
	if (bHasAbsoluteColormetric)
		strcat(tmp, "  Absolute Colormetric");
	if (bHasSaturation)
		strcat(tmp, "  Saturation");

	strcat(tmp, "\r\n\r\nColor space:  ");
	cmsColorSpaceSignature col = cmsGetColorSpace(hProfile);

	switch (col) {
	case cmsSigXYZData:	// 0x58595A20L
		strcat(tmp, "XYZ");
		break;
	case cmsSigLabData:  // 0x4C616220L
		strcat(tmp, "Lab");
		break;
	case cmsSigLuvData:	// 0x4C757620L
		strcat(tmp, "Luv");
		break;
	case cmsSigYCbCrData: // 0x59436272L
		strcat(tmp, "YCbr");
		break;
	case cmsSigYxyData:	// 0x59787920L
		strcat(tmp, "Yxy");
		break;

	case cmsSigRgbData:	// 0x52474220L
		strcat(tmp, "RGB");
		break;

	case cmsSigGrayData:	// 0x47524159L
		strcat(tmp, "Gray");
		break;

	case cmsSigHsvData:	// 0x48535620L
		strcat(tmp, "HSV");
		break;

	case cmsSigHlsData:	// 0x484C5320L
		strcat(tmp, "HLS");
		break;

	case cmsSigCmykData:	// 0x434D594BL
		strcat(tmp, "CMYK");
		break;

	case cmsSigCmyData:	// 0x434D5920L
		strcat(tmp, "CMY");
		break;

	case cmsSig2colorData:	// 0x32434C52L
		strcat(tmp, "2COL");
		break;

	case cmsSig3colorData:	// 0x33434C52L
		strcat(tmp, "3COL");
		break;

	case cmsSig4colorData:	// 0x34434C52L
		strcat(tmp, "4COL");
		break;

	case cmsSig5colorData:	// 0x35434C52L
		strcat(tmp, "5COL");
		break;
	case cmsSig6colorData:	// 0x36434C52L
		strcat(tmp, "6COL");
		break;

	case cmsSig7colorData:	// 0x37434C52L
		strcat(tmp, "7COL");
		break;

	case cmsSig8colorData:	// 0x38434C52L
		strcat(tmp, "8COL");
		break;

	case cmsSig9colorData:	// 0x39434C52L
		strcat(tmp, "9COL");
		break;

	case cmsSig10colorData:	// 0x41434C52L
		strcat(tmp, "ACOL");
		break;

	case cmsSig11colorData:	// 0x42434C52L
		strcat(tmp, "BCOL");
		break;

	case cmsSig12colorData:	// 0x43434C52L
		strcat(tmp, "CCOL");
		break;

	case cmsSig13colorData:	// 0x44434C52L
		strcat(tmp, "DCOL");
		break;

	case cmsSig14colorData:	// 0x45434C52L
		strcat(tmp, "ECOL");
		break;
	case cmsSig15colorData:	// 0x46434C52L
		strcat(tmp, "FCOL");
		break;
	}

	strcat(tmp, "\r\n\r\nProfile class:  ");

	cmsProfileClassSignature devcls = cmsGetDeviceClass(hProfile);
	switch (devcls) {
	case cmsSigInputClass:	// 0x73636E72L
		strcat(tmp, "Input/Screen");
		break;
	case cmsSigDisplayClass:	// 0x6D6E7472L
		strcat(tmp, "Display");
		break;
	case cmsSigOutputClass:	// 0x70727472L
		strcat(tmp, "Output/Printer");
		break;
	case cmsSigLinkClass:	// 0x6C696E6BL
		strcat(tmp, "Link");
		break;
	case cmsSigAbstractClass:	// 0x61627374L
		strcat(tmp, "Abstract");
		break;

	case cmsSigColorSpaceClass:	// 0x73706163L
		strcat(tmp, "Color Space");
		break;

	case cmsSigNamedColorClass:	// 0x6e6d636cL
		strcat(tmp, "Named Color");
		break;
	}
	strcat(tmp, "\r\n");

	int isValid = cmsIsIntentSupported(hProfile, Intent, UsedDirection);
	if (!isValid)
		strcat(tmp, "\r\n\r\nWARNING: CURRENT INTENT SETTING IS NOT SUPPORTED BY THIS PROFILE!\r\n");

	cmsCloseProfile(hProfile);

	return (isValid);
}


BYTE *ComputeICC2RGB(BYTE *pCMYKdata, DWORD tnw, DWORD tnh,
						CString sICCInputProfile,
						CString sICCMonitorProfile,
						CString sICCProofProfile,
						BOOL bUseICCProofProfile,
						int nICCRenderingIntent,
						int nICCProofRenderingIntent,
						BOOL bICCOutOfGamutAlarm, CString &sErrorMessage)
{
	g_util.LogprintfResample("INFO:  Processing ICC transform - CMYK->RGB in memory..\r\n");
	//LogICCmode(pJob);
	cmsUInt16Number Alarm[3] = { 0xDEAD, 0xBABE, 0xFACE };

	cmsSetLogErrorHandler(MyICCLogErrorHandler);

	cmsHPROFILE hInProfile = NULL;
	cmsHPROFILE hOutProfile = cmsOpenProfileFromFile(sICCMonitorProfile, "r");
	if (hOutProfile == NULL) {
		sErrorMessage = "Error opening monitor profile";
		return NULL;
	}
	cmsHPROFILE hProofProfile = NULL;
	cmsHTRANSFORM hTransform = NULL;

	int ICCTransformMode = ICC_INPUT_OUTPUT_TRANSFORM;
	if (sICCInputProfile == "")
		ICCTransformMode = ICC_DEVICE_LINK_TRANSFORM;
	if (bUseICCProofProfile)
		ICCTransformMode = ICC_INPUT_OUTPUT_PROOF_TRANSFORM;

	if (ICCTransformMode == ICC_INPUT_OUTPUT_TRANSFORM) {
		hInProfile = cmsOpenProfileFromFile(sICCInputProfile, "r");
		if (hInProfile == NULL) {
			sErrorMessage = _T("Error opening input profile");
			return NULL;
		}

		hTransform = cmsCreateTransform(hInProfile, TYPE_CMYK_8 /*TYPE_CMYK_8_PLANAR*/, hOutProfile, TYPE_RGB_8, nICCRenderingIntent, 0);
	}
	else if (ICCTransformMode == ICC_DEVICE_LINK_TRANSFORM)
		hTransform = cmsCreateTransform(hOutProfile, TYPE_CMYK_8 /*TYPE_CMYK_8_PLANAR*/, NULL, TYPE_RGB_8, nICCRenderingIntent, 0);
	else { // ICC_INPUT_OUTPUT_PROOF_TRANSFORM 
		hInProfile = cmsOpenProfileFromFile(sICCInputProfile, "r");
		if (hInProfile == NULL) {
			g_util.LogprintfResample("Error opening input profile");
			return NULL;
		}
		hProofProfile = cmsOpenProfileFromFile(sICCProofProfile, "r");
		if (hProofProfile == NULL) {
			cmsCloseProfile(hInProfile);
			g_util.LogprintfResample("Error opening proof profile");
			return NULL;
		}
		hTransform = cmsCreateProofingTransform(hInProfile, TYPE_CMYK_8 /*TYPE_CMYK_8_PLANAR*/,
			hOutProfile, TYPE_RGB_8,
			hProofProfile, nICCRenderingIntent, nICCProofRenderingIntent, 0);
		if (bICCOutOfGamutAlarm)
			cmsSetAlarmCodes(Alarm);
	}

	if (!hTransform) {
		g_util.LogprintfResample("Aborting ICC profile processing due to error\r\n");
		cmsCloseProfile(hInProfile);
		cmsCloseProfile(hOutProfile);
		if (ICCTransformMode == ICC_INPUT_OUTPUT_PROOF_TRANSFORM)
			cmsCloseProfile(hProofProfile);
		return FALSE;
	}

	BYTE *pOutLine = new BYTE[tnw * 3];
	BYTE *pRGB = new BYTE[tnw*tnh * 3];
	if (pOutLine == NULL || pRGB == NULL)
		return NULL;

	BYTE *dst = pRGB;
	BYTE *src = pCMYKdata;

	for (DWORD y = 0; y < tnh; y++) {

		// interleaved input and output
		cmsDoTransform(hTransform, src, pOutLine, tnw);

		memcpy(dst, pOutLine, tnw * 3);
		dst += tnw * 3;
		src += tnw * 4;
	}

	delete pOutLine;


	cmsDeleteTransform(hTransform);
	if (ICCTransformMode != ICC_DEVICE_LINK_TRANSFORM)
		cmsCloseProfile(hInProfile);
	cmsCloseProfile(hOutProfile);
	if (ICCTransformMode == ICC_INPUT_OUTPUT_PROOF_TRANSFORM)
		cmsCloseProfile(hProofProfile);

	g_util.LogprintfResample("INFO:  ICC-Processing done\r\n");
	return pRGB;

}

