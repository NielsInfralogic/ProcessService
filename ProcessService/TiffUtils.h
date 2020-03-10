#pragma once
#include "Stdafx.h"

BOOL	CreateJPEGs(CString sFileName, CString sPreviewName, CString sThumbnailName, int jpegquality, int nOrgResolution, int nPreviewResolution, int nThumbnailResolution);
BOOL	CreateJPEGsFromBuffer(BYTE *pData, int tnw, int tnh, int type, CString sPreviewName, 
							  CString sThumbnailName, int jpegquality, int nOrgResolution, int nPreviewResolution, int nThumbnailResolution, BOOL bFilter, CString sTitle);
BYTE	*CreateBMP(void *thumbnail, int width, int height, int type, char *BMPpath);

BYTE *Rotate180RGB(BYTE *buf, int BytesPerLine, int NumberOfLines);
BYTE *RotateRGB90(BYTE *buf, int *pBytesPerLine, int *pNumberOfLines, BOOL Rotate270);
void MirrorRGB(BYTE *buf, int BytesPerLine, int NumberOfLines);
void InvertRGB(BYTE *buf, int BytesPerLine, int NumberOfLines);
BYTE *ApplyBoundingBox(BYTE *thumbnail, int *tnw, int *tnh, int Type, double fPrevres, double fBoundingBoxSizeX, double fBoundingBoxSizeY, double fBoundingBoxPosX, double fBoundingBoxPosY);
BYTE *ReadJPEG(char *filename, DWORD *tnw, DWORD *tnh);
BOOL CreateJPEG(BYTE *image, DWORD tnw, DWORD tnh,  CString filename, int jpegquality);
int	CreateJPEG2(BYTE *thumbnail, int tnw, int tnh, int type, char *filename, int jpegquality, CString sComment);
int	CreateJPEG3(BYTE *thumbnail, int tnw, int tnh, int type, char *filename, int jpegquality, CString sComment, int resolution);

int SaveTiledJpeg(CString filePath, BYTE *image, DWORD tnw, DWORD tnh, int level, int tilesize);
BOOL SavePyramideTileJpeg(CString sFileName);
BYTE *ReadTiff(TCHAR *filename, int *tnw, int *tnh, int *type, int *res);
BYTE *ReadTiffAndCropMargin(TCHAR *filename, int *tnw, int *tnh, int *type, double margin);

BOOL	CompareTiff(CString filename1, CString filename2, double fPageFormatWidth, double fPageFormatHeight);

BOOL MirrorInvertShiftTiff(CString sFullInputPath, CString sTempOuputPath, BOOL bMirror, BOOL bInvert, BOOL bShifBits);

BOOL IsTiffInverted(CString filename);
BOOL ShiftTiff(CString sFullInputPath, CString sTempOuputPath, int nShiftBits);
BOOL ShiftTiffEx(CString sFullInputPath, CString sTempOuputPath, int nShiftBits);
BOOL ShiftTiffVertical(CString sFullInputPath, CString sTempOuputPath);
BYTE  *CMYKInterleavedToRGB(BYTE *cmykthumbnail, DWORD tnw, DWORD tnh);
BOOL CMYKInterleaved2Separated(BYTE *cmykimage, BYTE *c_buf, BYTE *m_buf, BYTE *y_buf, BYTE *k_buf, DWORD tnw, DWORD tnh);
BYTE  *GenerateHeatMap(BYTE *c_thumbnail, BYTE *m_thumbnail, BYTE *y_thumbnail, BYTE *k_thumbnail, DWORD tnw, DWORD tnh);

int	CreateSingleColorJPEG(BYTE *thumbnail, int tnw, int tnh, int coloridx, char *filename, int jpegquality, CString sComment, int resolution);
BYTE	*DownSampleCT(BYTE *highresbuf, DWORD tnw, DWORD tnh, int colorfile, DWORD *thumbnailwidth, DWORD *thumbnailheight, int prevres, int thres, int jpeg);
BOOL AddCommentToJPEG(CString sFileName, CString sTitle);
class CTiffUtils
{
public:
	CTiffUtils(void);
	~CTiffUtils(void);

	BOOL	TiffInfo(CString sFileName, float *Xres, float *Yres, 
		float *Width, float *Height, BOOL *bBlackIsMin, DWORD *dwPixels, DWORD *dwLines, DWORD *dwBytesPerLine);

	WORD	DIBNumColors(LPSTR lpDIB);
	void	Rotate180MonoMirror(BYTE *buf, DWORD BytesPerLine, DWORD NumberOfLines);
	BOOL	Tiff2BMP(CString sTiffName, CString sBMPName, float previewResolution);
	BYTE	*CMYK2RGB(BYTE *c_thumbnail, BYTE *m_thumbnail, BYTE *y_thumbnail, BYTE *k_thumbnail, DWORD tnw, DWORD tnh);
	void	Mirror(BYTE *pBytes, DWORD nWidth, DWORD nHeight);
	void	Invert(BYTE *pBytes, DWORD nBytes);
	BOOL	TiffNotEmpty(CString  Name);

	


};
