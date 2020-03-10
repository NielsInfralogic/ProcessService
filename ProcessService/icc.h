#pragma once
#include "Stdafx.h"

#define ICC_INPUT_OUTPUT_TRANSFORM			1
#define ICC_DEVICE_LINK_TRANSFORM			2
#define ICC_INPUT_OUTPUT_PROOF_TRANSFORM	3

#define ICC_INTENT_PERCEPTUAL				0
#define ICC_INTENT_RELATIVE_COLORIMETRIC	1
#define ICC_INTENT_SATURATION				2
#define ICC_INTENT_ABSOLUTE_COLORIMETRIC	3


BYTE *ComputeICC2RGB(BYTE *pCMYKdata, DWORD tnw, DWORD tnh,
	CString sICCInputProfile,
	CString sICCMonitorProfile,
	CString sICCProofProfile,
	BOOL bUseICCProofProfile,
	int nICCRenderingIntent,
	int nICCProofRenderingIntent,
	BOOL bICCOutOfGamutAlarm, CString &sErrorMessage);

int GetICCinfo(char *sICCfile, char *buf, int Intent, int UsedDirection);

