#pragma once
#include <math.h> 

#include "Filters.h" 

typedef struct 
{ 
   double *Weights;  // Normalized weights of neighboring pixels
   int Left,Right;   // Bounds of source pixels window
} ContributionType;  // Contirbution information for a single pixel

typedef struct 
{ 
   ContributionType *ContribRow; // Row (or column) of contribution weights 
   UINT WindowSize,              // Filter window size (of affecting source pixels) 
        LineLength;              // Length of line (no. or rows / cols) 
} LineContribType;               // Contribution information for an entire line (row or column)

typedef BOOL (*ProgressAnbAbortCallBack)(BYTE bPercentComplete);

template <class FilterClass>
class C2PassScale 
{
public:

    C2PassScale (ProgressAnbAbortCallBack callback = NULL) : 
        m_Callback (callback) {}

    virtual ~C2PassScale() {}

    BYTE * AllocAndScale (  
        BYTE   *pOrigImage, 
        UINT        uOrigWidth, 
        UINT        uOrigHeight, 
        UINT        uNewWidth, 
        UINT        uNewHeight);

   BYTE  * Scale (  
        BYTE   *pOrigImage, 
        UINT        uOrigWidth, 
        UINT        uOrigHeight, 
        BYTE   *pDstImage,
        UINT        uNewWidth, 
        UINT        uNewHeight);

private:

    ProgressAnbAbortCallBack    m_Callback;
    BOOL                        m_bCanceled;

    LineContribType *AllocContributions (   UINT uLineLength, 
                                            UINT uWindowSize);

    void FreeContributions (LineContribType * p);

    LineContribType *CalcContributions (    UINT    uLineSize, 
                                            UINT    uSrcSize, 
                                            double  dScale);

    void ScaleRow ( BYTE           *pSrc, 
                    UINT                uSrcWidth,
                    BYTE           *pRes, 
                    UINT                uResWidth,
                    UINT                uRow, 
                    LineContribType    *Contrib);

    void HorizScale (   BYTE           *pSrc, 
                        UINT                uSrcWidth,
                        UINT                uSrcHeight,
                        BYTE           *pDst,
                        UINT                uResWidth,
                        UINT                uResHeight);

    void ScaleCol ( BYTE           *pSrc, 
                    UINT                uSrcWidth,
                    BYTE           *pRes, 
                    UINT                uResWidth,
                    UINT                uResHeight,
                    UINT                uCol, 
                    LineContribType    *Contrib);

    void VertScale (    BYTE           *pSrc, 
                        UINT                uSrcWidth,
                        UINT                uSrcHeight,
                        BYTE           *pDst,
                        UINT                uResWidth,
                        UINT                uResHeight);
};

template <class FilterClass>
LineContribType *
C2PassScale<FilterClass>::
AllocContributions (UINT uLineLength, UINT uWindowSize)
{
    LineContribType *res = new LineContribType; 
        // Init structure header 
    res->WindowSize = uWindowSize; 
    res->LineLength = uLineLength; 
        // Allocate list of contributions 
    res->ContribRow = new ContributionType[uLineLength];
    for (UINT u = 0 ; u < uLineLength ; u++) 
    {
        // Allocate contributions for every pixel
        res->ContribRow[u].Weights = new double[uWindowSize];
    }
    return res; 
} 
 
template <class FilterClass>
void
C2PassScale<FilterClass>::
FreeContributions (LineContribType * p)
{ 
    for (UINT u = 0; u < p->LineLength; u++) 
    {
        // Free contribs for every pixel
        delete [] p->ContribRow[u].Weights;
    }
    delete [] p->ContribRow;    // Free list of pixels contribs
    delete p;                   // Free contribs header
} 
 
template <class FilterClass>
LineContribType *
C2PassScale<FilterClass>::
CalcContributions (UINT uLineSize, UINT uSrcSize, double dScale)
{ 
    FilterClass CurFilter;

    double dWidth;
    double dFScale = 1.0;
    double dFilterWidth = CurFilter.GetWidth();

    if (dScale < 1.0) 
    {    // Minification
        dWidth = dFilterWidth / dScale; 
        dFScale = dScale; 
    } 
    else
    {    // Magnification
        dWidth= dFilterWidth; 
    }
 
    // Window size is the number of sampled pixels
    int iWindowSize = 2 * (int)ceil(dWidth) + 1; 
 
    // Allocate a new line contributions strucutre
    LineContribType *res = AllocContributions (uLineSize, iWindowSize); 
 
    for (UINT u = 0; u < uLineSize; u++) 
    {   // Scan through line of contributions
        double dCenter = (double)u / dScale;   // Reverse mapping
        // Find the significant edge points that affect the pixel
        int iLeft = max (0, (int)floor (dCenter - dWidth)); 
        res->ContribRow[u].Left = iLeft; 
        int iRight = min ((int)ceil (dCenter + dWidth), int(uSrcSize) - 1); 
        res->ContribRow[u].Right = iRight;
 
        // Cut edge points to fit in filter window in case of spill-off
        if (iRight - iLeft + 1 > iWindowSize) 
        {
            if (iLeft < (int(uSrcSize) - 1 / 2)) 
            {
                iLeft++; 
            }
            else 
            {
                iRight--; 
            }
        }
        double dTotalWeight = 0.0;  // Zero sum of weights
        for (int iSrc = iLeft; iSrc <= iRight; iSrc++)
        {   // Calculate weights
            dTotalWeight += (res->ContribRow[u].Weights[iSrc-iLeft] =  
                                dFScale * CurFilter.Filter (dFScale * (dCenter - (double)iSrc))); 
        }
        ASSERT (dTotalWeight >= 0.0);   // An error in the filter function can cause this 
        if (dTotalWeight > 0.0)
        {   // Normalize weight of neighbouring points
            for (int iSrc = iLeft; iSrc <= iRight; iSrc++)
            {   // Normalize point
                res->ContribRow[u].Weights[iSrc-iLeft] /= dTotalWeight; 
            }
        }
   } 
   return res; 
} 
 
 
template <class FilterClass>
void 
C2PassScale<FilterClass>::
ScaleRow (  BYTE           *pSrc, 
            UINT                uSrcWidth,
            BYTE           *pRes, 
            UINT                uResWidth,
            UINT                uRow, 
            LineContribType    *Contrib)
{
    BYTE *pSrcRow = &(pSrc[3 * uRow * uSrcWidth]);
    BYTE *pDstRow = &(pRes[3 * uRow * uResWidth]);
    for (UINT x = 0; x < uResWidth; x++) 
    {   // Loop through row
        BYTE r = 0;
        BYTE g = 0;
        BYTE b = 0;
        int iLeft = Contrib->ContribRow[x].Left;    // Retrieve left boundries
        int iRight = Contrib->ContribRow[x].Right;  // Retrieve right boundries
        for (int i = iLeft; i <= iRight; i++)
        {   // Scan between boundries
            // Accumulate weighted effect of each neighboring pixel
//            r += (BYTE)(Contrib->ContribRow[x].Weights[i-iLeft] * (double)(pSrcRow[i])); 

            r += (BYTE)(Contrib->ContribRow[x].Weights[i-iLeft] * (double)pSrcRow[3*i]); 
            g += (BYTE)(Contrib->ContribRow[x].Weights[i-iLeft] * (double)pSrcRow[3*i+1]); 
            b += (BYTE)(Contrib->ContribRow[x].Weights[i-iLeft] * (double)pSrcRow[3*i+2]); 



        } 
        pDstRow[3*x] = r; // Place result in destination pixel
        pDstRow[3*x+1] = g; // Place result in destination pixel
        pDstRow[3*x+2] = b; // Place result in destination pixel
    } 
} 

template <class FilterClass>
void
C2PassScale<FilterClass>::
HorizScale (    BYTE           *pSrc, 
                UINT                uSrcWidth,
                UINT                uSrcHeight,
                BYTE           *pDst, 
                UINT                uResWidth,
                UINT                uResHeight)
{ 
    TRACE ("Performing horizontal scaling...\n"); 
    if (uResWidth == uSrcWidth)
    {   // No scaling required, just copy
        memcpy (pDst, pSrc, 3 * sizeof (BYTE) * uSrcHeight * uSrcWidth);
    }
    // Allocate and calculate the contributions
    LineContribType * Contrib = CalcContributions (uResWidth, uSrcWidth, double(uResWidth) / double(uSrcWidth)); 
    for (UINT u = 0; u < uResHeight; u++)
    {   // Step through rows
        if (NULL != m_Callback)
        {
            //
            // Progress and report callback supplied
            //
            if (!m_Callback (BYTE(double(u) / double (uResHeight) * 50.0)))
            {
                //
                // User wished to abort now
                //
                m_bCanceled = TRUE;
                FreeContributions (Contrib);  // Free contributions structure
                return;
            }
        }
                
        ScaleRow (  pSrc, 
                    uSrcWidth,
                    pDst,
                    uResWidth,
                    u,
                    Contrib);    // Scale each row 
    }
    FreeContributions (Contrib);  // Free contributions structure
} 
 
template <class FilterClass>
void
C2PassScale<FilterClass>::
ScaleCol (  BYTE           *pSrc, 
            UINT                uSrcWidth,
            BYTE           *pRes, 
            UINT                uResWidth,
            UINT                uResHeight,
            UINT                uCol, 
            LineContribType    *Contrib)
{ 
    for (UINT y = 0; y < uResHeight; y++) 
    {    // Loop through column
        BYTE r = 0;
        BYTE g = 0;
        BYTE b = 0;

        int iLeft = Contrib->ContribRow[y].Left;    // Retrieve left boundries
        int iRight = Contrib->ContribRow[y].Right;  // Retrieve right boundries
        for (int i = iLeft; i <= iRight; i++)
        {   // Scan between boundries
            // Accumulate weighted effect of each neighboring pixel
            r += BYTE(Contrib->ContribRow[y].Weights[i-iLeft] * (double)pSrc[3 * i * uSrcWidth + 3*uCol]);
            g += BYTE(Contrib->ContribRow[y].Weights[i-iLeft] * (double)pSrc[3 * i * uSrcWidth + 3*uCol + 1]);
            b += BYTE(Contrib->ContribRow[y].Weights[i-iLeft] * (double)pSrc[3 * i * uSrcWidth + 3*uCol + 2]);
        }
        pRes[3 * y * uResWidth + 3*uCol] = r;   // Place result in destination pixel
        pRes[3 * y * uResWidth + 3*uCol + 1] = g;   // Place result in destination pixel
        pRes[3 * y * uResWidth + 3*uCol + 2] = b;   // Place result in destination pixel
    }
} 
 

template <class FilterClass>
void
C2PassScale<FilterClass>::
VertScale ( BYTE           *pSrc, 
            UINT                uSrcWidth,
            UINT                uSrcHeight,
            BYTE           *pDst, 
            UINT                uResWidth,
            UINT                uResHeight)
{ 
    TRACE ("Performing vertical scaling..."); 

    if (uSrcHeight == uResHeight)
    {   // No scaling required, just copy
        memcpy (pDst, pSrc, 3 * sizeof (BYTE) * uSrcHeight * uSrcWidth);
    }
    // Allocate and calculate the contributions
    LineContribType * Contrib = CalcContributions (uResHeight, uSrcHeight, double(uResHeight) / double(uSrcHeight)); 
    for (UINT u = 0; u < uResWidth; u++)
    {   // Step through columns
        if (NULL != m_Callback)
        {
            //
            // Progress and report callback supplied
            //
            if (!m_Callback (BYTE(double(u) / double (uResWidth) * 50.0) + 50))
            {
                //
                // User wished to abort now
                //
                m_bCanceled = TRUE;
                FreeContributions (Contrib);  // Free contributions structure
                return;
            }
        }
        ScaleCol (  pSrc,
                    uSrcWidth,
                    pDst,
                    uResWidth,
                    uResHeight,
                    u,
                    Contrib);   // Scale each column
    }
    FreeContributions (Contrib);     // Free contributions structure
} 

template <class FilterClass>
BYTE *
C2PassScale<FilterClass>::
AllocAndScale ( 
    BYTE   *pOrigImage, 
    UINT        uOrigWidth, 
    UINT        uOrigHeight, 
    UINT        uNewWidth, 
    UINT        uNewHeight)
{ 
    // Scale source image horizontally into temporary image
    m_bCanceled = FALSE;
    BYTE *pTemp = new BYTE [3 * uNewWidth * uOrigHeight];
    HorizScale (pOrigImage, 
                uOrigWidth,
                uOrigHeight,
                pTemp,
                uNewWidth,
                uOrigHeight);
    if (m_bCanceled)
    {
        delete [] pTemp;
        return NULL;
    }
    // Scale temporary image vertically into result image    
    BYTE *pRes = new BYTE [3 * uNewWidth * uNewHeight];
    VertScale ( pTemp,
                uNewWidth,
                uOrigHeight,
                pRes,
                uNewWidth,
                uNewHeight);
    if (m_bCanceled)
    {
        delete [] pTemp;
        delete [] pRes;
        return NULL;
    }
    delete [] pTemp;
    return pRes;
} 

template <class FilterClass>
BYTE *
C2PassScale<FilterClass>::
Scale ( 
    BYTE   *pOrigImage, 
    UINT        uOrigWidth, 
    UINT        uOrigHeight, 
    BYTE   *pDstImage, 
    UINT        uNewWidth, 
    UINT        uNewHeight)
{ 
    // Scale source image horizontally into temporary image
    m_bCanceled = FALSE;
    BYTE *pTemp = new BYTE [3 * uNewWidth * uOrigHeight];
    HorizScale (pOrigImage, 
                uOrigWidth,
                uOrigHeight,
                pTemp,
                uNewWidth,
                uOrigHeight);
    if (m_bCanceled)
    {
        delete [] pTemp;
        return NULL;
    }
	
    // Scale temporary image vertically into result image    
    VertScale ( pTemp,
                uNewWidth,
                uOrigHeight,
                pDstImage,
                uNewWidth,
                uNewHeight);
    delete [] pTemp;
    if (m_bCanceled)
    {
        return NULL;
    }
    return pDstImage;
} 


