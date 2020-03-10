#include "StdAfx.h"
#include <math.h>
#include "tiffio.h"
#include "tiffutils.h"
#include "jpeglib.h"
#include "2PassScale.h"
#include "Prefs.h"
#include "ProofThread.h"
#include <setjmp.h>
#include "Utils.h"

extern CPrefs g_prefs;
extern CUtils g_util;

#define RGBBLANKPIXEL	255



static void MyErrorHandler(const char* module, const char* fmt, va_list ap)
{
	;
}

static void MyWarningHandler(const char* module, const char* fmt, va_list ap)
{
	;
}

static	DWORD bitsspecial[256];	
CTiffUtils::CTiffUtils(void)
{
	// Redirect TIFF library errors to local handler
	TIFFSetErrorHandler((TIFFErrorHandler) MyErrorHandler);
	TIFFSetWarningHandler((TIFFErrorHandler) MyWarningHandler);

	// Generate bit count table for downsampler
	for (int i = 0; i < 256; i++) {
		BYTE n = 0;
		if (i&0x0001) n++;
		if (i&0x0002) n++;
		if (i&0x0004) n++;
		if (i&0x0008) n++;
		if (i&0x0010) n++;
		if (i&0x0020) n++;
		if (i&0x0040) n++;
		if (i&0x0080) n++;
		bitsspecial[i] = (unsigned int)n;
	}
}

CTiffUtils::~CTiffUtils(void)
{
}

BOOL TiffInfo1(TCHAR *szFileName, float *Xres, float *Yres, float *Width, float *Height, BOOL *bBlackIsMin, DWORD *dwPixels, DWORD *dwLines, DWORD *dwBytesPerLine)
{

	TIFF	 *tif;
	UINT	dwGetWidth, dwGetHeight;
	WORD	res_unit, photometric;

	*Height = 0.0;
	*Width = 0.0;
	*dwPixels = 0;
	*dwLines = 0;
	*dwBytesPerLine = 0;


	__try {
		if ((tif = TIFFOpen(szFileName, "rm")) == NULL)
			return FALSE;
		TIFFGetField(tif, TIFFTAG_XRESOLUTION, Xres);
		TIFFGetField(tif, TIFFTAG_YRESOLUTION, Yres);
		TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit);
		if (res_unit == RESUNIT_CENTIMETER) {
			*Xres *= (float)2.54;
			*Yres *= (float)2.54;
		}
		if (*Xres <= 100 || *Yres <= 100) {
			TIFFClose(tif);
			Sleep(500);
			if ((tif = TIFFOpen(szFileName, "rm")) == NULL)
				return FALSE;
			TIFFGetField(tif, TIFFTAG_XRESOLUTION, Xres);
			TIFFGetField(tif, TIFFTAG_YRESOLUTION, Yres);
			TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit);
			if (res_unit == RESUNIT_CENTIMETER) {
				*Xres *= (float)2.54;
				*Yres *= (float)2.54;
			}
		}

		//dwGetWidth = TIFFScanlineSize(tif)*8;
		TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &dwGetWidth);
		TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &dwGetHeight);
		TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
		
		TIFFClose(tif);

		*bBlackIsMin = (photometric == PHOTOMETRIC_MINISBLACK) ? TRUE : FALSE;

		DWORD	bpl = TIFFScanlineSize(tif);


		if (*Xres)
			*Width = (float)dwGetWidth / *Xres;
		

		if (*Yres)
			*Height = (float)dwGetHeight / *Yres;


		*dwPixels = dwGetWidth;
		*dwLines = dwGetHeight;
		*dwBytesPerLine = bpl;
	
	}
	__except (EXCEPTION_EXECUTE_HANDLER) {
        return 0;
    }

	return TRUE;
}


BOOL CTiffUtils::TiffInfo(CString sFileName, float *Xres, float *Yres, float *Width, float *Height, BOOL *bBlackIsMin, DWORD *dwPixels, DWORD *dwLines, DWORD *dwBytesPerLine)
{
	TCHAR szFileName[MAX_PATH];
	strcpy(szFileName, sFileName);

	BOOL ret = TiffInfo1(szFileName, Xres, Yres, Width, Height, bBlackIsMin, dwPixels, dwLines, dwBytesPerLine);

	return ret;
}

#define IS_WIN30_DIB(lpbi)  ((*(LPDWORD)(lpbi)) == sizeof(BITMAPINFOHEADER))
//#define CVT(x)      (((x) * 255L) / ((1L<<16)-1))

WORD CTiffUtils::DIBNumColors(LPSTR lpDIB)
{
   WORD wBitCount;  // DIB bit count

   /*  If this is a Windows-style DIB, the number of colors in the
    *  color table can be less than the number of bits per pixel
    *  allows for (i.e. lpbi->biClrUsed can be set to some value).
    *  If this is the case, return the appropriate value.
    */

   if (IS_WIN30_DIB(lpDIB))
   {
      DWORD dwClrUsed;

      dwClrUsed = ((LPBITMAPINFOHEADER)lpDIB)->biClrUsed;
      if (dwClrUsed)
     return (WORD)dwClrUsed;
   }

   /*  Calculate the number of colors in the color table based on
    *  the number of bits per pixel for the DIB.
    */
   if (IS_WIN30_DIB(lpDIB))
      wBitCount = ((LPBITMAPINFOHEADER)lpDIB)->biBitCount;
   else
      wBitCount = ((LPBITMAPCOREHEADER)lpDIB)->bcBitCount;

   /* return number of colors based on bits per pixel */
   switch (wBitCount)
      {
   case 1:
      return 2;

   case 4:
      return 16;

   case 8:
      return 256;

   default:
      return 0;
      }
}

void CTiffUtils::Rotate180MonoMirror(BYTE *buf, DWORD BytesPerLine, DWORD NumberOfLines)
{
	DWORD dstptr = (DWORD)buf + (NumberOfLines - 1) * BytesPerLine;
		
	__asm {
		mov	esi, DWORD PTR buf
		mov	edi, DWORD PTR dstptr
		mov edx, DWORD PTR BytesPerLine	 
		mov	ebx, DWORD PTR NumberOfLines
		shr ebx, 1
	$monorot180mirloop1:			
		xor ecx, ecx
	$monorot180mirloop2:			
		mov al, BYTE PTR [esi+ecx]			// swap 
		mov ah, BYTE PTR [edi+ecx]
		mov BYTE PTR [esi+ecx],ah
		mov BYTE PTR [edi+ecx],al
		inc ecx
		cmp ecx,edx
		jne $monorot180mirloop2
		add esi, edx
		sub edi, edx
		dec ebx
		jnz $monorot180mirloop1		
		and NumberOfLines,1
		jz $monorot180exit
		xor ecx,ecx
	$monorot180mirloop3:			
		mov al, BYTE PTR [esi+ecx]			
		mov ah, BYTE PTR [esi+ecx]		
		mov BYTE PTR [esi+ecx],ah		
		mov BYTE PTR [esi+ecx],al			
		inc ecx
		cmp ecx,edx
		jne $monorot180mirloop3
	$monorot180exit:
	}
}

BOOL CTiffUtils::Tiff2BMP(CString sTiffName, CString sBMPName, float previewResolution)
{
	TIFF		*tif;
	WORD		res_unit, photometric;
	float		xres;
	DWORD		sh, s=0;
	BYTE		b;
	DWORD		rps = 256;	//!!

	if ((tif = TIFFOpen(sTiffName, "rm")) == NULL)
		return FALSE;
	TIFFGetField(tif, TIFFTAG_XRESOLUTION, &xres);
	TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit);
	if (res_unit == RESUNIT_CENTIMETER) 
		xres *= (float)2.54;

	if (xres <= 100) {
		TIFFClose(tif);
		Sleep(500);
		if ((tif = TIFFOpen(sTiffName, "rm")) == NULL)
			return FALSE;
		TIFFGetField(tif, TIFFTAG_XRESOLUTION, &xres);
		if (res_unit == RESUNIT_CENTIMETER) 
			xres *= (float)2.54;
		if (xres <= 100) {
			TIFFClose(tif);
			return FALSE;
		}
	}

	DWORD	bpl = TIFFScanlineSize(tif);
	DWORD	sw = bpl*8;
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &sh);
	TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);
	BOOL bBlackIsMin = (photometric == PHOTOMETRIC_MINISBLACK) ? TRUE : FALSE;

	
	// Do the downsampling
	BYTE		*rp, *thumbPtr, *readinptr;
	int			rows = 0;
    DWORD		padding = 0, rowsinthisstrip;

	DWORD	tnw = (int)previewResolution * sw / xres + 0.5;
	DWORD	tnh	= (int)previewResolution * sh / xres + 0.5;
	DWORD	boxheight = ceil((double) sw / (double) tnw) + 1;
	DWORD	stripbufsize = (rps + boxheight * 2) * (bpl+1);
	
	BYTE *pStripbuf =  new BYTE[stripbufsize];
	if (pStripbuf == NULL) {
		TIFFClose(tif);
		return FALSE;
	}
	rp = pStripbuf;
	if (tnw % 4) {
		padding = 4 - tnw % 4;
		tnw += padding;
	}

	BYTE *pPreview = new BYTE [tnw * tnh];
	if (pPreview == NULL) {
		TIFFClose(tif);
		delete [] pStripbuf;
		return FALSE;
	}

	BYTE	*prt = pPreview;
	BYTE	*HorizontalBitSteppingStart = (BYTE *)malloc(tnw);
	BYTE	*HorizontalBitSteppingMiddle = (BYTE *)malloc(tnw);
	BYTE	*HorizontalBitSteppingEnd = (BYTE *)malloc(tnw);
	BYTE	*HorizontalBitSteppingLength = (BYTE *)malloc(tnw);
	DWORD	*SteppingRowOffset = (DWORD *)malloc(tnw*sizeof(DWORD));
	register int filterw;
	// Calculate downsample ratio
	WORD filterWidth = (WORD) ceil((double) sw / (double) tnw);

	// Calculate the horizontal accumulation parameteres
	// according to the widths of the src and dst images.
	int step = sw;
	int limit = tnw;
	int err = 0;
	register DWORD sx = 0;
	for (int x = 0; x < tnw-padding; x++) {
		register DWORD sx0 = sx;
		err += step;
		while (err >= limit) {
			err -= limit;
			sx++;
		}
		SteppingRowOffset[x] = (WORD)(sx0 >> 3);
		filterw = sx - sx0;		// width 
		HorizontalBitSteppingLength[x] = filterw;
		b = (filterw < 8) ? 0xff<<(8-filterw) : 0xff;
		HorizontalBitSteppingStart[x] = b >> (sx0&7);
		filterw -= 8 - (sx0&7);
		if (filterw < 0)
			filterw = 0;
		HorizontalBitSteppingMiddle[x] = filterw >> 3;
		filterw -= (filterw>>3)<<3;
		HorizontalBitSteppingEnd[x] = 0xff << (8-filterw);
	}
	step = sh;
	limit = tnh;
	err = 0;
	DWORD	sy = 0;  
	thumbPtr = pPreview;

	for (DWORD dy = 0; dy < tnh; dy++) {
	
		int nrows = 1;
		err += step;
		while (err >= limit) {
			err -= limit;
			sy++;
			if (err >= limit)
				nrows++;
		}

		// Read nrows from tiff file
		readinptr = rp = pStripbuf;
		rowsinthisstrip	= (rows + nrows <= sh) ? nrows : sh - rows;

		// Now read the image data from the buffer
		for (int j=0; j<rowsinthisstrip;j++) 
			TIFFReadScanline(tif, readinptr+j*bpl, s++, 0);

		rows += rowsinthisstrip;
					
		DWORD area;// = nrows * filterWidth;

		for (DWORD x = 0; x < tnw - padding; x++) {
			DWORD StepMaskStart = HorizontalBitSteppingStart[x];
			DWORD filterw = HorizontalBitSteppingMiddle[x];
			DWORD StepMaskEnd = HorizontalBitSteppingEnd[x];
			DWORD off = SteppingRowOffset[x];
			area = nrows * HorizontalBitSteppingLength[x];
			//DWORD acc = 0;

			// Step through the boxheight lines 
			//DWORD ii;
			DWORD y = 0;
			//for (DWORD y = 0; y < nrows; y++) {
			__asm {
				xor eax, eax					// y
				mov DWORD PTR y, eax	
				xor edx, edx				    // edx = acc
				lea edi, DWORD PTR bitsspecial  // edi = bits[]
				mov ebx, DWORD PTR bpl		    // ebx = bpl
				mov esi, DWORD PTR rp
				add esi, DWORD PTR off
				sub esi,ebx
				push esi
			$yloop:
				pop esi
				add esi, ebx				// esi = SrcPtr = rp + y * bpl + off
				push esi
				movzx eax, BYTE PTR [esi]
				inc esi
				and eax, DWORD PTR StepMaskStart
				add edx, DWORD PTR [edi+4*eax]
				
				mov ecx, DWORD PTR filterw
				cmp ecx, 0
				jz $endfilterwloop
			$filterwloop:  
				movzx eax,BYTE PTR [esi]
				inc esi
				add edx, DWORD PTR [edi+4*eax]
				loop $filterwloop

			$endfilterwloop:
				movzx eax, BYTE PTR [esi]
				and eax, DWORD PTR StepMaskEnd
				add edx, DWORD PTR [edi+4*eax]				 
				
				inc DWORD PTR y
				mov eax, DWORD PTR y
				cmp eax, DWORD PTR nrows
				jne $yloop
				
				mov eax, 255
				mul edx
			 	xor edx, edx
				mov ebx, DWORD PTR area
				div ebx
				mov ebx, DWORD PTR thumbPtr
				mov BYTE PTR [ebx], al
				inc DWORD PTR thumbPtr
				pop esi
			}
		}
		
		rp += nrows * bpl;
		switch (padding) {
			case 3:
				*thumbPtr++ = 0;
			case 2:
				*thumbPtr++ = 0;
			case 1:
				*thumbPtr++ = 0;
			case 0:
				break;
		}
	}
	
	free(SteppingRowOffset);
	free(HorizontalBitSteppingLength);
	free(HorizontalBitSteppingEnd);
	free(HorizontalBitSteppingMiddle);
	free(HorizontalBitSteppingStart);

	//PhotometricAdjust(photometric);

	delete [] pStripbuf;

	TIFFClose(tif);

	// Write the BMP file


	Rotate180MonoMirror(pPreview, tnw, tnh);
	
	CFile				bmpfile;
	BITMAPFILEHEADER	bif;		// Bitmap file header
	BITMAPINFOHEADER	bi;			// Bitmap header
	WORD				wBitCount = 8;
//	RGBQUAD *colortable;
	int colortablesize = 0;
	// Initialize BITMAPFILEHEADER
	bif.bfType = (WORD)('B' + 'M' * 256); 

	// initialize BITMAPINFOHEADER
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = tnw;// + padding;         // fill in width from parameter
	bi.biHeight = tnh;       // fill in height from parameter
	bi.biPlanes = 1;              // must be 1
	bi.biBitCount = wBitCount;    // from parameter
	bi.biCompression = BI_RGB;    
	bi.biSizeImage = tnh * (tnw * (wBitCount>>3)/* + padding*/);
	bi.biXPelsPerMeter = 2834; //0;
	bi.biYPelsPerMeter = 2834; //0;
	bi.biClrUsed = 256;
	bi.biClrImportant = 0;
    
	if (IS_WIN30_DIB (&bi))
		colortablesize =  (DIBNumColors((LPSTR)&bi) * sizeof(RGBQUAD));
	else
		colortablesize =  (DIBNumColors((LPSTR)&bi) * sizeof(RGBTRIPLE));

	RGBQUAD *colortable = new RGBQUAD [colortablesize/4];
	for (int i = 0; i< colortablesize/4; i++) {
		colortable[i].rgbRed = colortable[i].rgbGreen = colortable[i].rgbBlue = 255-i;
		colortable[i].rgbReserved = 0;
	}
	    
	//bif.bfOffBits = (DWORD)sizeof(BITMAPINFOHEADER) + colortablesize;
	//bif.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + bif.bfOffBits + bi.biSizeImage); 
	bif.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + colortablesize;
	bif.bfSize = (DWORD)(bif.bfOffBits + bi.biSizeImage); 

	bmpfile.Open(sBMPName, CFile::modeCreate | CFile::modeWrite, 0);

	bmpfile.Write((void *)&bif, sizeof(BITMAPFILEHEADER));
	bmpfile.Write((void *)&bi, sizeof(BITMAPINFOHEADER));
	bmpfile.Write((void *)colortable, colortablesize);
		
	delete [] colortable;

	bmpfile.Write(pPreview, bi.biSizeImage);
	bmpfile.Close();

	return TRUE;
}

BYTE  *CTiffUtils::CMYK2RGB(BYTE *c_thumbnail, BYTE *m_thumbnail, BYTE *y_thumbnail, BYTE *k_thumbnail, DWORD tnw, DWORD tnh)
{
    BYTE	*c_th, *m_th, *y_th, *k_th, *th;
	DWORD	padding = 0;

	BYTE *emptybuf = new BYTE[tnw*tnh];
	memset(emptybuf,0x00,tnw*tnh);

	BYTE *thumbnail = new BYTE[4*tnw*tnh];
	if (thumbnail == NULL) {
		return NULL;
	}
	c_th = c_thumbnail != NULL ? c_thumbnail : emptybuf;
	m_th = m_thumbnail != NULL ? m_thumbnail : emptybuf;
	y_th = y_thumbnail != NULL ? y_thumbnail : emptybuf;
	k_th = k_thumbnail != NULL ? k_thumbnail : emptybuf;
	th = thumbnail;
	DWORD nPixels = tnw*tnh;

	__asm {
		mov ecx, 255
		mov	edi, DWORD PTR thumbnail
	$mergeloop:
		mov esi, DWORD PTR k_th
		mov ebx, 255				// k_tmp = 255 - k
		sub bl, BYTE PTR [esi]
		cmp ebx, 0					// if k == 0 -> All black
		jne $notallblack			
		mov BYTE PTR [edi], 0
		mov BYTE PTR [edi+1], 0
		mov BYTE PTR [edi+2], 0
		add edi, 3
		jmp $tonext
	$notallblack:
		mov esi, DWORD PTR y_th		// c_th
		mov eax, 255			
		sub al, BYTE PTR [esi]		// c_tmp = 255 - c
		mul ebx						// r = c_tmp * k_tmp / 255
		xor edx,edx					
		div ecx
		mov BYTE PTR [edi],al		// Store R
		inc edi

		mov esi, DWORD PTR m_th	
		mov eax, 255				// m_tmp = 255 - m
		sub al, BYTE PTR [esi]		// g = m_tmp * k_tmp / 255
		mul ebx
		xor edx,edx
		div ecx
		mov BYTE PTR [edi],al		// Store G
		inc edi

		mov esi, DWORD PTR c_th		// NAN y_th
		mov eax, 255
		sub al, BYTE PTR [esi]		// y_tmp = 255 - y
		mul ebx						// b = y_tmp * k_tmp / 255
		xor edx,edx
		div ecx
		mov BYTE PTR [edi],al		// Store B
		inc edi
		
	$tonext:
		mov BYTE PTR [edi],0		// Store alpha
		inc edi
		inc DWORD PTR c_th
		inc DWORD PTR m_th
		inc DWORD PTR y_th
		inc DWORD PTR k_th
		dec DWORD PTR nPixels
		cmp DWORD PTR nPixels,0
		jnz $mergeloop
	}

	delete[] emptybuf;

	return thumbnail;
}

void CTiffUtils::Invert(BYTE *pBytes, DWORD nBytes)
{
__asm {
		mov esi, DWORD PTR pBytes
		mov	ecx, DWORD PTR nBytes
		mov	ebx, 255
	$invmonoloop:
		mov	eax, ebx
		sub al, BYTE PTR [esi]
		mov BYTE PTR [esi], al
		inc esi
		loop $invmonoloop
	}
}

void CTiffUtils::Mirror(BYTE *pBytes, DWORD nWidth, DWORD nHeight)
{
__asm {
		mov	edi, DWORD PTR pBytes
		mov esi, DWORD PTR nWidth	 
		shr esi, 1						// half a line in bytes
	$monomirloop1:
		xor eax,eax						// left	pos
		mov ebx, DWORD PTR nWidth	 
		dec ebx							// right pos
		xor ecx,ecx
		xor edx,edx
	$monomirloop2:
		mov cl, BYTE PTR [edi+eax]	// cl = *(bptr+left)
		mov	ch, BYTE PTR [edi+ebx]	// ch = *(bptr+right)
		mov	BYTE PTR [edi+eax],ch	
		mov BYTE PTR [edi+ebx],cl		// swap
		inc eax		 					// left++
		dec ebx							// right--
		cmp eax, esi
		jl $monomirloop2
		add edi, DWORD PTR nWidth	 
		dec DWORD PTR nHeight
		jnz	$monomirloop1
	}
}

BOOL CTiffUtils::TiffNotEmpty(CString  sName)
{
	TIFF	*tif;
	BYTE	*buf;
	DWORD	row = 0, h, w, i;
	DWORD	rowsperstrip;
	float	xres;
	WORD	res_unit, photometric;
	tstrip_t strip = 0;

	if ((tif = TIFFOpen(sName, "rm")) == NULL) {	
		return FALSE;
	}

	TIFFGetField(tif, TIFFTAG_XRESOLUTION, &xres);
	TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit);
	if (res_unit == RESUNIT_CENTIMETER) 
		xres *= (float)2.54;


	if (xres <= 100) {
		TIFFClose(tif);
		Sleep(500);
		if ((tif = TIFFOpen(sName, "rm")) == NULL) {	
			return FALSE;
		}

		TIFFGetField(tif, TIFFTAG_XRESOLUTION, &xres);
		TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit);
		if (res_unit == RESUNIT_CENTIMETER) 
			xres *= (float)2.54;
		if (xres <= 100) {
			TIFFClose(tif);
			return FALSE;
		}

	}

	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
	TIFFGetField(tif, TIFFTAG_PHOTOMETRIC, &photometric);

	tsize_t scanline = TIFFScanlineSize(tif);
	int ss = TIFFStripSize(tif);
	try {
		buf = new BYTE[ss];
	}
	catch (CMemoryException *ex) {
		TIFFClose(tif);
		return FALSE;
	}
	
	while (row < h) {
		DWORD nrow = (row+rowsperstrip > h) ? h-row : rowsperstrip;
		ss = nrow*scanline;
		//tstrip_t strip = TIFFComputeStrip(tif, row, 0);

		if (TIFFReadEncodedStrip(tif, strip, buf, ss) < 0) {
			delete[] buf;
			TIFFClose(tif);
			return FALSE;
		} 
		row += nrow;
		strip++;
		BYTE *p = buf;
		if (photometric == PHOTOMETRIC_MINISWHITE) {
			for (i=0; i<ss; i++) {
				if (*p++){			// Found data !
					TIFFClose(tif);
					delete[] buf;
					return TRUE;
				}
			}
		} else {
			for (i=0; i<ss; i++) {
				if (*p++ != 0xff)	{		// Found data !
					TIFFClose(tif);
					delete[] buf;
					return TRUE;
				}
			}
		}
	}

	// No data found - file is emtpy
	TIFFClose(tif);
	delete[] buf;
	return FALSE;		
}




BYTE *ScaleTo (BYTE *m_pBits, DWORD m_dwWidth, DWORD m_dwHeight, DWORD dwNewWidth, DWORD dwNewHeight)
{
	C2PassScale <CCubicFilter> ScaleEngine;
	BYTE *pOldBitmap = m_pBits;
	m_pBits = ScaleEngine.AllocAndScale(m_pBits, m_dwWidth,m_dwHeight,dwNewWidth,dwNewHeight);
	                                    
	if (NULL == m_pBits) {
		_tprintf("ERROR: Scale Engine failed!\r\n");
		return FALSE;
	}
	m_dwWidth = dwNewWidth;
	m_dwHeight = dwNewHeight;
	delete pOldBitmap;

	return m_pBits;
}

////////////////////////// GLOBALS SO WE CAN ALLOCATE ONCE PER JOB ONLY!!!

// Box filter weight arrays
int		*HiiLeft, *HiiRight, *ViiLeft, *ViiRight;;
DWORD	**HiiWeights,  **ViiWeights;
int		Hboxfilterlength, Vboxfilterlength;
int		*rc1arr[4], *rc2arr[4];
double	*frc1arr[4], *frc2arr[4];
int		*i_x_arr, *i_y_arr;
double	*f_x_arr, *f_y_arr;

int CalcContributions (DWORD uLineSize, DWORD uSrcSize, double dScale, int *iiLeft[], int *iiRight[], DWORD **iiWeights[], int *boxfilterlength)
{ 
 	//CBilinearFilter *pCurFilter = new CBilinearFilter();
	//CGaussianFilter *pCurFilter = new CGaussianFilter();
	CBsplineFilter *pCurFilter = new CBsplineFilter(); //CCubicFilter();

	double dWidth;
    double dFScale = 1.0;
    double dFilterWidth = pCurFilter->GetWidth();

    if (dScale < 1.0) {    
		// Minification
        dWidth = dFilterWidth / dScale; 
        dFScale = dScale; 
    } else {   
		// Magnification
        dWidth= dFilterWidth; 
    }
 
    // Window size is the number of sampled pixels
    int iWindowSize = 2 * (int)ceil(dWidth) + 1; 
 
    // Allocate a new line contributions strucutre
	*boxfilterlength = uLineSize;
	int *iiiLeft = new int [uLineSize];
	int *iiiRight = new int [uLineSize];	
	DWORD **tPtr = (DWORD **) new unsigned int* [uLineSize];
	*iiWeights = tPtr;

	double * pWeights = new double[iWindowSize];
	
    for (DWORD u = 0 ; u < uLineSize ; u++) {
        // Allocate contributions for every pixel
		DWORD *up =  new DWORD [iWindowSize];
		tPtr[u] = up;
     // Scan through line of contributions
        double dCenter = (double)u / dScale;   // Reverse mapping
        // Find the significant edge points that affect the pixel

        int iLeft = max (0, (int)floor (dCenter - dWidth)); 
        int iRight = min ((int)ceil (dCenter + dWidth), int(uSrcSize) - 1); 
 
        // Cut edge points to fit in filter window in case of spill-off
		if (iRight - iLeft + 1 > iWindowSize) {
			if (iLeft < (int(uSrcSize) - 1 / 2)) { 
                iLeft++; 
            } else {
                iRight--; 
            }
        }
 		iiiLeft[u] = iLeft;
		iiiRight[u] = iRight;
        double dTotalWeight = 0.0;  // Zero sum of weights
		for (int iSrc = iLeft; iSrc <= iRight; iSrc++) {
            // Calculate weights
			pWeights[iSrc-iLeft] = dFScale * pCurFilter->Filter (dFScale * (dCenter - (double)iSrc));
			dTotalWeight += pWeights[iSrc-iLeft];
        }
        ASSERT (dTotalWeight >= 0.0);   // An error in the filter function can cause this 
		if (dTotalWeight > 0.0) {
            // Normalize weight of neighbouring points
			for (int iSrc = iLeft; iSrc <= iRight; iSrc++) {
                // Normalize point
				pWeights[iSrc-iLeft]  /= dTotalWeight; 
				up[iSrc-iLeft] = pWeights[iSrc-iLeft] * 10000.0;
			}
		}

   } 
   delete [] pWeights;
   
   *iiLeft = iiiLeft;
   *iiRight = iiiRight;

   //return res; 
   return TRUE;
}


BYTE *ScaleToFast(BYTE *m_pBits, DWORD m_dwWidth, DWORD m_dwHeight, DWORD dwNewWidth, DWORD dwNewHeight)
{
//	C2PassScale <CCubicFilter> ScaleEngine;

	BYTE *pOldBitmap = m_pBits;
	BYTE *m_pNewBits = new BYTE [dwNewWidth*dwNewHeight];
	if (NULL == m_pNewBits) {
		return FALSE;
	}
	memset(m_pNewBits,0x00,dwNewWidth*dwNewHeight);
	
	double scale_factor = (double)dwNewWidth/(double)m_dwWidth;
		
	// Allocate and calculate the contributions

	CalcContributions (dwNewWidth, m_dwWidth, (double)dwNewWidth/(double)m_dwWidth, &HiiLeft, &HiiRight, &HiiWeights, &Hboxfilterlength); 
	CalcContributions (dwNewHeight, m_dwHeight, double(dwNewHeight) / double(m_dwHeight), &ViiLeft, &ViiRight, &ViiWeights, &Vboxfilterlength); 
	
	// Scale source image horizontally into temporary image
	BYTE *pTemp = new BYTE [(dwNewWidth+1) * (m_dwHeight+1)];
	memset(pTemp,0x00,(dwNewWidth+1) * (m_dwHeight+1));
	BYTE *th = pTemp;
	BYTE *srcline = m_pBits;
	for (DWORD y = 0; y < m_dwHeight; y++) {
		DWORD x = 0;
		__asm {
			xor eax,eax
		$outer:		
			shl eax,2
			mov edi, DWORD PTR HiiLeft
			mov ebx, DWORD PTR [edi+eax]
			mov edi, DWORD PTR HiiRight
			mov ecx, DWORD PTR [edi+eax]
			inc ecx
			sub ecx, ebx			// subtract iLeft

			mov esi, DWORD PTR HiiWeights	
			mov	edi, DWORD PTR [esi+eax]	// p
			
			mov esi, DWORD PTR srcline	
			add esi, ebx
			xor ebx, ebx				// b
		$iloop:
			movzx edx, BYTE PTR [esi]	// *(ctline+i)
			mov eax, DWORD PTR [edi]	// *(p)
			mul edx						
			add ebx, eax																				
			inc esi
			add edi,4
			loop $iloop

			mov	eax, ebx
			mov ebx, 10000
			xor edx, edx
			div ebx
			mov edi, DWORD PTR th
			mov BYTE PTR [edi], al
			inc edi
			mov DWORD PTR th,edi
			inc DWORD PTR x
			mov eax, DWORD PTR x
			cmp eax, DWORD PTR dwNewWidth
			jne $outer
		
		}
		srcline += m_dwWidth;
	}
	
	// Scale temporary image vertically into result image    
	
	th = m_pNewBits;
	for (int x = 0; x < dwNewWidth; x++) {
		// Step through columns
		DWORD y = 0;
		DWORD z = 0;
		//for (DWORD y = 0, z = 0; y < tnh; y++, z+=tnw) {    // Loop through column
		__asm {
			xor eax,eax
		$outery2:
			shl eax,2
			mov edi, DWORD PTR ViiLeft
			mov ebx, DWORD PTR [edi+eax]
			mov edi, DWORD PTR ViiRight

			mov ecx, DWORD PTR [edi+eax] // iRight	
			inc ecx
			sub ecx, ebx	// i

			mov esi, DWORD PTR ViiWeights
			mov	edi, DWORD PTR [esi+eax]	// p
			
			mov esi, DWORD PTR pTemp
			mov eax, DWORD PTR dwNewWidth
			mul ebx
			add esi, eax				
			add esi, DWORD PTR x		// pTemp+k+x
			xor ebx, ebx				// b
		$iloop2:
			movzx edx, BYTE PTR [esi]	// *(pTemp+k+x)
			mov eax, DWORD PTR [edi]	// *(p)
			mul edx						
			add ebx, eax																				
			add esi, DWORD PTR dwNewWidth
			add edi,4
			loop $iloop2

			mov	eax, ebx
			mov ebx, 10000
			xor edx, edx
			div ebx
			mov edi, DWORD PTR th
			add edi, DWORD PTR z
			add edi, DWORD PTR x
			mov BYTE PTR [edi], al

			inc DWORD PTR y
			mov ebx, DWORD PTR  dwNewWidth
			add DWORD PTR z, ebx
			mov eax, DWORD PTR y
			cmp eax, DWORD PTR dwNewHeight
			jne $outery2
		}
	}
	for (DWORD i = 0; i < Hboxfilterlength; i++) {
		// Free contribs for every pixel
		delete [] HiiWeights[i];
	}
	delete [] HiiRight;
	delete [] HiiLeft;	
	delete [] HiiWeights;

	for (DWORD i = 0; i < Vboxfilterlength; i++) {
		// Free contribs for every pixel
		delete [] ViiWeights[i];
	}
	delete [] ViiRight;
	delete [] ViiLeft;	
	delete [] ViiWeights;
	
	delete [] pTemp;		

	
	m_dwWidth = dwNewWidth;
	m_dwHeight = dwNewHeight;
//	delete pOldBitmap;

	return m_pNewBits;
}

BYTE	*DownSampleCT(BYTE *highresbuf, DWORD tnw, DWORD tnh, int colorfile, DWORD *thumbnailwidth, DWORD *thumbnailheight,int prevres, int thres, int jpeg)
{
	int nobytes, thwidth;
	int ratio, ratiosquare;
	int	linetoskiptop=0, bytestoskipleft=0;

	DWORD x,y,ix,iy, sum,rsum, gsum, bsum, thx,thy;
	//DWORD vertratio, horzratio;

	if (thres) {
		ratio = (double)prevres / (double)thres;
		*thumbnailwidth = tnw / ratio;
		*thumbnailheight = tnh / ratio;
	} else {
		double f1 = (*thumbnailwidth);
		double f2 = (*thumbnailheight);
		double fratiox = (double)tnw / f1 + 0.5;
		double fratioy = tnh / f2 + 0.5;
		int ratiox = fratiox;
		int ratioy = fratioy;

		ratio = max(ratiox, ratioy);
		
		if (tnw/ratio > (*thumbnailwidth) || tnh/ratio > (*thumbnailheight))
			ratio++;
		
		linetoskiptop = (ratioy<ratio) ? (*thumbnailheight-tnh/ratio)/2 : 0;
		bytestoskipleft = (ratiox<ratio) ? (*thumbnailwidth-tnw/ratio)/2 * colorfile : 0;

	}
		
	ratiosquare = ratio*ratio;

	nobytes = (*thumbnailwidth+ratio)*(*thumbnailheight+ratio)*colorfile;
	
	thwidth = *thumbnailwidth * colorfile;

	BYTE *thumbnail = new BYTE[nobytes];
	if (thumbnail == NULL) {

		return (NULL);
	}	
	BYTE *th = thumbnail;
	
	if (colorfile == 1) {
		FillMemory(thumbnail, nobytes, 0x00);
		for (y=0, thy=linetoskiptop;y<tnh-ratio; y+= ratio, thy++) {
			for (x=0, thx=bytestoskipleft;x<tnw-ratio; x+= ratio,thx++) {
				sum = 0;
				for (ix = x; ix<x+ratio; ix++)
					for (iy = y; iy<y+ratio; iy++)
						sum += *(highresbuf + iy*tnw + ix);
				*(th+thwidth*thy + thx) = sum / ratiosquare;
			}
		}
	} else if (colorfile == 3 ) {
		FillMemory(thumbnail, nobytes, jpeg ? 0xFF : 0x00);
		for (y=0, thy=linetoskiptop;y<tnh-ratio; y+= ratio, thy++) {
			for (x=0, thx=bytestoskipleft;x<(tnw-ratio)*3; x+= ratio*3, thx+=3) {
				rsum = 0, gsum = 0, bsum = 0;
				for (ix = x; ix<x+ratio*3; ix+=3)
					for (iy = y; iy<y+ratio; iy++) {
						rsum += *(highresbuf + iy*tnw*3 + ix);
						gsum += *(highresbuf + iy*tnw*3 + ix + 1);
						bsum += *(highresbuf + iy*tnw*3 + ix + 2);
					}
				*(th + thwidth*thy + thx) = rsum / ratiosquare;
				*(th + thwidth*thy + thx + 1) = gsum / ratiosquare;
				*(th + thwidth*thy + thx + 2) = bsum / ratiosquare;
			}
		}
	} else {	// CMYK
		FillMemory(thumbnail, nobytes, jpeg ? 0xFF : 0x00);
		for (y=0, thy=linetoskiptop;y<tnh-ratio; y+= ratio, thy++) {
			for (x=0, thx=bytestoskipleft;x<(tnw-ratio)*4; x+= ratio*4, thx+=4) {
				rsum = 0, gsum = 0, bsum = 0;
				for (ix = x; ix<x+ratio*4; ix+=4)
					for (iy = y; iy<y+ratio; iy++) {
						rsum += *(highresbuf + iy*tnw*4 + ix);
						gsum += *(highresbuf + iy*tnw*4 + ix + 1);
						bsum += *(highresbuf + iy*tnw*4 + ix + 2);
			 			bsum += *(highresbuf + iy*tnw*4 + ix + 3);
					}
				*(th + thwidth*thy + thx) = rsum / ratiosquare;
				*(th + thwidth*thy + thx + 1) = gsum / ratiosquare;
				*(th + thwidth*thy + thx + 2) = bsum / ratiosquare;
				*(th + thwidth*thy + thx + 3) = bsum / ratiosquare;
			}
		}
	}
	return thumbnail;
}


BYTE *ReadJPEG(TCHAR *filename, int *tnw, int *tnh, int *type);
BYTE* CombinePlanes(DWORD tnw, DWORD tnh, BYTE *pRedData, BYTE *pGreenData, BYTE *pBlueData);
void SplitInterleaved(BYTE *pColorData, DWORD tnw, DWORD tnh, BYTE *pRedData, BYTE *pGreenData, BYTE *pBlueData);
#define CMYKFILE 4
#define COLORFILE 3
#define MONOFILE 1

BYTE	*CreateBMP(void *thumbnail, int width, int height, int type, char *BMPpath);

BOOL	CreateJPEGs(CString sFileName, CString sPreviewName, CString sThumbnailName, int jpegquality, int nOrgResolution, int nPreviewResolution, int nThumbnailResolution)
{
	TCHAR filename[MAX_PATH];
	strcpy(filename,sFileName);
	int tnw, tnh, type, res;
	BYTE *pData = ReadTiff(filename, &tnw, &tnh, &type, &res);
	if (pData == NULL)
		return FALSE;

	double ratio = (double)nPreviewResolution/(double)nOrgResolution;
	DWORD jpegpreviewwidth = (double)tnw*ratio;
	DWORD jpegpreviewheight = (double)tnh*ratio;

	ratio = (double)nThumbnailResolution/(double)nOrgResolution;
	DWORD jpegthumbnailwidth = (double)tnw*ratio;
	DWORD jpegthumbnailheight = (double)tnh*ratio;
	
	BOOL ret = TRUE;
	BYTE *pPreviewData;

	if (type == 1) {
		
		pPreviewData = ScaleToFast(pData, tnw, tnh, jpegpreviewwidth, jpegpreviewheight);

		delete pData;
	} else {
		DWORD n = tnw*tnh;
		BYTE *pChannelData = new BYTE[n];

		BYTE *pOrg = pData;
		BYTE *pCh = pChannelData;
		for (int i=0; i<n; i++) {
			*pCh++ = *pOrg++;
			pOrg++;
			pOrg++;
		}
		BYTE *pPreviewDataRed = ScaleToFast(pChannelData, tnw, tnh, jpegpreviewwidth, jpegpreviewheight);
			
		pOrg = pData;
		pCh = pChannelData;
		for (int i=0; i<n; i++) {
			pOrg++;
			*pCh++ = *pOrg++;
			pOrg++;
		}
		BYTE *pPreviewDataGreen = ScaleToFast(pChannelData, tnw, tnh, jpegpreviewwidth, jpegpreviewheight);
			
		pOrg = pData;
		pCh = pChannelData;
		for (int i=0; i<n; i++) {
			pOrg++;
			pOrg++;
			*pCh++ = *pOrg++;
			
		}
		delete pData;
		BYTE *pPreviewDataBlue = ScaleToFast(pChannelData, tnw, tnh, jpegpreviewwidth, jpegpreviewheight);

		delete [] pChannelData;
		n = jpegpreviewwidth*jpegpreviewheight;
		pPreviewData = new BYTE [3*n];

		BYTE *pR = pPreviewDataRed;
		BYTE *pG = pPreviewDataGreen;
		BYTE *pB = pPreviewDataBlue;
		pOrg = pPreviewData;
		for (int i=0; i<n; i++) {
			*pOrg++ = *pR++;
			*pOrg++ = *pG++;
			*pOrg++ = *pB++; 
		}
		delete pPreviewDataRed;
		delete pPreviewDataGreen;
		delete pPreviewDataBlue;
		
	
	}

	ret = CreateJPEG2(pPreviewData, jpegpreviewwidth, jpegpreviewheight, type, sPreviewName.GetBuffer(255), jpegquality, "");

	sPreviewName.ReleaseBuffer();
	if (ret && nThumbnailResolution>0) {
		BYTE *pThumbnailData = DownSampleCT(pPreviewData, jpegpreviewwidth, jpegpreviewheight, type, &jpegthumbnailwidth, &jpegthumbnailheight, nPreviewResolution, nThumbnailResolution, TRUE);
		delete pPreviewData;
		ret = CreateJPEG2(pThumbnailData, jpegthumbnailwidth, jpegthumbnailheight, type, sThumbnailName.GetBuffer(255), 100, "");
		sThumbnailName.ReleaseBuffer();
		if (ret) {
			TCHAR bmpPath[MAX_PATH];
			sprintf(bmpPath,"%s\\th.bmp", (LPCSTR)g_util.GetTempFolder());
			pThumbnailData = CreateBMP(pThumbnailData, jpegthumbnailwidth,jpegthumbnailheight, type ,bmpPath);
		}
		delete pThumbnailData;
	} else
		delete pPreviewData;


	DeleteFile( sFileName);

	return ret;
}

BOOL	CreateJPEGsFromBuffer(BYTE *pData, int tnw, int tnh, int type, CString sPreviewName, 
							  CString sThumbnailName, int jpegquality, int nOrgResolution, int nPreviewResolution, 
	int nThumbnailResolution, BOOL bFilter, CString sTitle)
{
	if (pData == NULL)
		return FALSE;

	double ratio = (double)nPreviewResolution/(double)nOrgResolution;
	DWORD jpegpreviewwidth = (double)tnw*ratio;
	DWORD jpegpreviewheight = (double)tnh*ratio;

	double ratio2 = (double)nThumbnailResolution/(double)nOrgResolution;
	DWORD jpegthumbnailwidth = (double)tnw*ratio2;
	DWORD jpegthumbnailheight = (double)tnh*ratio2;
	
	BOOL ret = TRUE;
	BYTE *pPreviewData = NULL;

	if (ratio == 1.0) 
		pPreviewData = pData;
	else {

		switch (type) {
			case 1: // mono
				pPreviewData = ScaleToFast(pData, tnw, tnh, jpegpreviewwidth, jpegpreviewheight);
				delete pData;			
				break;
			case 3:
				{
				DWORD n = tnw*tnh;
				BYTE *pChannelData = new BYTE[n];

				BYTE *pOrg = pData;
				BYTE *pCh = pChannelData;
				for (int i=0; i<n; i++) {
					*pCh++ = *pOrg++;
					pOrg++;
					pOrg++;
				}

				BYTE *pPreviewDataRed = ScaleToFast(pChannelData, tnw, tnh, jpegpreviewwidth, jpegpreviewheight);

				pOrg = pData;
				pCh = pChannelData;
				for (int i=0; i<n; i++) {
					pOrg++;
					*pCh++ = *pOrg++;
					pOrg++;
				}
				BYTE *pPreviewDataGreen = ScaleToFast(pChannelData, tnw, tnh, jpegpreviewwidth, jpegpreviewheight);
	
				pOrg = pData;
				pCh = pChannelData;
				for (int i=0; i<n; i++) {
					pOrg++;
					pOrg++;
					*pCh++ = *pOrg++;
					
				}
				delete pData;
				BYTE *pPreviewDataBlue = ScaleToFast(pChannelData, tnw, tnh, jpegpreviewwidth, jpegpreviewheight);
			

				delete [] pChannelData;
				n = jpegpreviewwidth*jpegpreviewheight;
				pPreviewData = new BYTE [3*n];

				BYTE *pR = pPreviewDataRed;
				BYTE *pG = pPreviewDataGreen;
				BYTE *pB = pPreviewDataBlue;
				pOrg = pPreviewData;
				for (int i=0; i<n; i++) {
					*pOrg++ = *pR++;
					*pOrg++ = *pG++;
					*pOrg++ = *pB++; 
				}
				delete pPreviewDataRed;
				delete pPreviewDataGreen;
				delete pPreviewDataBlue;
				break;
			}

		}
	}

	ret = CreateJPEG2(pPreviewData, jpegpreviewwidth, jpegpreviewheight, type, sPreviewName.GetBuffer(255), jpegquality, sTitle);

	sPreviewName.ReleaseBuffer();
	if (ret && nThumbnailResolution>0) {
		BYTE *pThumbnailData = DownSampleCT(pPreviewData, jpegpreviewwidth, jpegpreviewheight, type, &jpegthumbnailwidth, &jpegthumbnailheight, nPreviewResolution, nThumbnailResolution, TRUE);
		delete pPreviewData;
		ret = CreateJPEG2(pThumbnailData, jpegthumbnailwidth, jpegthumbnailheight, type, sThumbnailName.GetBuffer(255), 100,"");
		sThumbnailName.ReleaseBuffer();
		if (ret) {
			TCHAR bmpPath[MAX_PATH];
			sprintf(bmpPath,"%s\\th.bmp",(LPCSTR)g_util.GetTempFolder());
			pThumbnailData = CreateBMP(pThumbnailData, jpegthumbnailwidth,jpegthumbnailheight, type ,bmpPath);
		}
		delete pThumbnailData;
	} else
		delete pPreviewData;



	return ret;
}

int	CreateSingleColorJPEG(BYTE *thumbnail, int tnw, int tnh, int coloridx, char *filename, int jpegquality, CString sComment, int resolution)
{
	BYTE *thnew;

//	Logprintf("INFO: color %d, coloridx %d\r\n", color, coloridx);

	int cc=0, mm=0,yy=0,kk=100;
	try {
		thnew = new BYTE[tnw*tnh*3];
	}
	catch (CMemoryException *ex) {
		return FALSE;
	}

	BYTE *pSource = thumbnail;

	switch (coloridx) {
		
		case 0: //Cyan = RGB(0,255,255)
			for (int i=0;i<tnw*tnh*3;i+=3) {
				*(thnew+i) = 255-*pSource++;
				*(thnew+i+1) = 255;
				*(thnew+i+2) =  255;
			}

			break;
		case 1: //Magenta = RGB(255,0,255)
			for (int i=0;i<tnw*tnh*3;i+=3) {
				*(thnew+i) = 255;
				*(thnew+i+1) = 255-*pSource++;
				*(thnew+i+2) = 255;
			}
			break;
		case 2: //Yellow = RGB(255,0,255)
			for (int i=0;i<tnw*tnh*3;i+=3) {
				*(thnew+i) = 255;
				*(thnew+i+1) = 255; 
				*(thnew+i+2) = 255-*pSource++;
			}
			break;
		case 3: //Black = RGB(255,0,255)
			for (int i=0;i<tnw*tnh*3;i+=3) {
				*(thnew+i) = 255-*pSource;
				*(thnew+i+1) = 255-*pSource;
				*(thnew+i+2) = 255-*pSource++;
			}
			break; 
	}

	int ret =  CreateJPEG3(thnew, tnw, tnh, COLORFILE, filename, jpegquality,  sComment, resolution);

	delete thnew;
	return ret;						
}


BOOL AddCommentToJPEG(CString  sFileName, CString sTitle)
{
	int tnw = 0;
	int tnh = 0;
	int type = 3;
	
	TCHAR szFileName[MAX_PATH];
	strcpy(szFileName, sFileName);

	g_util.LogprintfResample("DEBUG: Trying to read jpeg  file %s..", szFileName);

	BYTE *pImage = ReadJPEG(szFileName, &tnw, &tnh, &type);
	if (pImage == NULL)
		return FALSE;

	g_util.LogprintfResample("DEBUG: AddCommentToJPEG() - Read in JPEG file %s", szFileName);
	
	CString sBackupCopyFileName = sFileName + _T(".tmp");
	::CopyFile(sFileName, sBackupCopyFileName, FALSE);
	::DeleteFile(sFileName);
	
	g_util.LogprintfResample("DEBUG: AddCommentToJPEG() - Creating jpeg %s with comment %s", szFileName, sTitle);
	if (CreateJPEG2(pImage, tnw, tnh, type, szFileName, 100, sTitle) == FALSE) {
		g_util.LogprintfResample("ERROR: CreateJPEG2 failed");
		::CopyFile(sBackupCopyFileName, sFileName, FALSE);
		::DeleteFile(sBackupCopyFileName);
		return FALSE;
	}
	
	g_util.LogprintfResample("DEBUG: CreateJPEG2 OK");
	delete pImage;

	return TRUE;

}

int	CreateJPEG2(BYTE *thumbnail, int tnw, int tnh, int type, char *filename, int jpegquality, CString sComment)
{
	TCHAR szComment[80];
	strcpy(szComment, sComment);

	void *pthumbnail = thumbnail;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	FILE * outfile;	
		
	if ((outfile = fopen(filename, "wb")) == NULL) {
	   // Logprintf("Can't open %s\n", filename);
		return (FALSE);
	}
	jpeg_stdio_dest(&cinfo, outfile);
		
	// Set jpeg info parameters
	cinfo.image_width = tnw;												// Width of image, in pixels
	cinfo.image_height = tnh;												// Height of image, in pixels
	cinfo.input_components = (type == COLORFILE) ? 3 : 1;					// Number of color channels (samples per pixel)
	cinfo.in_color_space = (type == COLORFILE) ? JCS_RGB : JCS_GRAYSCALE; 	// Color space of source image
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality (&cinfo, jpegquality, TRUE);
	//if (jpegprogression)
	//	jpeg_simple_progression (&cinfo);


	jpeg_start_compress(&cinfo, TRUE);
	if (strlen(szComment) > 0)
		jpeg_write_marker(&cinfo, JPEG_COM, (JOCTET *)szComment, strlen(szComment));

	JSAMPROW row_pointer[1];	/* pointer to a single row */
	int row_stride;			/* physical row width in buffer */

	if (type == MONOFILE) {
		// invert grayscale image
		int n = tnw * tnh;
		__asm {
			mov esi, DWORD PTR thumbnail
			mov	ecx, DWORD PTR n
			mov	ebx, 255
		$invloop:
			mov	eax, ebx
			sub	al, BYTE PTR [esi]
			mov BYTE PTR [esi], al
			inc esi
			loop $invloop
		}
	}

	row_stride = tnw * ((type == COLORFILE) ? 3 : 1);	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & thumbnail[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
  //  jpeg_write_scanlines(&cinfo, (BYTE *)thumbnail, tnh);

	jpeg_finish_compress(&cinfo);


	jpeg_create_compress(&cinfo);
	
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);



	if (type == MONOFILE) {
		// invert grayscale image back again
		int n = tnw * tnh;
		__asm {
			mov esi, DWORD PTR thumbnail
			mov	ecx, DWORD PTR n
			mov	ebx, 255
		$invloop2:
			mov	eax, ebx
			sub	al, BYTE PTR [esi]
			mov BYTE PTR [esi], al
			inc esi
			loop $invloop2
		}
	}

	return TRUE;
}

int	CreateJPEG3(BYTE *thumbnail, int tnw, int tnh, int type, char *filename, int jpegquality, CString sComment, int resolution)
{
	TCHAR szComment[80];
	strcpy(szComment, sComment);

	void *pthumbnail = thumbnail;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	FILE * outfile;	
		
	if ((outfile = fopen(filename, "wb")) == NULL) {
	   // Logprintf("Can't open %s\n", filename);
		return (FALSE);
	}
	jpeg_stdio_dest(&cinfo, outfile);
		
	// Set jpeg info parameters
	cinfo.image_width = tnw;												// Width of image, in pixels
	cinfo.image_height = tnh;												// Height of image, in pixels
	cinfo.input_components = (type == COLORFILE) ? 3 : 1;					// Number of color channels (samples per pixel)
	cinfo.in_color_space = (type == COLORFILE) ? JCS_RGB : JCS_GRAYSCALE; 	// Color space of source image
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality (&cinfo, jpegquality, TRUE);
	//if (jpegprogression)
	//	jpeg_simple_progression (&cinfo);

	if (resolution > 0) {
		cinfo.X_density = (int)resolution;
		cinfo.Y_density = (int)resolution;
		cinfo.density_unit = 1;
		cinfo.write_JFIF_header = TRUE;
	}

	jpeg_start_compress(&cinfo, TRUE);
	if (strlen(szComment) > 0)
		jpeg_write_marker(&cinfo, JPEG_COM, (JOCTET *)szComment, strlen(szComment));

	JSAMPROW row_pointer[1];	/* pointer to a single row */
	int row_stride;			/* physical row width in buffer */

	if (type == MONOFILE) {
		// invert grayscale image
		int n = tnw * tnh;
		__asm {
			mov esi, DWORD PTR thumbnail
			mov	ecx, DWORD PTR n
			mov	ebx, 255
		$invloop:
			mov	eax, ebx
			sub	al, BYTE PTR [esi]
			mov BYTE PTR [esi], al
			inc esi
			loop $invloop
		}
	}

	row_stride = tnw * ((type == COLORFILE) ? 3 : 1);	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & thumbnail[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
  //  jpeg_write_scanlines(&cinfo, (BYTE *)thumbnail, tnh);

	jpeg_finish_compress(&cinfo);


	jpeg_create_compress(&cinfo);
	
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);



	if (type == MONOFILE) {
		// invert grayscale image back again
		int n = tnw * tnh;
		__asm {
			mov esi, DWORD PTR thumbnail
			mov	ecx, DWORD PTR n
			mov	ebx, 255
		$invloop2:
			mov	eax, ebx
			sub	al, BYTE PTR [esi]
			mov BYTE PTR [esi], al
			inc esi
			loop $invloop2
		}
	}

	return TRUE;
}


BYTE *ReadTiff(TCHAR *filename, int *tnw, int *tnh, int *type, int *res)
{
	TIFF	*tif;
	short	spp;
	DWORD	rowsperstrip;
	float	Xres;
	WORD res_unit;

	if ((tif = TIFFOpen(filename, "rm")) == NULL) {
		return NULL;
	}
	int w,h;
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
	TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
	TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
	TIFFGetField(tif, TIFFTAG_XRESOLUTION, &Xres);
	TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit);
	if (res_unit == RESUNIT_CENTIMETER) 
		Xres *= (float)2.54;
	
	*res = (int)Xres;

	int ss = TIFFStripSize(tif);

	BYTE *buf = NULL;
	try {
		buf = new BYTE[(w+7)*h*spp];
	}
	catch (CMemoryException *ex) {
		TIFFClose(tif);
		return NULL;
	}

	BYTE *p = buf;
	DWORD	bpl = TIFFScanlineSize(tif);
	int s = 0;
	for (int j=0; j<h; j++) 
		TIFFReadScanline(tif, buf+j*bpl, s++, 0);
	TIFFClose(tif);
	*type = spp; 
	*tnw = w;
	*tnh = h;

	return buf;
}

// Read 1/16 of the file and determine polarity
BOOL IsTiffInverted(CString filename)
{
	TIFF	*tif;
	short	spp;
	DWORD	rowsperstrip;

	if ((tif = TIFFOpen(filename, "rm")) == NULL) {
		return NULL;
	}
	int w,h;
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
	 TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
	 TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);

	h /= 16;
	int ss = TIFFStripSize(tif);

	DWORD	bpl = TIFFScanlineSize(tif);
	
	BYTE *buf = NULL;
	try {
		buf = new BYTE[bpl*h];
	}
	catch (CMemoryException *ex) {
		TIFFClose(tif);
		return NULL;
	}

	BYTE *p = buf;
	
	int s = 0;
	for (int j=0; j<h; j++) 
		TIFFReadScanline(tif, buf+j*bpl, s++, 0);
	TIFFClose(tif);

	DWORD nBytesAllBlack = 0;
	DWORD nBytesAllWhite = 0;
	
	for (int i=0; i<h*bpl;i++) {
		if (*p == 0x00)
			nBytesAllWhite++;
		else if (*p == 0xFF)
			nBytesAllBlack++;
		p++;
	}

	delete buf;
	return nBytesAllBlack> nBytesAllWhite;
}

// define bitwise bytemirror table for mirroring output
static unsigned char mirror[256] = { 0, 128, 64, 192, 32, 160, 96, 224, 16,
						144, 80, 208, 48, 176, 112, 240, 8,
						136, 72, 200, 40, 168, 104, 232, 24,
						152, 88, 216, 56, 184, 120, 248, 4,
						132, 68, 196, 36, 164, 100, 228, 20,
						148, 84, 212, 52, 180, 116, 244, 12,
						140, 76, 204, 44, 172, 108, 236, 28,
						156, 92, 220, 60, 188, 124, 252, 2,
						130, 66, 194, 34, 162, 98, 226, 18,
						146, 82, 210, 50, 178, 114, 242, 10,
						138, 74, 202, 42, 170, 106, 234, 26,
						154, 90, 218, 58, 186, 122, 250, 6,
						134, 70, 198, 38, 166, 102, 230, 22,
						150, 86, 214, 54, 182, 118, 246, 14,
						142, 78, 206, 46, 174, 110, 238, 30,
						158, 94, 222, 62, 190, 126, 254, 1,
						129, 65, 193, 33, 161, 97, 225, 17,
						145, 81, 209, 49, 177, 113, 241, 9,
						137, 73, 201, 41, 169, 105, 233, 25,
						153, 89, 217, 57, 185, 121, 249, 5,
						133, 69, 197, 37, 165, 101, 229, 21,
						149, 85, 213, 53, 181, 117, 245, 13,
						141, 77, 205, 45, 173, 109, 237, 29,
						157, 93, 221, 61, 189, 125, 253, 3,
						131, 67, 195, 35, 163, 99, 227, 19,
						147, 83, 211, 51, 179, 115, 243, 11,
						139, 75, 203, 43, 171, 107, 235, 27,
						155, 91, 219, 59, 187, 123, 251, 7,
						135, 71, 199, 39, 167, 103, 231, 23,
						151, 87, 215, 55, 183, 119, 247, 15,
						143, 79, 207, 47, 175, 111, 239, 31,
						159, 95, 223, 63, 191, 127, 255};

void MirrorBlock(BYTE *buf, int BytesPerLine, int NumberOfLines)
{
	int	stopcond = BytesPerLine >> 1;
	int middlebyte = (BytesPerLine & 0x00000001);
	__asm {
		mov	edi, DWORD PTR buf
		lea	esi, DWORD PTR mirror
	  $ppmirloop1:
		xor eax,eax					// left	
		mov ebx,DWORD PTR BytesPerLine	
		dec ebx						// right
		xor ecx,ecx
		xor edx,edx
	  $ppmirloop2:
		mov cl, BYTE PTR [edi+eax]	// ecx = *(bptr+left)
		mov cl, BYTE PTR [esi+ecx]	// ecx = mirror[ecx]
		mov	dl, BYTE PTR [edi+ebx]	// edx = *(bptr+right)
		mov dl, BYTE PTR [esi+edx]	// edx = mirror[edx]
		mov	BYTE PTR [edi+eax],dl	
		mov BYTE PTR [edi+ebx],cl	// swap
		inc	eax						// left++
		dec ebx						// right--
		cmp eax, DWORD PTR stopcond
		jne $ppmirloop2
		cmp DWORD PTR middlebyte,0
		je $ppmirjmp3
		mov cl,BYTE PTR [edi+eax]
		mov	cl,BYTE PTR [esi+ecx]
		mov	BYTE PTR [edi+eax],cl
	  $ppmirjmp3:
		add edi, DWORD PTR BytesPerLine
		dec DWORD PTR NumberOfLines
		jnz	$ppmirloop1
	}
}

BOOL MirrorInvertShiftTiff(CString sFullInputPath, CString sTempOuputPath, BOOL bMirror, BOOL bInvert, BOOL bShiftBits) 
{
	TIFF *tif, *tifout;
	float imxres,imyres;
	WORD	bps,spp,plc,photo,resunit;
	DWORD imw, iml;
	time_t ltime;
	struct tm *today;

	if ((tif = TIFFOpen(sFullInputPath, "r")) == NULL) {
		return FALSE;
	}

	TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imw);
	TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&iml);
	TIFFGetField(tif,TIFFTAG_RESOLUTIONUNIT,&resunit);
	TIFFGetField(tif,TIFFTAG_XRESOLUTION,&imxres);
	TIFFGetField(tif,TIFFTAG_YRESOLUTION,&imyres);
	if (resunit == RESUNIT_CENTIMETER) {
		imxres *= (float)2.54;
		imyres *= (float)2.54;
	}
	DWORD bpl = TIFFScanlineSize(tif);

	BYTE *linebuf = new BYTE [bpl];
	char tempstring[100];

	// output file

	if((tifout = TIFFOpen(sTempOuputPath,"wm"))==NULL) {
		delete[] linebuf;
		TIFFClose(tif);
		return FALSE;
	}

	TIFFSetField(tifout, TIFFTAG_SUBFILETYPE,0x0);
	TIFFSetField(tifout, TIFFTAG_IMAGEWIDTH,(DWORD)imw);
	TIFFSetField(tifout, TIFFTAG_IMAGELENGTH,(DWORD)iml);

	TIFFSetField(tifout, TIFFTAG_BITSPERSAMPLE, 1);
	TIFFSetField(tifout, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(tifout, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tifout, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);

	TIFFSetField(tifout, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);
	sprintf(tempstring, "TiffInvert");
	TIFFSetField(tifout, TIFFTAG_IMAGEDESCRIPTION, tempstring);
	sprintf(tempstring, "InfraLogic");
	TIFFSetField(tifout, TIFFTAG_ARTIST, tempstring);
	sprintf(tempstring, "TiffInvert");
	TIFFSetField(tifout, TIFFTAG_SOFTWARE, tempstring);
	TIFFSetField(tifout, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tifout,TIFFTAG_XRESOLUTION, (float)imxres);
	TIFFSetField(tifout,TIFFTAG_YRESOLUTION, (float)imyres);
	TIFFSetField(tifout, TIFFTAG_ROWSPERSTRIP, (DWORD)1);

	time(&ltime);
	today = localtime(&ltime);
	strftime(tempstring,20,"%Y:%m:%d:%H:%M:%S",today);
	TIFFSetField(tif, TIFFTAG_DATETIME, tempstring);

	int paddingbits = bpl*8 - imw;
	BYTE byteToAnd = 0xFF << paddingbits;
	


	g_util.LogprintfResample("INFO: Padding bits = %d",paddingbits);


	for (int i=0; i<iml; i++) {
		if (TIFFReadScanline(tif, linebuf, i,0)==-1) {
			TIFFClose(tif);
			TIFFClose(tifout);
			delete[] linebuf;
			DeleteFile(sTempOuputPath);
			return FALSE;
		}	


		if (bInvert) {
			__asm {
				mov ecx, DWORD PTR bpl
				dec ecx
				mov ebx,ecx
				mov edi, DWORD PTR linebuf
			$invloop:
				not BYTE PTR [edi+ecx]
				dec ecx
				jns $invloop

//				mov al, BYTE PTR byteToAnd
//				mov dl,BYTE PTR [edi+ebx]
//				and dl,al
//				mov BYTE PTR [edi+ebx], dl
			}

			BYTE xx = *(linebuf+bpl-1);
			xx &= byteToAnd;
			*(linebuf+bpl-1) = xx;

		}

		if (bMirror)
			MirrorBlock(linebuf, bpl, 1);

		if (bShiftBits && paddingbits > 0) {

			int bitsToShift = paddingbits;
			int bitsToShift2 = 32 - bitsToShift;
			int bitsToShift3 = 8 - bitsToShift;

			//int dwordstoshift = BasicTiff.m_bpl/4;
			int dwordstoshift = 0;
			int restbytestoshift = bpl - 4 * dwordstoshift;
			BYTE *p = (BYTE *)linebuf;

			DWORD mask1 = 0xFFFFFFFF << bitsToShift;				// 0xFFFF FFFC
			DWORD mask1inv = ~mask1;								// 0x0000 0003

			DWORD lopart = 0x00000000;
			DWORD hipart = 0x00000000;

			for (int ecx=0; ecx<restbytestoshift; ecx++) {
				BYTE bt = *p;
				lopart = bt & mask1inv;
				bt >>= bitsToShift;
				bt |= hipart;
				*p = bt;
				hipart = lopart << bitsToShift3;
				p++;
			}
			lopart = 0x00000000;
			hipart = 0x00000000;
		}


		if (TIFFWriteScanline(tifout, linebuf, i, 0) < 0) {
			TIFFClose(tif);
			TIFFClose(tifout);
			delete[] linebuf;
			DeleteFile(sTempOuputPath);
			return FALSE;
		}
	}
	TIFFClose(tif);
	TIFFClose(tifout);
	delete[] linebuf;

	return TRUE;
}

// See if any data in last word of each scanline
BOOL HasDataInPaddingZone(CString sFullInputPath)
{
	TIFF *tif;
	float imxres,imyres;
	WORD	bps,spp,plc,photo,resunit;
	DWORD imw, iml;
	time_t ltime;
	struct tm *today;

	if ((tif = TIFFOpen(sFullInputPath, "rm")) == NULL) {
		return FALSE;
	}

	TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imw);
	TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&iml);
	TIFFGetField(tif,TIFFTAG_RESOLUTIONUNIT,&resunit);
	TIFFGetField(tif,TIFFTAG_XRESOLUTION,&imxres);
	TIFFGetField(tif,TIFFTAG_YRESOLUTION,&imyres);
	if (resunit == RESUNIT_CENTIMETER) {
		imxres *= (float)2.54;
		imyres *= (float)2.54;
	}
	DWORD bpl = TIFFScanlineSize(tif);
	BYTE *linebuf = new BYTE [bpl];

	BOOL bDataFound = FALSE;
	for (int i=0; i<iml; i++) {
		if (TIFFReadScanline(tif, linebuf, i,0)==-1) {
			TIFFClose(tif);
			delete[] linebuf;
			return -1;
		}	

		if (linebuf[bpl-1] != 0 || linebuf[bpl-2] != 0 || linebuf[bpl-3] != 0 || linebuf[bpl-4] != 0) {
			bDataFound = TRUE;
			break;
		}
	}

	TIFFClose(tif);

	delete[] linebuf;

	return bDataFound;

}

BOOL ShiftTiff(CString sFullInputPath, CString sTempOuputPath, int nShiftBits) 
{
	if (g_prefs.m_usefullbuffertiffshift)
		return ShiftTiffEx(sFullInputPath, sTempOuputPath, nShiftBits);

	TIFF *tif, *tifout;
	float imxres,imyres;
	WORD	bps,spp,plc,photo,resunit;
	DWORD imw, iml;
	time_t ltime;
	struct tm *today;

	CTime tStart = CTime::GetCurrentTime();

	if ((tif = TIFFOpen(sFullInputPath, "rm")) == NULL) {
		g_util.LogprintfResample("ERROR: ShiftTiff() - error opening TIFF %s for reading", sFullInputPath);
		return FALSE;
	}

	TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imw);
	TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&iml);
	TIFFGetField(tif,TIFFTAG_RESOLUTIONUNIT,&resunit);
	TIFFGetField(tif,TIFFTAG_XRESOLUTION,&imxres);
	TIFFGetField(tif,TIFFTAG_YRESOLUTION,&imyres);
	if (resunit == RESUNIT_CENTIMETER) {
		imxres *= (float)2.54;
		imyres *= (float)2.54;
	}
	DWORD bpl = TIFFScanlineSize(tif);

	int paddingbits = bpl*8 - imw;
	g_util.LogprintfResample("INFO:  ShiftTiff() - Bit-shifting %s %d bits right..", sFullInputPath, paddingbits);

	if (nShiftBits == 0 && paddingbits == 0) {
		TIFFClose(tif);
		g_util.LogprintfResample("INFO:  ShiftTiff() - Image already OK - returning..");
		return CopyFile(sFullInputPath, sTempOuputPath, FALSE);
	}

	BYTE *linebuf = new BYTE [bpl];
	char tempstring[100];

	// output file
	::DeleteFile(sTempOuputPath);
	if((tifout = TIFFOpen(sTempOuputPath,"wm"))==NULL) {
		delete[] linebuf;
		TIFFClose(tif);
		g_util.LogprintfResample("ERROR: ShiftTiff() - error opening TIFF %s for writing", sTempOuputPath);
		return FALSE;
	}

	TIFFSetField(tifout, TIFFTAG_SUBFILETYPE,0x0);
	//TIFFSetField(tifout, TIFFTAG_IMAGEWIDTH,(DWORD)imw);
	TIFFSetField(tifout, TIFFTAG_IMAGEWIDTH,(DWORD)bpl*8);
	TIFFSetField(tifout, TIFFTAG_IMAGELENGTH,(DWORD)iml);

	TIFFSetField(tifout, TIFFTAG_BITSPERSAMPLE, 1);
	TIFFSetField(tifout, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(tifout, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tifout, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);

	TIFFSetField(tifout, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);
	sprintf(tempstring, "ShiftTiff");
	TIFFSetField(tifout, TIFFTAG_IMAGEDESCRIPTION, tempstring);
	sprintf(tempstring, "InfraLogic");
	TIFFSetField(tifout, TIFFTAG_ARTIST, tempstring);
	sprintf(tempstring, "InputCenter");
	TIFFSetField(tifout, TIFFTAG_SOFTWARE, tempstring);
	TIFFSetField(tifout, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tifout,TIFFTAG_XRESOLUTION, (float)imxres);
	TIFFSetField(tifout,TIFFTAG_YRESOLUTION, (float)imyres);
	TIFFSetField(tifout, TIFFTAG_ROWSPERSTRIP, (DWORD)1);

	time(&ltime);
	today = localtime(&ltime);
	strftime(tempstring,20,"%Y:%m:%d:%H:%M:%S",today);
	TIFFSetField(tif, TIFFTAG_DATETIME, tempstring);
	
	
	if (nShiftBits == 0)
		nShiftBits = paddingbits;

	int bitsToShift = nShiftBits;
	//int bitsToShift2 = 32 - bitsToShift;
	int bitsToShift3 = 8 - bitsToShift;

	int dwordstoshift = 0;
	int restbytestoshift = bpl - 4 * dwordstoshift;
	
	DWORD mask1 = 0xFFFFFFFF << bitsToShift;				// 0xFFFF FFFC
	DWORD mask1inv = ~mask1;								// 0x0000 0003

	for (int i=0; i<iml; i++) {
		if (TIFFReadScanline(tif, linebuf, i,0) < 0) {
			TIFFClose(tif);
			TIFFClose(tifout);
			delete[] linebuf;
			::DeleteFile(sTempOuputPath);
			g_util.LogprintfResample("ERROR: ShiftTiff() - error reading line %d of TIFF file %s",i+1, sFullInputPath);

			return FALSE;
		}	

		//BYTE *p = (BYTE *)linebuf;
		//DWORD lopart = 0x00000000;
		//DWORD hipart = 0x00000000;

	/*
		for (int ecx=0; ecx<restbytestoshift; ecx++) {
			BYTE bt = *p;
			lopart = bt & mask1inv;
			bt >>= bitsToShift;
			bt |= hipart;
			*p = bt;
			hipart = lopart << bitsToShift3; 
			p++;
		}

	*/

		__asm {
			mov esi, DWORD PTR linebuf
			mov ebx, DWORD PTR bpl
			//mov edi, DWORD PTR 	mask1inv
			xor edi,edi
		$tiffshiftloop:
		    				
			movzx edx, BYTE PTR[esi]		// bt(dl) = *p(esi);
			mov eax, DWORD PTR 	mask1inv
			and eax, edx					// lopart(eax after) = bt(dl) & mask1inv;
		
			mov ecx, DWORD PTR bitsToShift
			shr edx, cl						// bt(dl) >>= bitsToShift(cl);

			//or edx, DWORD PTR hipart		// bt(dl) |= hipart;

			or edx, edi		// bt(dl) |= hipart(edi);
			mov BYTE PTR [esi],dl			// *p(esi) = bt(dl);

			mov ecx, DWORD PTR bitsToShift3 
			shl eax, cl						// hipart(eax after) = lopart(eax) << bitsToShift3(cl); 
			//mov DWORD PTR hipart,eax
		    mov edi,eax						
			inc esi

			dec ebx
			jnz $tiffshiftloop

		}
		
		if (TIFFWriteScanline(tifout, linebuf, i, 0) < 0) {
			TIFFClose(tif);
			TIFFClose(tifout);
			delete[] linebuf;
			DeleteFile(sTempOuputPath);
			g_util.LogprintfResample("ERROR: ShiftTiff() - error writing line %d of TIFF file %s",i+1, sTempOuputPath);

			return FALSE;
		}
	}
	TIFFClose(tif);
	TIFFClose(tifout);
	delete[] linebuf;
	CTime tStop = CTime::GetCurrentTime();
	CTimeSpan ts = tStop - tStart;
	g_util.LogprintfResample("INFO:  ShiftTiff() - Bit-shifting done - time %d sec.", ts.GetTotalSeconds());
	return TRUE;
}

// Full buffer version..
BYTE *pShiftBuffer = NULL;
DWORD dwShiftBufferSize = 0;
BOOL ShiftTiffEx(CString sFullInputPath, CString sTempOuputPath, int nShiftBits) 
{

	TIFF *tif, *tifout;
	float imxres,imyres;
	WORD	bps,spp,plc,photo,resunit;
	DWORD imw, iml;
	time_t ltime;
	struct tm *today;
	char tempstring[100];

	CTime tStart = CTime::GetCurrentTime();

	if ((tif = TIFFOpen(sFullInputPath, "rm")) == NULL) {
		g_util.LogprintfResample("ERROR: ShiftTiffEx() - error opening TIFF %s for reading", sFullInputPath);
		return FALSE;
	}

	TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imw);
	TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&iml);
	TIFFGetField(tif,TIFFTAG_RESOLUTIONUNIT,&resunit);
	TIFFGetField(tif,TIFFTAG_XRESOLUTION,&imxres);
	TIFFGetField(tif,TIFFTAG_YRESOLUTION,&imyres);
	if (resunit == RESUNIT_CENTIMETER) {
		imxres *= (float)2.54;
		imyres *= (float)2.54;
	}
	DWORD bpl = TIFFScanlineSize(tif);



	int paddingbits = bpl*8 - imw;
	g_util.LogprintfResample("INFO:  ShiftTiffEx() - Bit-shifting %s %d bits right..", sFullInputPath, paddingbits);

	if (nShiftBits == 0 && paddingbits == 0) {
		TIFFClose(tif);
		g_util.LogprintfResample("INFO:  ShiftTiffEx() - Image already OK - returning..");
		return CopyFile(sFullInputPath, sTempOuputPath, FALSE);
	}

	// output file
	::DeleteFile(sTempOuputPath);
	if((tifout = TIFFOpen(sTempOuputPath,"wm"))==NULL) {
		
		TIFFClose(tif);
		g_util.LogprintfResample("ERROR: ShiftTiffEx() - error opening TIFF %s for writing", sTempOuputPath);
		return FALSE;
	}

	DWORD dwBufSize = bpl * iml;
	if (pShiftBuffer == NULL) {
		pShiftBuffer = new BYTE [dwBufSize];
		if (pShiftBuffer == NULL) {
			g_util.LogprintfResample("ERROR:  Memory allocation error in ShiftTiffEx()");
			TIFFClose(tif);
			return FALSE;
		}
		dwShiftBufferSize = dwBufSize;
	} else if (dwShiftBufferSize < dwBufSize) {
		delete pShiftBuffer;
		pShiftBuffer = new BYTE [dwBufSize];
		if (pShiftBuffer == NULL) {
			g_util.LogprintfResample("ERROR:  Memory allocation error in ShiftTiffEx()");
			TIFFClose(tif);
			return FALSE;
		}
		dwShiftBufferSize = dwBufSize;
	}


	TIFFSetField(tifout, TIFFTAG_SUBFILETYPE,0x0);
	//TIFFSetField(tifout, TIFFTAG_IMAGEWIDTH,(DWORD)imw);
	TIFFSetField(tifout, TIFFTAG_IMAGEWIDTH,(DWORD)bpl*8);
	TIFFSetField(tifout, TIFFTAG_IMAGELENGTH,(DWORD)iml);

	TIFFSetField(tifout, TIFFTAG_BITSPERSAMPLE, 1);
	TIFFSetField(tifout, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(tifout, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tifout, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);

	TIFFSetField(tifout, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);
	sprintf(tempstring, "ShiftTiff");
	TIFFSetField(tifout, TIFFTAG_IMAGEDESCRIPTION, tempstring);
	sprintf(tempstring, "InfraLogic");
	TIFFSetField(tifout, TIFFTAG_ARTIST, tempstring);
	sprintf(tempstring, "InputCenter");
	TIFFSetField(tifout, TIFFTAG_SOFTWARE, tempstring);
	TIFFSetField(tifout, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tifout,TIFFTAG_XRESOLUTION, (float)imxres);
	TIFFSetField(tifout,TIFFTAG_YRESOLUTION, (float)imyres);
	TIFFSetField(tifout, TIFFTAG_ROWSPERSTRIP, (DWORD)1);

	time(&ltime);
	today = localtime(&ltime);
	strftime(tempstring,20,"%Y:%m:%d:%H:%M:%S",today);
	TIFFSetField(tif, TIFFTAG_DATETIME, tempstring);
	
	
	if (nShiftBits == 0)
		nShiftBits = paddingbits;

	int bitsToShift = nShiftBits;
	//int bitsToShift2 = 32 - bitsToShift;
	int bitsToShift3 = 8 - bitsToShift;

	int dwordstoshift = 0;
	int restbytestoshift = bpl - 4 * dwordstoshift;
	
	DWORD mask1 = 0xFFFFFFFF << bitsToShift;				// 0xFFFF FFFC
	DWORD mask1inv = ~mask1;								// 0x0000 0003

	BYTE *pIn = pShiftBuffer;
	for (int i=0; i<iml; i++) {
		if (TIFFReadScanline(tif, pIn, i,0) < 0) {
			TIFFClose(tif);
			TIFFClose(tifout);
			::DeleteFile(sTempOuputPath);
			g_util.LogprintfResample("ERROR: ShiftTiffEx() - error reading line %d of TIFF file %s",i+1, sFullInputPath);

			return FALSE;
		}	

		pIn += bpl;
	}


	pIn = pShiftBuffer;
	for (int i=0; i<iml; i++) {

		__asm {
			mov esi, DWORD PTR pIn
			mov ebx, DWORD PTR bpl
			//mov edi, DWORD PTR 	mask1inv
			xor edi,edi
		$tiffshiftloop:
		    				
			movzx edx, BYTE PTR[esi]		// bt(dl) = *p(esi);
			mov eax, DWORD PTR 	mask1inv
			and eax, edx					// lopart(eax after) = bt(dl) & mask1inv;
		
			mov ecx, DWORD PTR bitsToShift
			shr edx, cl						// bt(dl) >>= bitsToShift(cl);

			//or edx, DWORD PTR hipart		// bt(dl) |= hipart;

			or edx, edi		// bt(dl) |= hipart(edi);
			mov BYTE PTR [esi],dl			// *p(esi) = bt(dl);

			mov ecx, DWORD PTR bitsToShift3 
			shl eax, cl						// hipart(eax after) = lopart(eax) << bitsToShift3(cl); 
			//mov DWORD PTR hipart,eax
		    mov edi,eax						
			inc esi

			dec ebx
			jnz $tiffshiftloop

		}
		pIn += bpl;
	}
	
	pIn = pShiftBuffer;
	for (int i=0; i<iml; i++) {
		if (TIFFWriteScanline(tifout, pIn, i, 0) < 0) {
			TIFFClose(tif);
			TIFFClose(tifout);
			::DeleteFile(sTempOuputPath);
			g_util.LogprintfResample("ERROR: ShiftTiffEx() - error writing line %d of TIFF file %s",i+1, sTempOuputPath);

			return FALSE;
		}
		pIn += bpl;
	}

	TIFFClose(tif);
	TIFFClose(tifout);

	CTime tStop = CTime::GetCurrentTime();
	CTimeSpan ts = tStop - tStart;
	g_util.LogprintfResample("INFO:  ShiftTiffEx() - Bit-shifting done - time %d sec.", ts.GetTotalSeconds());
	return TRUE;
}



BOOL ShiftTiffVertical(CString sFullInputPath, CString sTempOuputPath) 
{

	TIFF *tif, *tifout;
	float imxres,imyres;
	WORD	bps,spp,plc,photo,resunit;
	DWORD imw, iml;
	time_t ltime;
	struct tm *today;

	if ((tif = TIFFOpen(sFullInputPath, "rm")) == NULL) {
		return FALSE;
	}

	TIFFGetField(tif,TIFFTAG_IMAGEWIDTH,&imw);
	TIFFGetField(tif,TIFFTAG_IMAGELENGTH,&iml);
	TIFFGetField(tif,TIFFTAG_RESOLUTIONUNIT,&resunit);
	TIFFGetField(tif,TIFFTAG_XRESOLUTION,&imxres);
	TIFFGetField(tif,TIFFTAG_YRESOLUTION,&imyres);
	if (resunit == RESUNIT_CENTIMETER) {
		imxres *= (float)2.54;
		imyres *= (float)2.54;
	}
	DWORD bpl = TIFFScanlineSize(tif);

	int paddinglines = 8 - (iml % 8);

	if (paddinglines == 0 || paddinglines == 8 ) {
		TIFFClose(tif);
		return CopyFile(sFullInputPath, sTempOuputPath, FALSE);
	}

	g_util.LogprintfResample("INFO: Shifting image down %d lines", paddinglines);


	BYTE *linebuf = new BYTE [bpl];
	char tempstring[100];

	// output file

	if((tifout = TIFFOpen(sTempOuputPath,"wm"))==NULL) {
		delete[] linebuf;
		TIFFClose(tif);
		return FALSE;
	}

	TIFFSetField(tifout, TIFFTAG_SUBFILETYPE,0x0);
	TIFFSetField(tifout, TIFFTAG_IMAGEWIDTH,(DWORD)imw);
	TIFFSetField(tifout, TIFFTAG_IMAGELENGTH,(DWORD)(iml + paddinglines));

	TIFFSetField(tifout, TIFFTAG_BITSPERSAMPLE, 1);
	TIFFSetField(tifout, TIFFTAG_SAMPLESPERPIXEL, 1);
	TIFFSetField(tifout, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tifout, TIFFTAG_COMPRESSION, COMPRESSION_CCITTFAX4);

	TIFFSetField(tifout, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_MINISWHITE);
	sprintf(tempstring, "ShiftTiff");
	TIFFSetField(tifout, TIFFTAG_IMAGEDESCRIPTION, tempstring);
	sprintf(tempstring, "InfraLogic");
	TIFFSetField(tifout, TIFFTAG_ARTIST, tempstring);
	sprintf(tempstring, "InputCenter");
	TIFFSetField(tifout, TIFFTAG_SOFTWARE, tempstring);
	TIFFSetField(tifout, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
	TIFFSetField(tifout,TIFFTAG_XRESOLUTION, (float)imxres);
	TIFFSetField(tifout,TIFFTAG_YRESOLUTION, (float)imyres);
	TIFFSetField(tifout, TIFFTAG_ROWSPERSTRIP, (DWORD)1);

	time(&ltime);
	today = localtime(&ltime);
	strftime(tempstring,20,"%Y:%m:%d:%H:%M:%S",today);
	TIFFSetField(tif, TIFFTAG_DATETIME, tempstring);

	g_util.LogprintfResample("INFO: Shift image %d lines down", paddinglines);
	
	memset(linebuf, 0x00, bpl);

	for (int i=0; i<paddinglines; i++) {
		if (TIFFWriteScanline(tifout, linebuf, i, 0) < 0) {
			TIFFClose(tif);
			TIFFClose(tifout);
			delete[] linebuf;
			DeleteFile(sTempOuputPath);
			return FALSE;
		}
	}

	for (int i=0; i<iml; i++) {
		if (TIFFReadScanline(tif, linebuf, i,0) < 0) {
			TIFFClose(tif);
			TIFFClose(tifout);
			delete[] linebuf;
			DeleteFile(sTempOuputPath);
			return FALSE;
		}	

		if (TIFFWriteScanline(tifout, linebuf, i+paddinglines, 0) < 0) {
			TIFFClose(tif);
			TIFFClose(tifout);
			delete[] linebuf;
			DeleteFile(sTempOuputPath);
			return FALSE;
		}
	}
	TIFFClose(tif);
	TIFFClose(tifout);
	delete[] linebuf;

	return TRUE;
}


BYTE *ReadTiffAndCropMargin(TCHAR *filename, int *tnw, int *tnh, int *type, double margin)
{
	int res;
	if (margin == 0.0)
		return ReadTiff(filename, tnw, tnh, type, &res);

	TIFF	*tif;
	float	xres, yres;
	short	spp;
	WORD	res_unit;
	DWORD	rowsperstrip;


	if ((tif = TIFFOpen(filename, "rm")) == NULL) {
		return NULL;
	}
	int w,h;
	TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
	TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
	 TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &spp);
	
	 TIFFGetField(tif, TIFFTAG_XRESOLUTION, &xres);
	TIFFGetField(tif, TIFFTAG_YRESOLUTION, &yres);
	TIFFGetFieldDefaulted(tif, TIFFTAG_RESOLUTIONUNIT, &res_unit);
		if (res_unit == RESUNIT_CENTIMETER) {
		xres *= (float)2.54;
		yres *= (float)2.54;
	}
	
	double pixelstoskip = yres*margin/25.4;
	int bytestoskip = pixelstoskip*spp;

	int linestoread = h - 2* pixelstoskip;
	int bpltoread = w - 2* pixelstoskip;
	bpltoread *= spp;

	

	BYTE *buf = NULL;
	try {
		buf = new BYTE[bpltoread*linestoread];
	}
	catch (CMemoryException *ex) {
		TIFFClose(tif);
		return NULL;
	}
	BYTE *p = buf;
	BYTE *tempbuf = new BYTE[w*spp];

	
	DWORD	bpl = TIFFScanlineSize(tif);
	int s = 0;
	
	for (int i=0; i<(int)pixelstoskip; i++)
		TIFFReadScanline(tif, tempbuf, s++, 0);

	for (int j=0; j<linestoread; j++) {
		TIFFReadScanline(tif, tempbuf, s++, 0);
		memmove((void *)(buf+j*bpltoread), (void *)(tempbuf+bytestoskip), bpltoread);
	}

	TIFFClose(tif);
	*type = spp; 
	*tnw = bpltoread/spp;
	*tnh = linestoread;

	delete [] tempbuf;
	return buf;
}


BOOL	CompareTiff(CString filename1, CString filename2, double fWidth, double fHeight)
{

	TIFF	*tif1, *tif2;
	float	xres1, yres1;
	float	xres2, yres2;
//	short	spp;
	WORD	res_unit;
//	DWORD	rowsperstrip;

	// Convert to inches
	fWidth /= 25.4;
	fHeight /= 25.4;

	if ((tif1 = TIFFOpen(filename1, "rm")) == NULL) {
		return FALSE;
	}
	if ((tif2 = TIFFOpen(filename2, "rm")) == NULL) {
		TIFFClose(tif1);
		return FALSE;
	}

	int w1,h1,w2,h2;
	TIFFGetField(tif1, TIFFTAG_IMAGEWIDTH, &w1);
	TIFFGetField(tif2, TIFFTAG_IMAGEWIDTH, &w2);
	TIFFGetField(tif1, TIFFTAG_IMAGELENGTH, &h1);
	TIFFGetField(tif2, TIFFTAG_IMAGELENGTH, &h2);
	
	TIFFGetField(tif1, TIFFTAG_XRESOLUTION, &xres1);
	TIFFGetField(tif2, TIFFTAG_XRESOLUTION, &xres2);
	TIFFGetField(tif1, TIFFTAG_YRESOLUTION, &yres1);
	TIFFGetField(tif2, TIFFTAG_YRESOLUTION, &yres2);

	TIFFGetFieldDefaulted(tif1, TIFFTAG_RESOLUTIONUNIT, &res_unit);
	if (res_unit == RESUNIT_CENTIMETER) {
		xres1 *= (float)2.54;
		yres1 *= (float)2.54;
	}
	TIFFGetFieldDefaulted(tif2, TIFFTAG_RESOLUTIONUNIT, &res_unit);
	if (res_unit == RESUNIT_CENTIMETER) {
		xres2 *= (float)2.54;
		yres2 *= (float)2.54;
	}


	DWORD	bpl1 = TIFFScanlineSize(tif1);
	DWORD	bpl2 = TIFFScanlineSize(tif2);


	if (xres1!=xres2 || yres1 != yres2 || w1 != w2 || h1 != h2 || bpl1 != bpl2) {
		TIFFClose(tif1);
		TIFFClose(tif2);
		return FALSE;
	}


	int nDwordsToCompare = (bpl1+3)/4;
	BYTE *buf1 = NULL;
	BYTE *buf2 = NULL;
	try {
		buf1 = new BYTE[nDwordsToCompare*4];
	}
	catch (CMemoryException *ex) {
		TIFFClose(tif1);
		TIFFClose(tif2);
		return FALSE;
	}
	try {
		buf2 = new BYTE[nDwordsToCompare*4];
	}
	catch (CMemoryException *ex) {
		delete [] buf1;
		TIFFClose(tif1);
		TIFFClose(tif2);
		return FALSE;
	}

	memset(buf1, 0x00, nDwordsToCompare*4);
	memset(buf2, 0x00, nDwordsToCompare*4);

	double ff = fWidth * xres1;
	DWORD dwOffsetX = ff / 8;

	if (dwOffsetX >= bpl1 || dwOffsetX >= bpl2)
		dwOffsetX = 0;

	if (dwOffsetX > 0)
		dwOffsetX = (bpl1 - dwOffsetX)/2;

	ff = fHeight * yres1;
	DWORD dwOffsetY = ff;
	if (dwOffsetY >= h1 ||  dwOffsetY >= h2)
		dwOffsetY = 0;

	if (dwOffsetY > 0)
		dwOffsetY = (h1 - dwOffsetY) / 2;

	if (dwOffsetX > 0)
		nDwordsToCompare -= (dwOffsetX/4);

	if (dwOffsetX < 0)
		dwOffsetX = 0;

	if (dwOffsetY < 0)
		dwOffsetY = 0;

	BOOL bIsDifference = FALSE;

	if (dwOffsetY > 0) {
		for (int j=0; j<dwOffsetY; j++) {
			TIFFReadScanline(tif1, buf1, j, 0);
			TIFFReadScanline(tif2, buf2, j, 0);
		}
	}

	for (int j=dwOffsetY; j<h1; j++) {
		TIFFReadScanline(tif1, buf1, j, 0);
		TIFFReadScanline(tif2, buf2, j, 0);

		__asm {
			mov esi, DWORD PTR buf1
			mov edi, DWORD PTR buf2
			mov ecx, DWORD PTR nDwordsToCompare
			mov edx, DWORD PTR dwOffsetX			// xor edx,edx
		$compareloop:
			
			mov eax, DWORD PTR [esi+edx]
			mov ebx, DWORD PTR [edi+edx]
			cmp eax,ebx
			jne $compareloopfailed
			add edx,4
			loop $compareloop
			jmp $compareloopdone
		$compareloopfailed:
			mov DWORD PTR bIsDifference,1
		$compareloopdone:
		}
		if (bIsDifference)
			break;
	}

	TIFFClose(tif1);
	TIFFClose(tif2);

	delete [] buf1;
	delete [] buf2;
	return bIsDifference ? FALSE :  TRUE;
}


BYTE *ReadJPEG(TCHAR *filename, int *tnw, int *tnh, int *type)
{
	struct jpeg_decompress_struct cinfo;	
	
	FILE * infile;		// source file 
	JSAMPARRAY buffer;	// Output row buffer 
	int row_stride;		// physical row width in output buffer 
	BYTE	*bitarray;

	if ((infile = fopen(filename, "rb")) == NULL) {
		//Logprintf("Read JPEG: can't open %s\n", filename);
		return NULL;
	}

	// Now we can initialize the JPEG decompression object. 
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);

	(void) jpeg_read_header(&cinfo, TRUE);

	*tnw = cinfo.image_width; 												// Width of image, in pixels
	*tnh = cinfo.image_height;												// Height of image, in pixels
	*type = cinfo.output_components;										// Number of color channels (samples per pixel)
	
	row_stride = cinfo.output_width * cinfo.output_components;
	
	(void) jpeg_start_decompress(&cinfo);

	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	bitarray = new BYTE[row_stride * cinfo.output_height];
	if (bitarray == NULL) {
//		Logprintf("JPEG read : Memory allocation error\r\n"),
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return NULL;
	}
	
	BYTE *pbuf = bitarray;

	while (cinfo.output_scanline < cinfo.output_height) {
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		memmove(pbuf, buffer[0], row_stride);
		pbuf += row_stride;;
	}

	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	return bitarray;						
}

void SplitInterleaved(BYTE *pColorData, DWORD tnw, DWORD tnh, BYTE *pRedData, BYTE *pGreenData, BYTE *pBlueData)
{
	pRedData = new BYTE[tnw*tnh];
	BYTE *pR = pRedData;
	pGreenData = new BYTE[tnw*tnh];
	BYTE *pG = pGreenData;
	pBlueData = new BYTE[tnw*tnh];
	BYTE *pB = pBlueData;
	BYTE *pOrg = pColorData;
	DWORD n = tnw*tnh;

	for (int i=0; i<n; i++) {
		*pR++ = *pOrg++;
		*pG++ = *pOrg++;
		*pB++ = *pOrg++;
	}
	
	delete pColorData;
}

BYTE* CombinePlanes(DWORD tnw, DWORD tnh, BYTE *pRedData, BYTE *pGreenData, BYTE *pBlueData)
{

	BYTE *pColorData = new BYTE[3*tnw*tnh];
	BYTE *pR = pRedData;
	BYTE *pG = pGreenData;
	BYTE *pB = pBlueData;
	BYTE *pOrg = pColorData;
	DWORD n = tnw*tnh;

	for (int i=0; i<n; i++) {
		*pOrg++ = *pR++;
		*pOrg++ = *pG++;
		*pOrg++ = *pB++; 
	}
	delete pRedData;
	delete pGreenData;
	delete pBlueData;

	return pColorData;
}




WORD DIBNumColors(LPSTR lpDIB)
{
   WORD wBitCount;  // DIB bit count

   /*  If this is a Windows-style DIB, the number of colors in the
    *  color table can be less than the number of bits per pixel
    *  allows for (i.e. lpbi->biClrUsed can be set to some value).
    *  If this is the case, return the appropriate value.
    */

   if (IS_WIN30_DIB(lpDIB))
   {
      DWORD dwClrUsed;

      dwClrUsed = ((LPBITMAPINFOHEADER)lpDIB)->biClrUsed;
      if (dwClrUsed)
     return (WORD)dwClrUsed;
   }

   /*  Calculate the number of colors in the color table based on
    *  the number of bits per pixel for the DIB.
    */
   if (IS_WIN30_DIB(lpDIB))
      wBitCount = ((LPBITMAPINFOHEADER)lpDIB)->biBitCount;
   else
      wBitCount = ((LPBITMAPCOREHEADER)lpDIB)->bcBitCount;

   /* return number of colors based on bits per pixel */
   switch (wBitCount)
      {
   case 1:
      return 2;

   case 4:
      return 16;

   case 8:
      return 256;

   default:
      return 0;
      }
}

WORD PaletteSize(LPSTR lpDIB)
{
   /* calculate the size required by the palette */
   if (IS_WIN30_DIB (lpDIB))
      return (DIBNumColors(lpDIB) * sizeof(RGBQUAD));
   else
      return (DIBNumColors(lpDIB) * sizeof(RGBTRIPLE));
}


void Rotate180BGRMirror(BYTE *buf, DWORD BytesPerLine, DWORD NumberOfLines)
{
	int RGBBytesPerLine = 3 * BytesPerLine;

	DWORD dstptr = (DWORD)buf + (NumberOfLines - 1) * RGBBytesPerLine;
		
	__asm {
		mov	esi, DWORD PTR buf
		mov	edi, DWORD PTR dstptr
		mov edx, DWORD PTR RGBBytesPerLine	 
		mov	ebx, DWORD PTR NumberOfLines
		shr ebx, 1
	$rgbrot180mirloop1:			
		xor ecx, ecx
	$rgbrot180mirloop2:			
		mov al, BYTE PTR [esi+ecx]			//B
		mov ah, BYTE PTR [edi+ecx+2]
		mov BYTE PTR [esi+ecx],ah
		mov BYTE PTR [edi+ecx+2],al
		mov al, BYTE PTR [esi+ecx+1]		//G
		mov ah, BYTE PTR [edi+ecx+1]		//G
		mov BYTE PTR [esi+ecx+1],ah
		mov BYTE PTR [edi+ecx+1],al
		mov al, BYTE PTR [esi+ecx+2]		//R
		mov ah, BYTE PTR [edi+ecx]
		mov BYTE PTR [esi+ecx+2],ah
		mov BYTE PTR [edi+ecx],al
		add ecx,3
		cmp ecx,edx
		jne $rgbrot180mirloop2
		add esi, edx
		sub edi, edx
		dec ebx
		jnz $rgbrot180mirloop1		
		and NumberOfLines,1
		jz $rgbrot180exit
		xor ecx,ecx
	$rgbrot180mirloop3:			
		mov al, BYTE PTR [esi+ecx]			//B
		mov ah, BYTE PTR [esi+ecx+2]		//R
		mov BYTE PTR [esi+ecx],ah			//R
		mov BYTE PTR [esi+ecx+2],al			//B
		add ecx,3
		cmp ecx,edx
		jne $rgbrot180mirloop3
	$rgbrot180exit:
	}
}

void Rotate180RGBMirror(BYTE *buf, DWORD BytesPerLine, DWORD NumberOfLines)
{
	int RGBBytesPerLine = 3 * BytesPerLine;

	DWORD dstptr = (DWORD)buf + (NumberOfLines - 1) * RGBBytesPerLine;
		
	__asm {
		mov	esi, DWORD PTR buf
		mov	edi, DWORD PTR dstptr
		mov edx, DWORD PTR RGBBytesPerLine	 
		mov	ebx, DWORD PTR NumberOfLines
		shr ebx, 1
	$rgbrot180mirloop1:			
		xor ecx, ecx
	$rgbrot180mirloop2:			
		mov al, BYTE PTR [esi+ecx]			//R
		mov ah, BYTE PTR [edi+ecx]
		mov BYTE PTR [esi+ecx],ah
		mov BYTE PTR [edi+ecx],al
		mov al, BYTE PTR [esi+ecx+1]		//G
		mov ah, BYTE PTR [edi+ecx+1]		
		mov BYTE PTR [esi+ecx+1],ah
		mov BYTE PTR [edi+ecx+1],al
		mov al, BYTE PTR [esi+ecx+2]		//B
		mov ah, BYTE PTR [edi+ecx+2]
		mov BYTE PTR [esi+ecx+2],ah
		mov BYTE PTR [edi+ecx+2],al
		add ecx,3
		cmp ecx,edx
		jne $rgbrot180mirloop2
		add esi, edx
		sub edi, edx
		dec ebx
		jnz $rgbrot180mirloop1		
		and NumberOfLines,1
		jz $rgbrot180exit
		xor ecx,ecx
	$rgbrot180mirloop3:			
	//	mov al, BYTE PTR [esi+ecx]			//B
	//	mov ah, BYTE PTR [esi+ecx+2]		//R
	//	mov BYTE PTR [esi+ecx],ah			//R
	//	mov BYTE PTR [esi+ecx+2],al			//B
		add ecx,3
		cmp ecx,edx
		jne $rgbrot180mirloop3
	$rgbrot180exit:
	}
}

void Rotate180MonoMirror(BYTE *buf, DWORD BytesPerLine, DWORD NumberOfLines)
{
	DWORD dstptr = (DWORD)buf + (NumberOfLines - 1) * BytesPerLine;
		
	__asm {
		mov	esi, DWORD PTR buf
		mov	edi, DWORD PTR dstptr
		mov edx, DWORD PTR BytesPerLine	 
		mov	ebx, DWORD PTR NumberOfLines
		shr ebx, 1
	$monorot180mirloop1:			
		xor ecx, ecx
	$monorot180mirloop2:			
		mov al, BYTE PTR [esi+ecx]			// swap 
		mov ah, BYTE PTR [edi+ecx]
		mov BYTE PTR [esi+ecx],ah
		mov BYTE PTR [edi+ecx],al
		inc ecx
		cmp ecx,edx
		jne $monorot180mirloop2
		add esi, edx
		sub edi, edx
		dec ebx
		jnz $monorot180mirloop1		
		and NumberOfLines,1
		jz $monorot180exit
		xor ecx,ecx
	$monorot180mirloop3:			
		mov al, BYTE PTR [esi+ecx]			
		mov ah, BYTE PTR [esi+ecx]		
		mov BYTE PTR [esi+ecx],ah		
		mov BYTE PTR [esi+ecx],al			
		inc ecx
		cmp ecx,edx
		jne $monorot180mirloop3
	$monorot180exit:
	}
}

int BMPcolor = FALSE;

BYTE	*CreateBMP(void *thumbnail, int width, int height, int type, char *BMPpath)
{
	
	WORD	wBitCount = (type == COLORFILE) ? 24 : 8;

	int clrs = (wBitCount>>3);
	BYTE *newth = NULL;
	if (width%4) {
		int newwidth = width + 4 - (width%4);
		BYTE *newth = new BYTE[newwidth*height*clrs];
		if (newth == NULL) {
			return NULL;
		}
		BYTE *ps = (BYTE *)thumbnail;
		BYTE *pd = newth;
		memset(newth,0xFF,newwidth*height*clrs);
		for (int y=0; y<height; y++) {
			memcpy(pd, ps, width*clrs);
			pd += newwidth*clrs;
			ps += width*clrs;
		}
		delete thumbnail;
		thumbnail = newth;
		width = newwidth;
	}
	if (type == COLORFILE)
		if (BMPcolor)
			Rotate180RGBMirror((BYTE *)thumbnail,width, height);
		else
			Rotate180BGRMirror((BYTE *)thumbnail,width, height);
	else
		Rotate180MonoMirror((BYTE *)thumbnail, width, height);
	
	CFile				bmpfile;
	BITMAPFILEHEADER	bif;		// Bitmap file header
	BITMAPINFOHEADER	bi;			// Bitmap header
	RGBQUAD *colortable = NULL;
	int colortablesize = 0;
	// Initialize BITMAPFILEHEADER
	bif.bfType = (WORD)('B' + 'M' * 256); 
	bif.bfReserved1 = 0;
	bif.bfReserved2 = 0;

	// initialize BITMAPINFOHEADER
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;// + padding;         // fill in width from parameter
	bi.biHeight = height;       // fill in height from parameter
	bi.biPlanes = 1;              // must be 1
	bi.biBitCount = wBitCount;    // from parameter
	bi.biCompression = BI_RGB;    
	bi.biSizeImage = height * width * clrs/* + padding*/;
	bi.biXPelsPerMeter = 0; //2834;
	bi.biYPelsPerMeter = 0; //2834;
	bi.biClrUsed = (type == COLORFILE) ? 0 : 256;
	bi.biClrImportant = 0;
    
	if (type != COLORFILE) {
		colortablesize = PaletteSize((LPSTR)&bi);
		colortable = new RGBQUAD [colortablesize/4];
		for (int i = 0; i< colortablesize/4; i++) {
			colortable[i].rgbRed = colortable[i].rgbGreen = colortable[i].rgbBlue = 255-i;
			colortable[i].rgbReserved = 0;
		}
	}
    
//	bif.bfOffBits = (DWORD)sizeof(BITMAPINFOHEADER) + colortablesize;
//	bif.bfSize = (DWORD)(sizeof(BITMAPFILEHEADER) + bif.bfOffBits + bi.biSizeImage); 
	bif.bfOffBits = (DWORD)sizeof(BITMAPFILEHEADER) + (DWORD)sizeof(BITMAPINFOHEADER) + colortablesize;
	bif.bfSize = (DWORD)(bif.bfOffBits + bi.biSizeImage); 


	bmpfile.Open(BMPpath, CFile::modeCreate | CFile::modeWrite, 0);

	bmpfile.Write((void *)&bif, sizeof(BITMAPFILEHEADER));
	bmpfile.Write((void *)&bi, sizeof(BITMAPINFOHEADER));
	if (type != COLORFILE && colortable != NULL) {
		bmpfile.Write((void *)colortable, colortablesize);
		delete colortable;
	}
	bmpfile.Write(thumbnail, bi.biSizeImage);
	bmpfile.Close();

	return (BYTE *)thumbnail;
}


BYTE *ApplyBoundingBox(BYTE *thumbnail, int *tnw, int *tnh, int Type, double fPrevres, double fBoundingBoxSizeX, double fBoundingBoxSizeY, double fBoundingBoxPosX, double fBoundingBoxPosY) 
{
	int BytesPerPixel = (Type ==  MONOFILE ? 1 :(Type == CMYKFILE ? 4 : 3));
	int local_tnw = *tnw;
	int local_tnh = *tnh;

	// CString s;
	//s.Format("BBOX: cx=%.2f, cy=%.2f, x=%.2f, y=%.2f, res=%.2f", fBoundingBoxSizeX,fBoundingBoxSizeY,fBoundingBoxPosX,fBoundingBoxPosY,fPrevres);
	//AfxMessageBox(s);

	if (fBoundingBoxSizeX == 0.0 || fBoundingBoxSizeY == 0.0 )
		return NULL;
	
	double fTmp =  -1.0 * fBoundingBoxPosY * fPrevres;
	int AddTopLines = fTmp;
	
	fTmp = -1.0 * fBoundingBoxPosX * fPrevres;
	int AddLeftBytes = fTmp;

	fTmp = fBoundingBoxSizeX * fPrevres;
	int AddRightBytes = (int)fTmp - local_tnw - AddLeftBytes;

	fTmp = (fBoundingBoxSizeY + fBoundingBoxPosY ) * fPrevres;
	int AddBottomLines = (int)fTmp - local_tnh;


	AddLeftBytes *= BytesPerPixel;
	AddRightBytes *= BytesPerPixel;
	local_tnw *= BytesPerPixel;

	if (!AddLeftBytes && !AddRightBytes && !AddTopLines && !AddBottomLines) 
		return thumbnail;

	DWORD new_tnh = local_tnh + AddTopLines + AddBottomLines;
	DWORD new_tnw = local_tnw + AddLeftBytes + AddRightBytes;


	BYTE *NewTh = new BYTE[new_tnw * new_tnh];
	if (NewTh == NULL) {
		return thumbnail;
	}	

	// Do the cropping

	BYTE *dest = NewTh;
	BYTE *src = thumbnail;

	// Do top crop/margin
	int LinesToMove = new_tnh;
	if (AddTopLines > 0) {
		dest += AddTopLines * new_tnw;
		LinesToMove -= AddTopLines;
	} else if (AddTopLines < 0) 
		src -= AddTopLines * local_tnw;
		
	if (AddBottomLines > 0)
		LinesToMove -= AddBottomLines;

	FillMemory(NewTh, new_tnh * new_tnw, Type ==  COLORFILE ? 0xFF: 0x00);
	
	for (int y = 0;y < LinesToMove; y++) {
		if (AddLeftBytes>=0 && AddRightBytes>=0)
			memcpy(dest+AddLeftBytes, src, local_tnw);
			
		else if (AddLeftBytes>=0 && AddRightBytes <0) 
			memcpy(dest+AddLeftBytes, src, local_tnw + AddRightBytes);
			
		else if (AddLeftBytes<0 && AddRightBytes>=0) 
			memcpy(dest, src - AddLeftBytes, local_tnw + AddLeftBytes);
			
		else if (AddLeftBytes<0 && AddRightBytes<0) 
			memcpy(dest, src - AddLeftBytes, new_tnw);
				
		src += local_tnw;
		dest += new_tnw; 		
	}

	*tnh = new_tnh;
	*tnw = new_tnw / BytesPerPixel;
	delete thumbnail;
	thumbnail = NewTh;
	return thumbnail;
}

void MirrorRGB(BYTE *buf, int BytesPerLine, int NumberOfLines)
{
	int totalBytesPerLine = BytesPerLine * 3;

	// Always DWORD aligned number of triples
	__asm {
		mov	edi, DWORD PTR buf
		mov esi, DWORD PTR totalBytesPerLine	 
		shr esi, 1						// half a line in bytes
	  $ppmirloop1:
		xor eax,eax						// left	pos
		mov ebx, DWORD PTR totalBytesPerLine	 
		sub ebx,3							// right pos
		xor ecx,ecx
		xor edx,edx
	  $ppmirloop2:
		mov cl, BYTE PTR [edi+eax]		// ecx = *(bptr+left)		Red
		mov	dl, BYTE PTR [edi+ebx]		// edx = *(bptr+right)
		mov	BYTE PTR [edi+eax],dl	
		mov BYTE PTR [edi+ebx],cl		// swap
		mov cl, BYTE PTR [edi+eax+1]	// ecx = *(bptr+left)		Green
		mov	dl, BYTE PTR [edi+ebx+1]	// edx = *(bptr+right)
		mov	BYTE PTR [edi+eax+1],dl	
		mov BYTE PTR [edi+ebx+1],cl		// swap
		mov cl, BYTE PTR [edi+eax+2]	// ecx = *(bptr+left)		Blue
		mov	dl, BYTE PTR [edi+ebx+2]	// edx = *(bptr+right)
		mov	BYTE PTR [edi+eax+2],dl	
		mov BYTE PTR [edi+ebx+2],cl		// swap
		add	eax,3						// left++
		sub ebx,3						// right--
		cmp eax, esi
		jl $ppmirloop2
		add edi, DWORD PTR totalBytesPerLine	 
		dec DWORD PTR NumberOfLines
		jnz	$ppmirloop1
	}
}

BYTE *RotateRGB90(BYTE *buf, int *pBytesPerLine, int *pNumberOfLines, BOOL Rotate270)
{
	DWORD NumberOfLines = *pNumberOfLines;
	DWORD BytesPerLine = *pBytesPerLine;
	DWORD RotBytesPerLine = NumberOfLines;
	DWORD RotNumberOfLines = BytesPerLine;
	int	Padding = 0, i, j, k, m;
	
	// Align to 32 bit
	if (RotBytesPerLine % 4) {
		Padding = 4 - (RotBytesPerLine % 4);
		RotBytesPerLine += Padding;
	}

	BYTE *RotBuf = new BYTE[3 * RotNumberOfLines * RotBytesPerLine];

	int RGBBytesPerLine = 3 * BytesPerLine;
	int RGBRotBytesPerLine = 3 * RotBytesPerLine;
	
	if (Rotate270) {
		for (i=0, m=0; i < NumberOfLines; i++, m+=3) {
			for (j=0, k=0; j < RGBBytesPerLine; j+=3,k++) {
				*(RotBuf + ((RotNumberOfLines - 1 - k) * RGBRotBytesPerLine) + m) = *(buf + i * RGBBytesPerLine + j);
				*(RotBuf + ((RotNumberOfLines - 1 - k) * RGBRotBytesPerLine) + m + 1) = *(buf + i * RGBBytesPerLine + j + 1);
				*(RotBuf + ((RotNumberOfLines - 1 - k) * RGBRotBytesPerLine) + m + 2) = *(buf + i * RGBBytesPerLine + j + 2);
			}
		}
	} else {
		for (i=0, m=0; i < NumberOfLines; i++, m+=3) {
			for (j=0, k=0; j < RGBBytesPerLine; j+=3,k++) {
				*(RotBuf + (k * RGBRotBytesPerLine) + m) = *(buf + (NumberOfLines - 1 - i) * RGBBytesPerLine + j);
				*(RotBuf + (k * RGBRotBytesPerLine) + m + 1) = *(buf + (NumberOfLines - 1 - i) * RGBBytesPerLine + j + 1);
				*(RotBuf + (k * RGBRotBytesPerLine) + m + 2) = *(buf + (NumberOfLines - 1 - i) * RGBBytesPerLine + j + 2);
			}
		}
	}

	switch (Padding) {
		case 3:
			for (j = 1; j <= RotNumberOfLines; j++) {
				*(RotBuf + j * RGBRotBytesPerLine - 7) = RGBBLANKPIXEL;
				*(RotBuf + j * RGBRotBytesPerLine - 8) = RGBBLANKPIXEL;
				*(RotBuf + j * RGBRotBytesPerLine - 9) = RGBBLANKPIXEL;
			}
		case 2:	// Fall through
			for (j = 1; j <= RotNumberOfLines; j++) {
				*(RotBuf + j * RGBRotBytesPerLine - 4) = RGBBLANKPIXEL;
				*(RotBuf + j * RGBRotBytesPerLine - 5) = RGBBLANKPIXEL;
				*(RotBuf + j * RGBRotBytesPerLine - 6) = RGBBLANKPIXEL;
			}
		case 1: // Fall through
			for (j = 1; j <= RotNumberOfLines; j++) {
				*(RotBuf + j * RGBRotBytesPerLine - 1) = RGBBLANKPIXEL;
				*(RotBuf + j * RGBRotBytesPerLine - 2) = RGBBLANKPIXEL;
				*(RotBuf + j * RGBRotBytesPerLine - 3) = RGBBLANKPIXEL;
			}
	}

	delete buf;

	// Switch width-height values
	*pNumberOfLines = RotNumberOfLines;
	*pBytesPerLine = RotBytesPerLine;
	return RotBuf;
}

BYTE *Rotate180RGB(BYTE *buf, int BytesPerLine, int NumberOfLines)
{
	int RGBBytesPerLine = 3 * BytesPerLine;
	
	BYTE *RotBuf = new BYTE[NumberOfLines * RGBBytesPerLine];

	DWORD dstptr = (DWORD)RotBuf + (NumberOfLines-1) * RGBBytesPerLine;

	__asm {
		mov	esi, DWORD PTR buf
		mov	edi, DWORD PTR dstptr
		mov edx, DWORD PTR RGBBytesPerLine	 
		shr edx, 1						// half a line in bytes
	$rgbrot180mirloop1:
		xor eax,eax						// left	pos
		mov ebx, DWORD PTR RGBBytesPerLine	 
		sub ebx,3							// right pos
	$rgbrot180mirloop2:
		mov cl, BYTE PTR [esi+eax]	// cl = *(bptr+left)
		mov	ch, BYTE PTR [esi+ebx]	// ch = *(bptr+right)
		mov	BYTE PTR [edi+eax],ch	
		mov BYTE PTR [edi+ebx],cl		// swap
		mov cl, BYTE PTR [esi+eax+1]	// cl = *(bptr+left)
		mov	ch, BYTE PTR [esi+ebx+1]	// ch = *(bptr+right)
		mov	BYTE PTR [edi+eax+1],ch	
		mov BYTE PTR [edi+ebx+1],cl		// swap
		mov cl, BYTE PTR [esi+eax+2]	// cl = *(bptr+left)
		mov	ch, BYTE PTR [esi+ebx+2]	// ch = *(bptr+right)
		mov	BYTE PTR [edi+eax+2],ch	
		mov BYTE PTR [edi+ebx+2],cl		// swap
		add eax, 3		 					// left+=3
		sub ebx, 3							// right-=3
		cmp eax, edx
		jl $rgbrot180mirloop2
		add esi, DWORD PTR RGBBytesPerLine	 
		sub edi, DWORD PTR RGBBytesPerLine	 
		dec DWORD PTR NumberOfLines
		jnz	$rgbrot180mirloop1
	}

	delete buf;
	return RotBuf;
}



void InvertRGB(BYTE *buf, int BytesPerLine, int NumberOfLines)
{
		
	int n = BytesPerLine * NumberOfLines * 3;

	__asm {
		mov esi, DWORD PTR buf
		mov	ecx, DWORD PTR n
		mov	ebx, 255
	$invmonoloop:
		mov	eax, ebx
		sub al, BYTE PTR [esi]
		mov BYTE PTR [esi], al
		inc esi
		loop $invmonoloop
	}
}

BOOL CreateJPEG(BYTE *image, DWORD tnw, DWORD tnh,  CString filename, int jpegquality)
{
	void *pimage = image;
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	cinfo.err = jpeg_std_error(&jerr);
	jpeg_create_compress(&cinfo);

	FILE * outfile;	
		
	TCHAR szFilename[260];
	strcpy(szFilename, filename);
	if ((outfile = fopen(szFilename, "wb")) == NULL) 
		return FALSE;

	jpeg_stdio_dest(&cinfo, outfile);
		
	// Set jpeg info parameters
	cinfo.image_width = tnw;												// Width of image, in pixels
	cinfo.image_height = tnh;												// Height of image, in pixels
	cinfo.input_components = 3;												// Number of color channels (samples per pixel)
	cinfo.in_color_space = JCS_RGB;										 	// Color space of source image
	jpeg_set_defaults(&cinfo);
	jpeg_set_quality (&cinfo, jpegquality, TRUE);
	//if (jpegprogression)
	//	jpeg_simple_progression (&cinfo);

	jpeg_start_compress(&cinfo, TRUE);

	JSAMPROW row_pointer[1];	/* pointer to a single row */
	int row_stride;			/* physical row width in buffer */

	row_stride = tnw * cinfo.input_components ;	/* JSAMPLEs per row in image_buffer */

	while (cinfo.next_scanline < cinfo.image_height) {
		row_pointer[0] = & image[cinfo.next_scanline * row_stride];
		jpeg_write_scanlines(&cinfo, row_pointer, 1);
	}
	jpeg_finish_compress(&cinfo);
//	jpeg_create_compress(&cinfo);	
	fclose(outfile);
	jpeg_destroy_compress(&cinfo);

	return TRUE;
}

struct my_error_mgr {
  struct jpeg_error_mgr pub;	/* "public" fields */

  jmp_buf setjmp_buffer;	/* for return to caller */
};

typedef struct my_error_mgr * my_error_ptr;

METHODDEF(void)
my_error_exit (j_common_ptr cinfo)
{
  /* cinfo->err really points to a my_error_mgr struct, so coerce pointer */
  my_error_ptr myerr = (my_error_ptr) cinfo->err;

  /* Always display the message. */
  /* We could postpone this until after returning, if we chose. */
  (*cinfo->err->output_message) (cinfo);

  /* Return control to the setjmp point */
  longjmp(myerr->setjmp_buffer, 1);
}

BYTE *ReadJPEG(char *filename, DWORD *tnw, DWORD *tnh)
{
	struct jpeg_decompress_struct cinfo;	
	 struct my_error_mgr jerr;
	FILE * infile;		// source file 
	JSAMPARRAY buffer;	// Output row buffer 
	int row_stride;		// physical row width in output buffer 
	BYTE	*bitarray;

	if ((infile = fopen(filename, "rb")) == NULL) {
		return FALSE;
	}

	 cinfo.err = jpeg_std_error(&jerr.pub);
	   if (setjmp(jerr.setjmp_buffer)) {
    /* If we get here, the JPEG code has signaled an error.
     * We need to clean up the JPEG object, close the input file, and return.
     */
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
    return 0;
  }
	// Now we can initialize the JPEG decompression object. 
	jpeg_create_decompress(&cinfo);
	jpeg_stdio_src(&cinfo, infile);

	(void) jpeg_read_header(&cinfo	, TRUE);
	(void) jpeg_start_decompress(&cinfo);

	*tnw = cinfo.image_width; 												// Width of image, in pixels
	*tnh = cinfo.image_height;												// Height of image, in pixels
//	*type = cinfo.output_components == 3 ? 1 : 0;					// Number of color channels (samples per pixel)
	
	row_stride = cinfo.output_width * cinfo.output_components;

	buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr) &cinfo, JPOOL_IMAGE, row_stride, 1);

	try {
		//bitarray = new BYTE[row_stride * cinfo.output_height];
		bitarray = new BYTE[3 * cinfo.output_width * cinfo.output_height];
	}
	catch (CMemoryException *ex) {
		jpeg_destroy_decompress(&cinfo);
		fclose(infile);
		return FALSE;
	}
	
	BYTE *pbuf = bitarray;

	while (cinfo.output_scanline < cinfo.output_height) {
		(void) jpeg_read_scanlines(&cinfo, buffer, 1);
		if (cinfo.output_components == 3) {
			memmove(pbuf, buffer[0], row_stride);
			pbuf +=  row_stride;
		} else {
			for (int i=0; i<cinfo.output_width; i++) {
				*pbuf++ = buffer[0][i];
				*pbuf++ = buffer[0][i];
				*pbuf++ = buffer[0][i];
			}
		}
	}

	(void) jpeg_finish_decompress(&cinfo);
	jpeg_destroy_decompress(&cinfo);

	fclose(infile);

	return bitarray;						
}

BOOL SavePyramideTileJpeg(CString sFileName)
{
	CUtils util;
	DWORD newtnw, newtnh;
	DWORD tnw = 0,tnh = 0;
	int tiles = 0;
	CString s, ss;
	int levels = 1;

	CString mcss = util.GetFileName(sFileName,TRUE);
	if (mcss == "")
		return FALSE;

	BYTE *image = ReadJPEG(sFileName.GetBuffer(255), &tnw, &tnh);
	sFileName.ReleaseBuffer();
	if (image == NULL || tnw == 0 || tnh == 0)
		return FALSE;

	int x = max(tnw, tnh);

	while (x > g_prefs.m_flashtilesize) {
		levels++;
		x /= 2;
	}
	
	s.Format("%s\\%s", g_prefs.m_previewPath, mcss); 

	if (util.CheckFolder(s) == FALSE)
		::CreateDirectory(s, NULL);
	else {
		util.DeleteFiles(s + "\\TileGroup0", "*.*");
		util.DeleteFiles(s, "*.*");
	}

	s.Format("%s\\%s\\TileGroup0", g_prefs.m_previewPath, mcss); 
	::CreateDirectory(s, NULL);

	DWORD orgtnw = tnw;
	DWORD orgtnh = tnh;
	for (int i=0; i<levels; i++) {
		int tilesWritten = SaveTiledJpeg(s, image, tnw, tnh, levels - i - 1, g_prefs.m_flashtilesize);
		if (tilesWritten == 0)
			return FALSE;
		tiles += tilesWritten;
		if ((int)tnw <= g_prefs.m_flashtilesize && (int)tnh <= g_prefs.m_flashtilesize)
			break;
		newtnw = tnw/2;
		newtnh = tnh/2;
		image = ScaleTo (image, tnw, tnh, tnw/2, tnh/2);
		tnw /= 2;
		tnh /= 2;
	}

	s.Format("%s\\%s\\ImageProperties.xml", g_prefs.m_previewPath, mcss); 
	ss.Format("<IMAGE_PROPERTIES WIDTH=\"%d\" HEIGHT=\"%d\" NUMTILES=\"%d\" NUMIMAGES=\"1\" VERSION=\"1.8\" g_prefs.m_flashtilesize=\"%d\" />", orgtnw, orgtnh, tiles, g_prefs.m_flashtilesize);
	HANDLE hndl = ::CreateFile(s, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 0, 0);
	DWORD dwBytesWritten;
	WriteFile(hndl, ss.GetBuffer(255), ss.GetLength(), &dwBytesWritten, NULL);
	CloseHandle(hndl);

	return TRUE;
}

int SaveTiledJpeg(CString filePath, BYTE *image, DWORD tnw, DWORD tnh, int level, int tilesize)
{
	int tilesacross = (tnw+tilesize-1)/tilesize;
	int tilesdown = (tnh+tilesize-1)/tilesize;

	CString sFileName;
	int x = 0;
	int y = 0;
	sFileName.Format("%s\\%d-%d-%d.jpg", filePath, level, x, y);
	
	// Last preview (untiled thumbnail) ??	
	if (tilesacross == 1 && tilesdown == 1) {
		if (CreateJPEG(image, tnw, tnh, sFileName, 100) == FALSE)
			return 0;
		else
			return 1;
	}

	int xpos = 0;
	int ypos = 0;
	BYTE *buf = NULL;

	try {
		buf = new BYTE[tilesize*tilesize*3];
	}
	catch (CMemoryException *ex) {
		return FALSE;
	}
	int tileswritten = 0;
	for (y=0; y<tilesdown; y++) {
		int ysize = tilesize;
		if (ypos+g_prefs.m_flashtilesize > tnh)
			ysize = tnh - ypos;		// residual
		xpos = 0;
		for (x=0; x<tilesacross; x++) {				
			sFileName.Format("%s\\%d-%d-%d.jpg", filePath, level, x, y);
			int xsize = tilesize;

			if (xpos+g_prefs.m_flashtilesize > tnw)
				xsize = tnw - xpos; 	// residual

			memset(buf, 0xFF, tilesize*tilesize*3);

			BYTE *p = image + 3*ypos*tnw + xpos*3;
			BYTE *bufp = buf;
			for (int z=0;z<ysize; z++) {
				memcpy(bufp, p, 3*xsize);
				p += 3*tnw;
				bufp += 3*xsize;
			}
			if (CreateJPEG(buf, xsize, ysize, sFileName, 100) == FALSE)
				return 0;
			tileswritten++;
			xpos += xsize;
		}
		ypos += ysize;
	}


	delete [] buf;

	return tileswritten;
}

BYTE  *CMYKInterleavedToRGB(BYTE *cmykthumbnail, DWORD tnw, DWORD tnh)
{
	
	//Logprintf("INFO:  Converting CMYK interleaved to RGB\r\n");

	BYTE *rgbthumbnail = NULL;
	
	try {
		rgbthumbnail = new BYTE[3*tnw*tnh];
	}
	catch (CException*ex) {
//		Logprintf("ERROR: Not enough memory - CMYKInterleavedToRGB()\r\n");
		return (FALSE);
	}
	
	DWORD nPixels = tnw*tnh;

	__asm {
		mov ecx, 255
		mov	edi, DWORD PTR rgbthumbnail
		mov esi, DWORD PTR cmykthumbnail
	$mergeloop:
		
		mov ebx, 255				// k_tmp = 255 - k
		sub bl, BYTE PTR [esi+3]
		cmp ebx, 0					// if k == 0 -> All black
		jne $notallblack			
		mov BYTE PTR [edi], 0
		mov BYTE PTR [edi+1], 0
		mov BYTE PTR [edi+2], 0
		add edi, 3
		jmp $tonext
	$notallblack:
		
		mov eax, 255			
		sub al, BYTE PTR [esi]		// c_tmp = 255 - c
	
		mul ebx						// r = c_tmp * k_tmp / 255
		xor edx,edx					
		div ecx
		mov BYTE PTR [edi],al		// Store R
		inc edi

		mov eax, 255				// m_tmp = 255 - m
		sub al, BYTE PTR [esi+1]		// g = m_tmp * k_tmp / 255
		
		mul ebx
		xor edx,edx
		div ecx
		mov BYTE PTR [edi],al		// Store G
		inc edi

		mov eax, 255		
		sub al, BYTE PTR [esi+2]		// y_tmp = 255 - y
		mul ebx						// b = y_tmp * k_tmp / 255
		xor edx,edx
		div ecx
		mov BYTE PTR [edi],al		// Store B
		inc edi
		
	$tonext:
		add esi,4
		dec DWORD PTR nPixels
		cmp DWORD PTR nPixels,0
		jnz $mergeloop
	}
	delete cmykthumbnail;

	return rgbthumbnail;
}

BOOL CMYKInterleaved2Separated(BYTE *cmykimage, BYTE *c_buf, BYTE *m_buf, BYTE *y_buf, BYTE *k_buf, DWORD tnw, DWORD tnh)
{
//	Logprintf("INFO:  Converting CMYK interleaved to CMYK separated\r\n");
	
	
//	c_buf = new BYTE[tnw*tnh];
//	m_buf = new BYTE[tnw*tnh];
//	y_buf = new BYTE[tnw*tnh];
//	k_buf = new BYTE[tnw*tnh];
	
	if (c_buf == NULL || m_buf == NULL || y_buf == NULL || k_buf == NULL) {
		return FALSE;
	}
	
	DWORD nPixels = tnw*tnh;

	BYTE *pSrc = cmykimage;
	BYTE *pDstC = c_buf;
	BYTE *pDstM = m_buf;
	BYTE *pDstY = y_buf;
	BYTE *pDstK = k_buf;

	for (DWORD x=0; x<nPixels; x++) {
		*pDstC++ = *pSrc++;
		*pDstM++ = *pSrc++;
		*pDstY++ = *pSrc++;
		*pDstK++ = *pSrc++;
	}

	return TRUE;
}

BYTE  *GenerateHeatMap(BYTE *c_thumbnail, BYTE *m_thumbnail, BYTE *y_thumbnail, BYTE *k_thumbnail, DWORD tnw, DWORD tnh)
{
	BYTE *thumbnail = NULL;
//	Logprintf("INFO:  Generating heat-map\r\n");
	try {
		thumbnail = new BYTE[4*tnw*tnh];
	}
	catch (CException*ex) {
//		Logprintf("ERROR: Not enough memory  GenerateHeatMap()\r\n");
		return (FALSE);
	}

	BYTE *pC = c_thumbnail;
	BYTE *pM = m_thumbnail;
	BYTE *pY = y_thumbnail;
	BYTE *pK = k_thumbnail;
	BYTE *pScanHeatmap = thumbnail;

	DWORD nBytes = tnh * tnw; 

	for(int x=0; x < nBytes; x++ ) {
		int sum = 0;
		if (pC != NULL) {
			sum += *pC++;
		}
		
		if (pM != NULL) {
			sum += *pM++;
		}
		if (pY != NULL)
		{
			sum += *pY++;
		}
		if (pK != NULL)
		{
			sum += *pK++;
		}

		sum *= 400;
		sum /= 1020;

		if (sum == 0) {
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)255;
		} else if (sum > 0 && sum < 50) {
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)194;
		} else if (sum > 0 && sum < 100) {
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)128;
		} else if  (sum >= 100 && sum < 150) {
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)64;
		} else if  (sum >= 150 && sum < 200) {
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)0;
		} else if  (sum >= 200 && sum < 225) {
			*pScanHeatmap++ = (BYTE)255;
			//*pScanHeatmap++ = (BYTE)170;
			*pScanHeatmap++ = (BYTE)192;
			*pScanHeatmap++ = (BYTE)64;
		} else if  (sum >= 225 && sum < 250) {
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)128;
			*pScanHeatmap++ = (BYTE)0;
		} else if  (sum >= 250 && sum < 275) {
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)64;
			*pScanHeatmap++ = (BYTE)0;
		} else if  (sum >= 275) {
			*pScanHeatmap++ = (BYTE)255;
			*pScanHeatmap++ = (BYTE)0;
			*pScanHeatmap++ = (BYTE)0;
		}
	}

	return thumbnail;
}



/*
BYTE *Filter5x5(BYTE *thumbnail, DWORD tnw, DWORD tnh)
{
	int colors = 1;
	int bpl = tnw * colors;
	int sum, fsum;
	BYTE *newthumbnail;
	try {
		newthumbnail = new BYTE[bpl*tnh];
	}
	catch (CMemoryException *ex) {
		return thumbnail;
	}

	BYTE *dst = newthumbnail;
	
	BYTE *src = thumbnail;
	int intFilter[25];

	double filter_coef[25] = {-0.0035,-0.0159,-0.0262,-0.0159,-0.0035,  -0.0159,-0.0712,-0.1173,-0.0712,-0.0159,  -0.0262,-0.1173,2.0,-0.1173,-0.0262, -0.0159,-0.0712,-0.1173,-0.0712,-0.0159,-0.0035,-0.0159,-0.0262,-0.0159,-0.0035};

	for (int y=0;y<tnh;y++) {
		for (int x=0;x<bpl;x++) {
			if (y<2 || y>=tnh-2 || x<2 || x>=bpl-2) {
				*dst++ = *src++;
			} else {

				fsum = filter_coef[0]*(double)(*(src-2*bpl-2)) + 
					  filter_coef[1]*(double)(*(src-2*bpl-1)) + 
					  filter_coef[2]*(double)(*(src-2*bpl)) + 
					  filter_coef[3]*(double)(*(src-2*bpl+1)) + 
					  filter_coef[4]*(double)(*(src-2*bpl+2)) +
	
					  filter_coef[5]*(double)(*(src-bpl-2)) + 
					  filter_coef[6]*(double)(*(src-bpl-1)) + 
					  filter_coef[7]*(double)(*(src-bpl)) + 
					  filter_coef[8]*(double)(*(src-bpl+1)) + 
					  filter_coef[9]*(double)(*(src-bpl+2)) +

					  filter_coef[10]*(double)(*(src-2)) + 
					  filter_coef[11]*(double)(*(src-1)) + 
					  filter_coef[12]*(double)(*(src)) +
					  filter_coef[13]*(double)(*(src+1)) + 
					  filter_coef[14]*(double)(*(src+2)) + 
				
					  filter_coef[15]*(double)(*(src+bpl-2)) + 
					  filter_coef[16]*(double)(*(src+bpl-1)) + 
					  filter_coef[17]*(double)(*(src+bpl)) + 
					  filter_coef[18]*(double)(*(src+bpl+1)) +
					  filter_coef[19]*(double)(*(src+bpl+2)) +

					  filter_coef[20]*(double)(*(src+2*bpl-2)) + 
					  filter_coef[21]*(double)(*(src+2*bpl-1)) + 
					  filter_coef[22]*(double)(*(src+2*bpl)) + 
					  filter_coef[23]*(double)(*(src+2*bpl+1)) +
					  filter_coef[24]*(double)(*(src+2*bpl+2));

					 sum = fsum;
				if (sum<0)
					*dst = (BYTE)0;
				else if (sum>255)
					*dst = (BYTE)255;
				else
					*dst = (BYTE)sum;
				dst++;
				src++;
			}
			
		}
	}
	delete thumbnail;

	return newthumbnail;
}
*/



/*
BOOL SetTiffTagPhotometric(CString sInputFile, void *pValue)
{
	CUtils util;

	char *tagname;

	
	TCHAR szFilename[MAX_PATH];
	strcpy(szFilename, sInputFile);
	TIFF *tiff = TIFFOpen(szFilename, "r+");
	if (tiff == NULL) {
		util.Logprintf("ERROR: SetTiffTag() - Unable to open TIFF file %s", szFilename);
        return FALSE;
	 }


	int ret = TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC,(short)pValue);
	
	if (ret != 1) {
		util.Logprintf("ERROR: SetTiffTag() - TIFFSetField returned false ");
		TIFFClose(tiff);
		return FALSE;
	}
	TIFFRewriteDirectory(tiff);

	TIFFClose(tiff);
	
	return TRUE;

}
*/