/*!
**
** \file ptbExport.h
**
** \brief callas pdfEngine SDK: Export actions
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_EXPORT_H)
/*! \def PTB_EXPORT_H header define */
#define PTB_EXPORT_H

/******************************************************************************/

#include "ptbTypes.h"
#include "ptbProgress.h"

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** Export
*/

/*!
** \brief  Postscript level
*/
enum PTB_EPostScriptLevel
{ 
	  PTB_eLevel2
	, PTB_eLevel3
};

/*!
** \brief  EPS flags
*/
enum PTB_EEPSFlags { PTB_eEPSNone                 = 0x00    /*!< None */
                   , PTB_eEPSAddPageInfo          = 0x01    /*!< Add page information */
                   , PTB_eEPSAddColorBars         = 0x02    /*!< Add color bars */
                   , PTB_eEPSAddRegistrationMarks = 0x04    /*!< Add registration marks */
                   , PTB_eEPSAddCutMarks          = 0x08    /*!< Adds cutmarks */
                   , PTB_eEPSApplyColormanagement = 0x10    /*!< Apply host based color management */
                   , PTB_eEPSApplyOutputPreview   = 0x20    /*!< Apply output preview settings - only applied if 'PTB_eEPSApplyColormanagement' is used */
                   , PTB_eEPSSimulateOverprint    = 0x40    /*!< Simulate overprint */
                   , PTB_eEPSWriteASCII           = 0x80    /*!< Writes in 'Clean7Bit' ASCII */
                   };

/*!
** \brief  Callback for PTB_SaveAsEPS. Called for every created document.
*/
typedef void(*PTB_SaveAsEPSCB)( PTB_StringID    idPath    /*!< Created file */
                              , PTB_int32_t     page      /*!< Page */
                              , void*           userData  /*!< User data */
                              );

/*!
** \brief Save as EPS
**        Converts the PDF into EPS.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsEPS( const PTB_Path_t*          src                        /*!< Document */
             , enum PTB_EPostScriptLevel  postscriptLevel            /*!< PostScript level [2|3] */
             , PTB_uint32_t               flags                      /*!< Flags (any combination of PTB_EEPSFlags values) */
             , PTB_float_t                lineWeight                 /*!< Line weight of cut marks in pt */
             , const PTB_utf8_char_t*     simulationProfile          /*!< Simulation profile, default: 'ISO Coated v2 (ECI)' only applied if 'PTB_eEPSApplyColormanagement' is used */
             , PTB_uint16_t               transparencyQuality        /*!< Transparency quality in % */
             , PTB_uint32_t               gradientResolution         /*!< Gradient resolution in ppi */
             , PTB_uint32_t               bitmapResolution           /*!< Bitmap resolution in ppi */
             , PTB_uint32_t               firstPage                  /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
             , PTB_uint32_t               lastPage                   /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
             , const PTB_Path_t*          destFolder                 /*!< Destination folder path (including folder name) */
             , PTB_SaveAsEPSCB            cbSaveAsEPS                /*!< Callback function to be called for every created PDF, might be NULL */
             , void*                      cbSaveAsEPSUserData        /*!< User data submitted to callback function */
             , PTB_ProgressCB             cbProgress                 /*!< Progress CB function to be called during processing, might be NULL */
             , void*                      cbProgressUserData         /*!< User data submitted to progress callback function */
             );


/*!
** \brief  Image format
*/
enum PTB_EImageFormat { PTB_eJPEG                 /*!< JPEG */
                      , PTB_ePNG                  /*!< PNG */
                      , PTB_eTIFF                 /*!< TIFF */
                      , PTB_eImgPDF               /*!< PDF */
                      };

/*!
** \brief  JPEG format
*/
enum PTB_EJPEGFormat { PTB_eBaseline_Standard     /*!< Baseline_Standard */
                     , PTB_eProgressive_3_Scan    /*!< Progressive_3_Scan */
                     };

/*!
** \brief  JPEG, PDF or TIFF compression
*/
enum PTB_ECompression { PTB_eJPEG_low             /*!< JPEG, PDF: low compression (Quality: high)        */
                      , PTB_eJPEG_medium          /*!< JPEG, PDF: medium compression (Quality: medium)   */
                      , PTB_eJPEG_high            /*!< JPEG, PDF: high compression (Quality: low)        */
                      , PTB_eJPEG_minimum         /*!< JPEG, PDF: minimum compression (Quality: maximum) */
                      , PTB_eJPEG_maximum         /*!< JPEG, PDF: maximum compression (Quality: minimum) */
                      , PTB_ePDF_flate            /*!< PDF: flate (lossless)                             */
                      , PTB_eTIFF_none            /*!< TIFF: None (lossless)                             */
                      , PTB_eTIFF_lzw             /*!< TIFF: LZW (lossless)                              */
                      , PTB_eTIFF_flate           /*!< TIFF: flate (lossless)                            */
                      };

/*!
** \brief  Image color space
*/
enum PTB_EColorSpace { PTB_eCMYK                  /*!< CMYK - NOTE: Currently not supported for PNG */
                     , PTB_eRGB                   /*!< RGB */
                     , PTB_eGray                  /*!< Gray */
                     , PTB_eDeviceN               /*!< CMYK + Spot (Multichannel, TIFF only) */
                     , PTB_eRGBA                  /*!< RGB with alpha channel (PNG only) */
                     };


/*!
** \brief  Image flags
*/
enum PTB_EImageFlags { PTB_eImgNone                   = 0x00    /*!< None               */
                     , PTB_eImgSmoothLineArt          = 0x01    /*!< Smooth line art    */
                     , PTB_eImgSmoothImages           = 0x02    /*!< Smooth images      */
                     , PTB_eImgSmoothText             = 0x04    /*!< Smooth text        */
                     , PTB_eImgSmoothAll              = PTB_eImgSmoothLineArt | PTB_eImgSmoothImages | PTB_eImgSmoothText
                     , PTB_eImgSimulateOverprint      = 0x08    /*!< Simulate overprint */
                     , PTB_eImgSimulatePaperColor     = 0x10    /*!< Simulate paper color - only applied if 'PTB_eImgSimulateOverprint' is used */
                     , PTB_eImgBlackpointCompensation = 0x20    /*!< Use blackpoint compensation - only applied if 'PTB_eImgSimulateOverprint' is used */
					 , PTB_eImgNoThinLineHeuristics   = 0x40    /*!< Disable thin line heuristics */
					 , PTB_eImgExtractClippingPath    = 0x80    /*!< Extract clipping path information from eps files (TIFF only) */
					 };


/*!
** \brief  A page geometry box
*/
enum PTB_EPageBox { PTB_eImgCropBox            /*!< CropBox of page */
                  , PTB_eImgTrimBox            /*!< TrimBox of page */
                  , PTB_eImgBleedBox           /*!< BleedBox of page */
                  , PTB_eImgMediaBox           /*!< MediaBox of page */
                  , PTB_eImgArtBox             /*!< ArtBox of page; Not implmented yet for PTB_SaveAsImage5 */
                  };

/*!
** \brief  Callback for PTB_SaveAsImage. Called for every created document.
*/
typedef void(*PTB_SaveAsImageCB)( PTB_StringID    idPath    /*!< Created file */
                                , PTB_int32_t     page      /*!< Page */
                                , void*           userData  /*!< User data */
                                );

/*!
** \brief  Save as Image
**         Renders an image per page preserving the page aspect ratio.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsImage( const PTB_Path_t*          src                   /*!< Document */
               , enum PTB_EImageFormat      imageFormat           /*!< JPEG, PNG, TIFF or PDF */
               , const PTB_sys_char_t*      resolution            /*!< Resolution in ppi (e.g. 72) or width x height in pixel (e.g. 1024x800) */
               , enum PTB_EJPEGFormat       jpegFormat            /*!< JPEG format - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
               , enum PTB_ECompression      compression           /*!< Compression - only applied if 'imageFormat' is PTB_eJPEG, PTB_ePDF or PTB_eTIFF */
               , PTB_uint32_t               firstPage             /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
               , PTB_uint32_t               lastPage              /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
               , const PTB_Path_t*          destFolder            /*!< Destination folder path (including folder name) */
               , PTB_SaveAsImageCB          cbSaveAsImage         /*!< Callback function to be called for every created image, might be NULL */
               , void*                      cbSaveAsImageUserData /*!< User data submitted to callback function */
               , PTB_ProgressCB             cbProgress            /*!< Progress CB function to be called during processing, might be NULL */
               , void*                      cbProgressUserData    /*!< User data submitted to progress callback function */
               );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsImage2( const PTB_Path_t*          src                   /*!< Document */
                , enum PTB_EImageFormat      imageFormat           /*!< JPEG, PNG, TIFF or PDF */
                , PTB_uint32_t               resolution            /*!< Resolution in ppi (e.g. 72) */
                , PTB_uint32_t               width                 /*!< Width in pixel - only applied if 'resolution' equal 0 */
                , PTB_uint32_t               height                /*!< Height in pixel - only applied if 'resolution' equal 0 */
                , enum PTB_EJPEGFormat       jpegFormat            /*!< JPEG format - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
                , enum PTB_ECompression      compression           /*!< Compression - only applied if 'imageFormat' is PTB_eJPEG, PTB_ePDF or PTB_eTIFF */
                , enum PTB_EColorSpace       colorspace            /*!< Color space */
                , PTB_uint32_t               firstPage             /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
                , PTB_uint32_t               lastPage              /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                , const PTB_Path_t*          destFolder            /*!< Destination folder path (including folder name) */
                , PTB_SaveAsImageCB          cbSaveAsImage         /*!< Callback function to be called for every created image, might be NULL */
                , void*                      cbSaveAsImageUserData /*!< User data submitted to callback function */
                , PTB_ProgressCB             cbProgress            /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData    /*!< User data submitted to progress callback function */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsImage3( const PTB_Path_t*          src                   /*!< Document */
                , enum PTB_EImageFormat      imageFormat           /*!< JPEG, PNG, TIFF or PDF */
                , PTB_uint32_t               resolution            /*!< Resolution in ppi (e.g. 72) */
                , PTB_uint32_t               width                 /*!< Width in pixel - only applied if 'resolution' equal 0 */
                , PTB_uint32_t               height                /*!< Height in pixel - only applied if 'resolution' equal 0 */
                , enum PTB_EJPEGFormat       jpegFormat            /*!< JPEG format - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
                , enum PTB_ECompression      compression           /*!< Compression - only applied if 'imageFormat' is PTB_eJPEG, PTB_ePDF or PTB_eTIFF */
                , enum PTB_EColorSpace       colorspace            /*!< Color space */
                , PTB_uint32_t               flags                 /*!< Flags (any combination of PTB_EImageFlags values) */
                , PTB_uint32_t               firstPage             /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
                , PTB_uint32_t               lastPage              /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                , const PTB_Path_t*          destFolder            /*!< Destination folder path (including folder name) */
                , PTB_SaveAsImageCB          cbSaveAsImage         /*!< Callback function to be called for every created image, might be NULL */
                , void*                      cbSaveAsImageUserData /*!< User data submitted to callback function */
                , PTB_ProgressCB             cbProgress            /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData    /*!< User data submitted to progress callback function */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsImage4( const PTB_Path_t*          src                   /*!< Document */
                , enum PTB_EImageFormat      imageFormat           /*!< JPEG, PNG, TIFF or PDF */
                , PTB_uint32_t               resolution            /*!< Resolution in ppi (e.g. 72) */
                , PTB_uint32_t               width                 /*!< Width in pixel - only applied if 'resolution' equal 0 */
                , PTB_uint32_t               height                /*!< Height in pixel - only applied if 'resolution' equal 0 */
                , enum PTB_EJPEGFormat       jpegFormat            /*!< JPEG format - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
                , enum PTB_ECompression      compression           /*!< Compression - only applied if 'imageFormat' is PTB_eJPEG, PTB_ePDF or PTB_eTIFF */
                , enum PTB_EColorSpace       colorspace            /*!< Color space */
                , const PTB_Path_t*          simulationProfile     /*!< Simulation profile: Path to ICC profile or Description of ICC profile or OUTPUTINTENT or NULL */
                , PTB_uint32_t               flags                 /*!< Flags (any combination of PTB_EImageFlags values) */
                , PTB_uint32_t               firstPage             /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
                , PTB_uint32_t               lastPage              /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                , const PTB_Path_t*          destFolder            /*!< Destination folder path (including folder name) */
                , PTB_SaveAsImageCB          cbSaveAsImage         /*!< Callback function to be called for every created image, might be NULL */
                , void*                      cbSaveAsImageUserData /*!< User data submitted to callback function */
                , PTB_ProgressCB             cbProgress            /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData    /*!< User data submitted to progress callback function */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsImage5( const PTB_Path_t*          src                   /*!< Document */
                , enum PTB_EImageFormat      imageFormat           /*!< JPEG, PNG, TIFF or PDF */
                , PTB_uint32_t               resolution            /*!< Resolution in ppi (e.g. 72) */
                , PTB_uint32_t               width                 /*!< Width in pixel - only applied if 'resolution' equal 0 */
                , PTB_uint32_t               height                /*!< Height in pixel - only applied if 'resolution' equal 0 */
                , enum PTB_EJPEGFormat       jpegFormat            /*!< JPEG format - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
                , enum PTB_ECompression      compression           /*!< Compression - only applied if 'imageFormat' is PTB_eJPEG, PTB_ePDF or PTB_eTIFF */
                , enum PTB_EColorSpace       colorspace            /*!< Color space */
                , const PTB_Path_t*          simulationProfile     /*!< Simulation profile: Path to ICC profile or Description of ICC profile or OUTPUTINTENT or NULL */
                , PTB_uint32_t               flags                 /*!< Flags (any combination of PTB_EImageFlags values) */
                , enum PTB_EPageBox          pagebox               /*!< Render pages using a page geometry box - ignored if imageFormat is PTB_eImgPDF */
                , const PTB_sys_char_t*      rect                  /*!< Render only part of page relative to 'pagebox'; syntax: <lower left x>,<lower left y>,<upper right x>,<upper right y>[<unit>] (Optional unit for coordinates: pt, mm, in (default: pt)), might be NULL */
                , PTB_uint32_t               firstPage             /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
                , PTB_uint32_t               lastPage              /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                , const PTB_Path_t*          destFolder            /*!< Destination folder path (including folder name) */
                , PTB_SaveAsImageCB          cbSaveAsImage         /*!< Callback function to be called for every created image, might be NULL */
                , void*                      cbSaveAsImageUserData /*!< User data submitted to callback function */
                , PTB_ProgressCB             cbProgress            /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData    /*!< User data submitted to progress callback function */
                );

/*!
** \brief  PDF Export flags
*/
enum PTB_EPDFExportFlags { PTB_ePDFExportNone                       = 0x000    /*!< None               */
                         , PTB_ePDFExportForceOpenOffice            = 0x001    /*!< Office only: Processes MS Office documents with OpenOffice */
                         , PTB_ePDFExportUseExcelPageLayout         = 0x002    /*!< Excel only: Use Excel page layout */
                         , PTB_ePDFExportOptimizeForScreen          = 0x004    /*!< Word only: Images from office documents will have lower quality, result file will be smaller */
                         , PTB_ePDFExportNoOptimization             = 0x008    /*!< Deactivates optimization of the internal structure when saving the PDF */
                         , PTB_ePDFExportNoPostprocess              = 0x010    /*!< Disables the saving process after the conversion */
 						 , PTB_ePDFExportUseUniqueFontCache         = 0x020    /*!< PS only: Use unique font cache */
                         , PTB_ePDFExportForcePostScript            = 0x040    /*!< Forces using PostScript to PDF */
                         , PTB_ePDFExportNoRemoteContent            = 0x080    /*!< HTML only: Disables remote content */
                         , PTB_ePDFExportOptimizeForPrintWithMarkup = 0x100    /*!< Word only: Print quality and markup/comments remains  */
                        };

/*!
** \brief  Saves supported non-PDF files to PDF
**         
** \note http://www.callassoftware.com/goto/tbx_en_topdf
**         
** \note Supported files and applications<br/>
** Microsoft Word:<br/>
** .doc .docx .rtf<br/>
** Microsoft Excel:<br/>
** .xls .xlsx<br/>
** Microsoft PowerPoint:<br/>
** .ppt .pptx<br/>
** Microsoft Visio:<br/>
** .vsd .vdx .vdw .vss .vsx<br/>
** Microsoft Project:<br>
** .mpp .mpt .mpd .mpw .mpx<br/>
** Microsoft Publisher:<br/>
** .pub<br/>
** OpenOffice Writer:<br/>
** .odt .ott .doc .docx .txt .rtf (...)<br/>
** OpenOffice Calc:<br/>
** .ods .ots .xls .xlsx .csv (...)<br/>
** OpenOffice Impress:<br/>
** .odp .otp .ppt .pptx .pptm (...)<br/>
** Pages:<br/>
** .pages<br/>
**         
** SDK:<br/>
** .eps .ps<br/>
** .tiff .jpeg .png .gif .psd<br/>
** .html .svg<br/>
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsPDF( const PTB_Path_t*     src                 /*!< Document */
             , PTB_uint32_t          firstPage           /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
             , PTB_uint32_t          lastPage            /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
             , PTB_bool_t            forceOpenOffice     /*!< Processes MS Office documents with OpenOffice */
             , const PTB_Path_t*     dest                /*!< Destination path (including file name) */
             , PTB_ProgressCB        cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
             , void*                 cbProgressUserData  /*!< User data submitted to progress callback function */
             );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsPDF2( const PTB_Path_t*     src                 /*!< Document */
              , PTB_uint32_t          firstPage           /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
              , PTB_uint32_t          lastPage            /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
              , PTB_bool_t            forceOpenOffice     /*!< Office only: Processes MS Office documents with OpenOffice */
              , const PTB_Path_t*     pdfsetting          /*!< EPS and PS only: Full path to the PDF setting file (*.joboption), might be NULL to use default */
              , const PTB_Path_t*     dest                /*!< Destination path (including file name) */
              , PTB_ProgressCB        cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
              , void*                 cbProgressUserData  /*!< User data submitted to progress callback function */
              );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsPDF3( const PTB_Path_t*     src                 /*!< Document */
              , PTB_uint32_t          firstPage           /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
              , PTB_uint32_t          lastPage            /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
              , PTB_bool_t            forceOpenOffice     /*!< Office only: Processes MS Office documents with OpenOffice */
              , const PTB_Path_t*     pdfsetting          /*!< EPS and PS only: Full path to the PDF setting file (*.joboption), might be NULL to use default */
			  , const PTB_sys_char_t* password            /*!< PDF only: Password to generate an unprotected PDF from a secured PDF, might be NULL */
              , const PTB_Path_t*     dest                /*!< Destination path (including file name) */
              , PTB_ProgressCB        cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
              , void*                 cbProgressUserData  /*!< User data submitted to progress callback function */
              );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsPDF4( const PTB_Path_t*     src                 /*!< Document */
			  , PTB_uint32_t          firstPage           /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
			  , PTB_uint32_t          lastPage            /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
			  , PTB_uint32_t          flags               /*!< Flags (any combination of PTB_EPDFExportFlags values) */
			  , const PTB_Path_t*     pdfsetting          /*!< EPS and PS only: Full path to the PDF setting file (*.joboption), might be NULL to use default */
			  , const PTB_sys_char_t* password            /*!< PDF only: Password to generate an unprotected PDF from a secured PDF, might be NULL */
			  , const PTB_Path_t*     dest                /*!< Destination path (including file name) */
			  , PTB_bool_t*           fontsubstitution    /*!< [OUT] true if fonts are missing and would be substituted otherwise false, might be NULL */
			  , PTB_ProgressCB        cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
			  , void*                 cbProgressUserData  /*!< User data submitted to progress callback function */
			  );

/*!
** \brief  Callback for PTB_SaveAsPDF5. Called for every missing font.
** \note: Currently supported: Microsoft Word
*/
typedef void(*PTB_ExportPDFFontCB)( PTB_StringID    idMissingFont        /*!< Missing font */
								  , PTB_StringID    idSubstitutedFont    /*!< Substituted font */
								  , void*           userData             /*!< User data */
							      );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsPDF5( const PTB_Path_t*     src                    /*!< Document */
			  , PTB_uint32_t          firstPage              /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
			  , PTB_uint32_t          lastPage               /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
			  , PTB_uint32_t          flags                  /*!< Flags (any combination of PTB_EPDFExportFlags values) */
			  , const PTB_Path_t*     pdfsetting             /*!< EPS and PS only: Full path to the PDF setting file (*.joboption), might be NULL to use default */
			  , const PTB_sys_char_t* password               /*!< PDF only: Password to generate an unprotected PDF from a secured PDF, might be NULL */
			  , const PTB_Path_t*     dest                   /*!< Destination path (including file name) */
			  , PTB_ExportPDFFontCB   cbMissingFont          /*!< Missing font CB function to be called during processing, might be NULL */
			  , void*                 cbMissingFontUserData  /*!< User data submitted to font callback function */
			  , PTB_ProgressCB        cbProgress             /*!< Progress CB function to be called during processing, might be NULL */
			  , void*                 cbProgressUserData     /*!< User data submitted to progress callback function */
			  );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsPDF6( const PTB_Path_t*     src                    /*!< Document */
			  , PTB_uint32_t          firstPage              /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
			  , PTB_uint32_t          lastPage               /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
			  , PTB_uint32_t          flags                  /*!< Flags (any combination of PTB_EPDFExportFlags values) */
			  , const PTB_Path_t*     pdfsetting             /*!< EPS and PS only: Full path to the PDF setting file (*.joboption), might be NULL to use default */
			  , const PTB_sys_char_t* password               /*!< PDF only: Password to generate an unprotected PDF from a secured PDF, might be NULL */
			  , const PTB_Path_t*     prologue               /*!< PS only: Full path to a prologue file (.ps), might be NULL */
			  , const PTB_Path_t*     epilogue               /*!< PS only: Full path to an epilogue file (.ps), might be NULL */
			  , const PTB_Path_t*     dest                   /*!< Destination path (including file name) */
			  , PTB_ExportPDFFontCB   cbMissingFont          /*!< Missing font CB function to be called during processing, might be NULL */
			  , void*                 cbMissingFontUserData  /*!< User data submitted to font callback function */
			  , PTB_ProgressCB        cbProgress             /*!< Progress CB function to be called during processing, might be NULL */
			  , void*                 cbProgressUserData     /*!< User data submitted to progress callback function */
			  );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsPDF7( const PTB_Path_t*     src                    /*!< Document */
			  , PTB_uint32_t          firstPage              /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
			  , PTB_uint32_t          lastPage               /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
			  , PTB_uint32_t          flags                  /*!< Flags (any combination of PTB_EPDFExportFlags values) */
			  , const PTB_Path_t*     pdfsetting             /*!< EPS and PS only: Full path to the PDF setting file (*.joboption), might be NULL to use default */
			  , const PTB_sys_char_t* password               /*!< PDF only: Password to generate an unprotected PDF from a secured PDF, might be NULL */
			  , const PTB_Path_t*     prologue               /*!< PS only: Full path to a prologue file (.ps), might be NULL */
			  , const PTB_Path_t*     epilogue               /*!< PS only: Full path to an epilogue file (.ps), might be NULL */
			  , const PTB_Path_t*     logfile                /*!< PS and HTML only: Full path to an log file, might be NULL */
			  , const PTB_Path_t*     dest                   /*!< Destination path (including file name) */
			  , PTB_ExportPDFFontCB   cbMissingFont          /*!< Missing font CB function to be called during processing, might be NULL */
			  , void*                 cbMissingFontUserData  /*!< User data submitted to font callback function */
			  , PTB_ProgressCB        cbProgress             /*!< Progress CB function to be called during processing, might be NULL */
			  , void*                 cbProgressUserData     /*!< User data submitted to progress callback function */
			  );

/*!
** \brief  PostScript flags
*/
enum PTB_EPostScriptFlags { PTB_ePSNone                 = 0x00    /*!< None */
                          , PTB_ePSAddPageInfo          = 0x01    /*!< Add page information */
                          , PTB_ePSAddColorBars         = 0x02    /*!< Add color bars */
                          , PTB_ePSAddRegistrationMarks = 0x04    /*!< Add registration marks */
                          , PTB_ePSAddCutMarks          = 0x08    /*!< Adds cutmarks */
                          , PTB_ePSApplyColormanagement = 0x10    /*!< Apply host based color management */
                          , PTB_ePSApplyOutputPreview   = 0x20    /*!< Apply output preview settings - only applied if 'PTB_ePSApplyColormanagement' is used */
                          , PTB_ePSSimulateOverprint    = 0x40    /*!< Simulate overprint */
                          , PTB_ePSWriteASCII           = 0x80    /*!< Writes in 'Clean7Bit' ASCII */
						  , PTB_ePSSPD                  = 0x100   /*!< SPD */
                          };

/*!
** \brief Save as PostScript
**        Converts the PDF into PostScript.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsPostScript( const PTB_Path_t*          src                        /*!< Document */
                    , enum PTB_EPostScriptLevel  postscriptLevel            /*!< PostScript level [2|3] */
                    , PTB_uint32_t               flags                      /*!< Flags (any combination of PTB_EPostScriptFlags values) */
                    , PTB_float_t                lineWeight                 /*!< Line weight of cut marks in pt */
                    , const PTB_utf8_char_t*     simulationProfile          /*!< Simulation profile, default: 'ISO Coated v2 (ECI)' only applied if 'PTB_ePSApplyColormanagement' is used */
                    , PTB_uint16_t               transparencyQuality        /*!< Transparency quality in % */
                    , PTB_uint32_t               gradientResolution         /*!< Gradient resolution in ppi */
                    , PTB_uint32_t               bitmapResolution           /*!< Bitmap resolution in ppi */
                    , PTB_uint32_t               firstPage                  /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
                    , PTB_uint32_t               lastPage                   /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                    , const PTB_Path_t*          dest                       /*!< Destination path (including file name) */
                    , PTB_ProgressCB             cbProgress                 /*!< Progress CB function to be called during processing, might be NULL */
                    , void*                      cbProgressUserData         /*!< User data submitted to progress callback function */
                    );

/*!
** \brief Save as HTML
**        Converts the tagged PDF into HTML.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsHTML( const PTB_Path_t*          src                        /*!< Document */
              , const PTB_Path_t*          styleFolder                /*!< Custom style folder, might be NULL to ignore */
              , const PTB_Path_t*          destFolder                 /*!< Destination folder path (including folder name) */
              , PTB_ProgressCB             cbProgress                 /*!< Progress CB function to be called during processing, might be NULL */
              , void*                      cbProgressUserData         /*!< User data submitted to progress callback function */
              );

/*!
** \brief Save as EPUB
**        Converts the tagged PDF into EPUB.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveAsEPUB( const PTB_Path_t*          src                        /*!< Document */
              , const PTB_Path_t*          styleFolder                /*!< Custom style folder, might be NULL to ignore */
              , const PTB_Path_t*          dest                       /*!< Destination path (including file name) */
              , PTB_ProgressCB             cbProgress                 /*!< Progress CB function to be called during processing, might be NULL */
              , void*                      cbProgressUserData         /*!< User data submitted to progress callback function */
              );


/*!
** \brief  An enum to define the PDF/A conformance level
*/
enum PTB_EEmailLevel { PTB_eEmailNoPDFA        /*!< No PDF/A      */
				 	 , PTB_eEmailPDFA1a        /*!< PDF/A-1a:2005 */
					 , PTB_eEmailPDFA1b        /*!< PDF/A-1b:2005 */
					 , PTB_eEmailPDFA2a        /*!< PDF/A-2a      */
					 , PTB_eEmailPDFA2b        /*!< PDF/A-2b      */
					 , PTB_eEmailPDFA2u        /*!< PDF/A-2u      */
					 , PTB_eEmailPDFA3a        /*!< PDF/A-3a      */
					 , PTB_eEmailPDFA3b        /*!< PDF/A-3b      */
					 , PTB_eEmailPDFA3u        /*!< PDF/A-3u      */
					 };

/*!
** \brief  An enum to define the handling of email attachments
*/
enum PTB_EEmailAttachments { PTB_eEmailDefault       = 0x00   /*!< Using defaults: Add additional pages on PDF/A-1, otherwise embed attachments as PDF */
						   , PTB_eEmailIgnore        = 0x01   /*!< Ignore attachments                  */
						   , PTB_eEmailEmbedOriginal = 0x02   /*!< Embed attachments as original       */
						   , PTB_eEmailEmbedPDF      = 0x04   /*!< Embed attachments as PDF            */
						   , PTB_eEmailAddPages      = 0x08   /*!< Add attachments as additional pages */
						   };

/*!
** \brief  An enum to define the handling of email attachments in case of error
*/
enum PTB_EEmailOnError { PTB_eEmailOnErrorFallback  /*!< Embed attachment as original and convert to PDF/A-3 */
					   , PTB_eEmailOnErrorSkip      /*!< Skip attachment                                     */
					   , PTB_eEmailOnErrorAbort     /*!< Abort the whole conversion process                  */
					   };

/*!
** \brief  Flags to control some aspects during email conversion
*/
enum PTB_EEmailFlags { PTB_eEmailNone                             = 0x00    /*!< None               */
					 , PTB_eEmailEmbedSourceEmailIntoPDF          = 0x01    /*!< Embed original e-mail file (msg, eml or emlx) into PDF */
					 , PTB_eEmailOfficeUseExcelPageLayout         = 0x02    /*!< Excel only: Use Excel page layout */
					 , PTB_eEmailOfficeOptimizeForScreen          = 0x04    /*!< Word only: Images from office documents will have lower quality, result file will be smaller */
					 , PTB_eEmailOfficeNoRemoteContent            = 0x08    /*!< HTML only: Disables remote content */
				     , PTB_eEmailOfficeOptimizeForPrintWithMarkup = 0x10    /*!< Word only: Print quality and markup/comments remains  */
					 };

/*!
** \brief  Callback for PTB_SaveEmailAsPDF. Called for every attachment that could not be converted.
**         Further handling was defined by the 'onerror' parameter (Skip, Abort or Fallback)
*/
typedef void(*PTB_EmailAttachmentCB)( PTB_StringID           idAttachment    /*!< Attachment */
									, enum PTB_EEmailOnError onerror         /*!< Used 'onerror' value from PTB_SaveEmailAsPDF */
									, void*                  userData        /*!< User data */
									);
/*!
** \brief Save email as PDF
**        Converts an e-mail (.eml, .emlx, .msg) into PDF.
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveEmailAsPDF( const PTB_Path_t*          src                        /*!< E-mail */
				  , const PTB_Path_t*          templateFolder             /*!< Template folder, might be NULL to use default (Basic)*/
				  , enum PTB_EEmailLevel       level                      /*!< PDF/A level or None */
				  , PTB_uint32_t               attachments                /*!< Attachments handling (any combination of PTB_EEmailAttachments values) */
				  , PTB_uint32_t               flags                      /*!< Flags (any combination of PTB_EEmailFlags values) */
				  , enum PTB_EEmailOnError     onerror                    /*!< Handling of attachments in case of error */
				  , const PTB_utf8_char_t*     noconvert                  /*!< RegEx based filter for files that should not be converted to PDF, e.g.: ".*.(txt|var)", might be NULL */
				  , const PTB_utf8_char_t*     noembed                    /*!< RegEx based filter for files that should not be embedded, e.g.: ".*.(vir|bin|exe)", might be NULL */
			      , const PTB_Path_t*          dest                       /*!< Destination path (including file name) */
				  , PTB_EmailAttachmentCB      cbAttachment               /*!< E-mail attachment CB function to be called for every attachment that could not be converted, might be NULL */
				  , void*                      cbAttachmentUserData       /*!< User data submitted to e-mail attachment callback function */
			      , PTB_ProgressCB             cbProgress                 /*!< Progress CB function to be called during processing, might be NULL */
			      , void*                      cbProgressUserData         /*!< User data submitted to progress callback function */
			      );


/*!
** \brief  Flags of the attachment conversion
*/
enum PTB_EEmailAttachmentFlags { PTB_eEmailAttachmentNone    = 0x00 /*!< None                                     */
							   , PTB_eEmailAttachmentPDF     = 0x01 /*!< Attachment was already a PDF             */
							   , PTB_eEmailAttachmentArchive = 0x02 /*!< Attachment was extracted from an archive */
							   , PTB_eEmailAttachmentFailed  = 0x04 /*!< Conversion to PDF failed                 */
							   };

/*!
** \brief  Callback for PTB_SaveEmailAsPDF2. Called for every attachment (before convert PDF to PDF/A) that might be converted to PDF 
**         with additional information and BEFORE finally the email will be converted to PDF.
** \return 'false' to reject/ignore the attachment otherwise 'true' - the attachment will be handled as defined by 'onerror'
*/
typedef PTB_bool_t (*PTB_EmailAttachmentCB2)( PTB_StringID  idAttachment   /*!< Full file path to the attachment (located in a temporary location!) */
											, PTB_uint32_t  flags          /*!< Flags (any combination of PTB_EEmailAttachmentFlags values) */
											, void*         userData       /*!< User data */
											);

/*!
** \brief Save email as PDF
**        Converts an e-mail (.eml, .emlx, .msg) into PDF.
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveEmailAsPDF2( const PTB_Path_t*          src                        /*!< E-mail */
				   , const PTB_Path_t*          templateFolder             /*!< Template folder, might be NULL to use default (Basic)*/
				   , enum PTB_EEmailLevel       level                      /*!< PDF/A level or None */
				   , PTB_uint32_t               attachments                /*!< Attachments handling (any combination of PTB_EEmailAttachments values) */
				   , PTB_uint32_t               flags                      /*!< Flags (any combination of PTB_EEmailFlags values) */
				   , enum PTB_EEmailOnError     onerror                    /*!< Handling of attachments in case of error */
				   , const PTB_utf8_char_t*     noconvert                  /*!< RegEx based filter for files that should not be converted to PDF, e.g.: ".*.(txt|var)", might be NULL */
				   , const PTB_utf8_char_t*     noembed                    /*!< RegEx based filter for files that should not be embedded, e.g.: ".*.(vir|bin|exe)", might be NULL */
				   , const PTB_Path_t*          dest                       /*!< Destination path (including file name) */
				   , PTB_EmailAttachmentCB2     cbAttachment               /*!< E-mail attachment CB function to be called for every attachment, might be NULL */
				   , void*                      cbAttachmentUserData       /*!< User data submitted to e-mail attachment callback function */
				   , PTB_ProgressCB             cbProgress                 /*!< Progress CB function to be called during processing, might be NULL */
				   , void*                      cbProgressUserData         /*!< User data submitted to progress callback function */
				   );


/*!
** \brief  Callback for PTB_SaveEmailAsPDF3. Called for every attachment (before convert PDF to PDF/A) that might be converted to PDF 
**         with additional information and BEFORE finally the email will be converted to PDF.
** \return a new file spec (full path) as replacement for idAttachment
** \return idAttachment - the attachment will be handled as defined by 'onerror'
** \return NULL - the attachment will be ignored/rejected
*/
typedef PTB_Path_t* (*PTB_EmailAttachmentCB3)(PTB_StringID   idAttachment   /*!< Full file path to the attachment (located in a temporary location!) */
											 , PTB_uint32_t  flags          /*!< Flags (any combination of PTB_EEmailAttachmentFlags values) */
											 , void*         userData       /*!< User data */
											 );

/*!
** \brief Save email as PDF
**        Converts an e-mail (.eml, .emlx, .msg) into PDF.
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SaveEmailAsPDF3( const PTB_Path_t*          src                        /*!< E-mail */
				   , const PTB_Path_t*          templateFolder             /*!< Template folder, might be NULL to use default (Basic)*/
				   , enum PTB_EEmailLevel       level                      /*!< PDF/A level or None */
				   , PTB_uint32_t               attachments                /*!< Attachments handling (any combination of PTB_EEmailAttachments values) */
				   , PTB_uint32_t               flags                      /*!< Flags (any combination of PTB_EEmailFlags values) */
				   , enum PTB_EEmailOnError     onerror                    /*!< Handling of attachments in case of error */
				   , const PTB_utf8_char_t*     noconvert                  /*!< RegEx based filter for files that should not be converted to PDF, e.g.: ".*.(txt|var)", might be NULL */
				   , const PTB_utf8_char_t*     noembed                    /*!< RegEx based filter for files that should not be embedded, e.g.: ".*.(vir|bin|exe)", might be NULL */
				   , const PTB_Path_t*          dest                       /*!< Destination path (including file name) */
				   , PTB_EmailAttachmentCB3     cbAttachment               /*!< E-mail attachment CB function to be called for every attachment, might be NULL */
				   , void*                      cbAttachmentUserData       /*!< User data submitted to e-mail attachment callback function */
				   , PTB_ProgressCB             cbProgress                 /*!< Progress CB function to be called during processing, might be NULL */
				   , void*                      cbProgressUserData         /*!< User data submitted to progress callback function */
				   );


/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_EXPORT_H*/

/******************************************************************************/
/* EOF */
