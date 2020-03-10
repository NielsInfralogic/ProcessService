/*!
**
** \file ptbDocuments.h
**
** \brief callas pdfEngine SDK: Documents actions
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_DOCUMENTS_H)
/*! \def PTB_DOCUMENTS_H header define */
#define PTB_DOCUMENTS_H

/******************************************************************************/

#include "ptbTypes.h"
#include "ptbProgress.h"
#include "ptbPreflight.h"
#include "ptbExport.h"

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** Documents
*/

/*!
** \brief General callback function to be called for every created file
*/
typedef void (*PTB_CreatedFileCB)( PTB_StringID    idFile       /*!< File      */
                                 , void*           userData     /*!< User data */
                                 );

/*!
** \brief  An enum to define the placement
*/
enum PTB_EPlacement { PTB_eTopLeft        /*!< Use top left */
                    , PTB_eTopCenter      /*!< Use top center */
                    , PTB_eTopRight       /*!< Use top right */
                    , PTB_eTopCenterLeft  /*!< Use top center left */
                    , PTB_eCenter         /*!< Use center */
                    , PTB_eCenterRight    /*!< Use center right */
                    , PTB_eBottomLeft     /*!< Use bottom left */
                    , PTB_eBottomCenter   /*!< Use bottom center */
                    , PTB_eBottomRight    /*!< Use bottom right */
                    };
/*!
** \brief Overlay
**        Places the chosen content on top of the input document.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Overlay( const PTB_Path_t*        src                 /*!< Document */
           , const PTB_Path_t*        overlayFile         /*!< Full path to PDF to put on top of the input PDF */
           , enum PTB_EPlacement      placement           /*!< Placement of the pages */
           , PTB_float_t              hoffset             /*!< Horizontal offset from placement in point */
           , PTB_float_t              voffset             /*!< Vertical offset from placement in point */
           , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
           , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
           , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
           );

/*!
** \brief Underlay
**        Places the chosen content underneath of the input document.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Underlay( const PTB_Path_t*        src                 /*!< Document */
            , const PTB_Path_t*        underlayFile        /*!< Full path to PDF to places the chosen content underneath of the input PDF */
		    , enum PTB_EPlacement      placement           /*!< Placement of the pages */
            , PTB_float_t              hoffset             /*!< Horizontal offset from placement in point */
            , PTB_float_t              voffset             /*!< Vertical offset from placement in point */
            , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
            , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
            , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
            );

/*!
** \brief  Extract XMP Metadata
**         Extract XMP Metadata of the processed PDF into a configurable XML file.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ExtractXMPMetadata( const PTB_Path_t*        src                 /*!< Document */
                      , const PTB_Path_t*        cfg                 /*!< Path to configuration file(including file name) */
                      , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
                      , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                      , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
                      );

/*!
** \brief  An enum to define the visualizer format
*/
enum PTB_EVisualizerFormat { PTB_ePDF        /*!< create visualizer PDF report */
                           , PTB_eImages     /*!< create individual visualizer images for every report part */
                           };

/*!
** \brief  An enum to define the threshold for small object coverage highlighting
*/
enum PTB_ESmallObjectsThreshold { PTB_eLowRes        /*!< low resolution output */
                                , PTB_eMediumRes     /*!< medium resolution output */
                                , PTB_eHighRes       /*!< high resolution output */
                                };

/*!
** \brief  A bitmask to select which parts of a visualizer report part should be written
*/
enum PTB_EVisualizerParts { PTB_eFull           = 0x01    /*! regular page view */

                          , PTB_eInk_temp       = 0x02    /*!< Images only: ink coverage above threshold */
                          , PTB_eInk_topo       = 0x04    /*!< Images only: ink coverage topographic view */
                          , PTB_eInk            = PTB_eInk_temp | PTB_eInk_topo

                          , PTB_eProcess_CMYK   = 0x08    /*!< Images only: CMYK channels (without spots) */
                          , PTB_eProcess_CMY    = 0x10    /*!< Images only: CMY */
                          , PTB_eProcess_K      = 0x20    /*!< Images only: K channel only */
                          , PTB_eProcess        = PTB_eProcess_CMYK | PTB_eProcess_CMY | PTB_eProcess_K

                          , PTB_eSpot_Spots     = 0x40    /*!< Images only: spot color channels  */
                          , PTB_eSpot_K         = 0x80    /*!< Images only: spot color + K channels */
                          , PTB_eSpot_CMYK      = 0x100   /*!< Images only: CMYK channels (without spots) */
                          , PTB_eSpot           = PTB_eSpot_Spots | PTB_eSpot_K | PTB_eSpot_CMYK
                          
                          , PTB_eSeps_process   = 0x200   /*!< Images only: all individual process separations */
                          , PTB_eSeps_spot      = 0x400   /*!< Images only: all individual spot color separations */
                          , PTB_eSeps           = PTB_eSeps_process | PTB_eSeps_spot

                          , PTB_eImgres_img     = 0x800   /*!< Images only: image resolution below threshold */
                          , PTB_eImgres_bmp     = 0x1000  /*!< Images only: bitmap resolution below threshold */
                          , PTB_eImgres         = PTB_eImgres_img | PTB_eImgres_bmp

                          , PTB_eSmallobj_text  = 0x2000  /*!< Images only: very small text objects below threshold */
                          , PTB_eSmallobj_lines = 0x4000  /*!< Images only: very small vector objects below threshold */
                          , PTB_eSmallobj       = PTB_eSmallobj_text | PTB_eSmallobj_lines

                          , PTB_eSafety_bleed   = 0x8000  /*!< Images only: bleed area safety zone */
                          , PTB_eSafety_trim    = 0x10000 /*!< Images only: page border safety zone */
                          , PTB_eSafety_full    = 0x20000 /*!< Images only: page border regular page view */
                          , PTB_eSafety         = PTB_eSafety_bleed | PTB_eSafety_trim | PTB_eSafety_full
                          
                          , PTB_eAll            = PTB_eFull | PTB_eInk | PTB_eProcess | PTB_eSpot | PTB_eSeps | PTB_eImgres | PTB_eSmallobj | PTB_eSafety
                          };

/*!
** \brief  Create a visualizer report
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Visualizer( const PTB_Path_t*                src                 /*!< Document */
              , enum PTB_EVisualizerFormat       format              /*!< PDF or Images */
              , enum PTB_ESmallObjectsThreshold  smlobj              /*!< threshold for small object coverage highlighting */
              , PTB_uint32_t                     inkcov              /*!< threshold for ink coverage highlighting in % */
              , PTB_uint32_t                     bmpres              /*!< threshold for bitmap resolution highlighting in ppi */
              , PTB_uint32_t                     imgres              /*!< threshold for image resolution highlighting in ppi */
              , PTB_uint32_t                     parts               /*!< which parts (any combination of PTB_EVisualizerParts values) */
              , PTB_uint32_t                     resolution          /*!< Resolution in ppi (e.g. 72) - only applied if 'format' is PTB_eImage */
              , PTB_uint32_t                     width               /*!< width in pixel - only applied if 'format' is PTB_eImage and 'resolution' equal 0 */
              , PTB_uint32_t                     height              /*!< height in pixel - only applied if 'format' is PTB_eImage and 'resolution' equal 0 */
              , PTB_uint32_t                     firstPage           /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
              , PTB_uint32_t                     lastPage            /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
              , const PTB_Path_t*                destFolder          /*!< Destination folder path (including folder name) */
              , PTB_CreatedFileCB                cbVisualizer        /*!< Callback function to be called for every created file, might be NULL */
              , void*                            cbVisualizerData    /*!< User data submitted to callback function */
              , PTB_ProgressCB                   cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
              , void*                            cbProgressUserData  /*!< User data submitted to progress callback function */
              );

/*!
** \brief  Flags to control some aspects for the Visualizer
*/
enum PTB_EVisualizerFlags { PTB_evfNone                 = 0x00  /*!< None */
					      , PTB_evfSeparationColorized  = 0x01  /*!< Render all individual separations in their respective color */
					      , PTB_evfProblemsOnly         = 0x02  /*!< Only emit pages that have problems */
					      };

/*!
** \brief  Create a visualizer report
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_Visualizer2( const PTB_Path_t*                src                 /*!< Document */
               , enum PTB_EVisualizerFormat       format              /*!< PDF or Images */
               , enum PTB_ESmallObjectsThreshold  smlobj              /*!< Threshold for small object coverage highlighting */
               , PTB_uint32_t                     inkcov              /*!< Threshold for ink coverage highlighting in % */
               , PTB_uint32_t                     bmpres              /*!< Threshold for bitmap resolution highlighting in ppi */
               , PTB_uint32_t                     imgres              /*!< Threshold for image resolution highlighting in ppi */
               , PTB_uint32_t                     parts               /*!< which parts (any combination of PTB_EVisualizerParts values) */
               , PTB_uint32_t                     flags               /*!< Flags (any combination of PTB_EVisualizerFlags values) */
               , PTB_uint32_t                     resolution          /*!< Resolution in ppi (e.g. 72) - only applied if 'format' is PTB_eImage */
               , PTB_uint32_t                     width               /*!< width in pixel - only applied if 'format' is PTB_eImage and 'resolution' equal 0 */
               , PTB_uint32_t                     height              /*!< height in pixel - only applied if 'format' is PTB_eImage and 'resolution' equal 0 */
               , PTB_uint32_t                     firstPage           /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
               , PTB_uint32_t                     lastPage            /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
               , const PTB_Path_t*                destFolder          /*!< Destination folder path (including folder name) */
               , PTB_CreatedFileCB                cbVisualizer        /*!< Callback function to be called for every created file, might be NULL */
               , void*                            cbVisualizerData    /*!< User data submitted to callback function */
               , PTB_ProgressCB                   cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
               , void*                            cbProgressUserData  /*!< User data submitted to progress callback function */
               );

/*!
** \brief  Create a visualizer images
**         Basically PTB_Visualizer with format PTB_eImages and some more settings for the images
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_VisualizerAsImages( const PTB_Path_t*                src                 /*!< Document */
                      , enum PTB_ESmallObjectsThreshold  smlobj              /*!< Threshold for small object coverage highlighting */
                      , PTB_uint32_t                     inkcov              /*!< Threshold for ink coverage highlighting in % */
                      , PTB_uint32_t                     bmpres              /*!< Threshold for bitmap resolution highlighting in ppi */
                      , PTB_uint32_t                     imgres              /*!< Threshold for image resolution highlighting in ppi */
                      , PTB_uint32_t                     parts               /*!< Parts (any combination of PTB_EVisualizerParts values) */
                      , const PTB_utf8_char_t**          sepNames            /*!< the address of an array of strings describing the name of separations
                                                                                  use 0 as parts to write only this separation names (similar to PTB_eSeps_spot)
                                                                             */
                      , PTB_uint32_t                     sepNamesCount       /*!< indicates the size of the sepNames array */
                      , PTB_uint32_t                     flags               /*!< Flags (any combination of PTB_EVisualizerFlags values) */
                      , enum PTB_EImageFormat            imageFormat         /*!< JPEG, PNG, TIFF or PDF */
                      , PTB_uint32_t                     resolution          /*!< Resolution in ppi (e.g. 72) */
                      , PTB_uint32_t                     width               /*!< width in pixel - only applied if 'resolution' equal 0 */
                      , PTB_uint32_t                     height              /*!< height in pixel - only applied if 'resolution' equal 0 */
                      , enum PTB_EJPEGFormat             jpegFormat          /*!< JPEG format - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
                      , enum PTB_ECompression            compression         /*!< Compression - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
                      , enum PTB_EColorSpace             colorspace          /*!< PTB_eRGB can be used only - other color spaces not yet implemented! */
                      , PTB_uint32_t                     firstPage           /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
                      , PTB_uint32_t                     lastPage            /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                      , const PTB_Path_t*                destFolder          /*!< Destination folder path (including folder name) */
                      , PTB_CreatedFileCB                cbVisualizer        /*!< Callback function to be called for every created file, might be NULL */
                      , void*                            cbVisualizerData    /*!< User data submitted to callback function */
                      , PTB_ProgressCB                   cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                      , void*                            cbProgressUserData  /*!< User data submitted to progress callback function */
                      );

/*!
** \brief  Extract text from PDF
**         Extract text from PDF into a text file
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ExtractText( const PTB_Path_t*     src                 /*!< Document */
               , const PTB_Path_t*     dest                /*!< Destination path (including file name) */
               , PTB_ProgressCB        cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
               , void*                 cbProgressUserData  /*!< User data submitted to progress callback function */
               );


/*!
** \brief  A bitmask to select which parts should be included in the XML
*/
enum PTB_EContentParts { PTB_eContentNone      = 0x00    /*! None */
                       , PTB_eContentWords     = 0x01    /*! Include words */
                       , PTB_eContentWordBBox  = 0x02    /*! Include bounding box information for words */
                       , PTB_eContentWordQuads = 0x04    /*! Include quad point information for word parts */
                       , PTB_eContentChars     = 0x08    /*! Include quad point information for individual characters */
                       , PTB_eContentAnnots    = 0x10    /*! Include link annotations */
                       , PTB_eContentDocInfo   = 0x20    /*! Include document info */
                       , PTB_eContentDocXMP    = 0x40    /*! Include document XMP metadata */

                       , PTB_eContentAll       = PTB_eContentWords | PTB_eContentWordBBox | PTB_eContentWordQuads | PTB_eContentChars | PTB_eContentAnnots | PTB_eContentDocInfo | PTB_eContentDocXMP
                       };

/*!
** \brief  Extract content from PDF
**         Extract content from PDF into a XML file
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ExtractContent( const PTB_Path_t*      src                 /*!< Document */
                  , enum PTB_EContentParts parts               /*!< which parts (any combination of PTB_EContentParts values) */
                  , const PTB_Path_t*      dest                /*!< Destination path (including file name) */
                  , PTB_ProgressCB         cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                  , void*                  cbProgressUserData  /*!< User data submitted to progress callback function */
                  );

/*!
** \brief  Re-Distill
**         Recreates the PDF via PostScript, prepares for use with older equipment (RIPs)
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Redistill( const PTB_Path_t*     src                 /*!< Document */
             , const PTB_Path_t*     pdfsetting          /*!< Full path to the PDF setting file (*.joboption), might be NULL to use default */
             , const PTB_Path_t*     dest                /*!< Destination path (including file name) */
             , PTB_ProgressCB        cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
             , void*                 cbProgressUserData  /*!< User data submitted to progress callback function */
             );

/*!
** \brief Save callback for the function 'PTB_Compare'; to ignore the report you must return NULL (only available on differences >= 0).
*/
typedef PTB_Path_t* (*PTB_CompareCB)( double        differences  /*!< Differences in percent, might be -1 if not determined */
                                    , void*         userData     /*!< User data              */
                                    );

/*!
** \brief Difference highlighting sensitivity
*/
enum PTB_ESensitivity { PTB_eSensitivityMaximum  /*! Maximum sensitivity (threshold=0) */
                      , PTB_eSensitivityMedium   /*! Medium sensitivity (threshold=100) */
                      , PTB_eSensitivityMinimum  /*! Minimum sensitivity (threshold=220) */
                       };
/*!
** \brief  Compare two documents and creates a differences report
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Compare( const PTB_Path_t*     src1                /*!< Document */
           , const PTB_Path_t*     src2                /*!< Document to compare with */
           , enum PTB_ESensitivity sensitivity         /*!< Highlighting sensitivity */
           , PTB_CompareCB         cbCompare           /*!< CB function to save the PDF report file, might be NULL */
           , void*                 cbCompareUserData   /*!< User data submitted to PTB_CompareCB callback function */
           , PTB_ProgressCB        cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
           , void*                 cbProgressUserData  /*!< User data submitted to progress callback function */
           );

/*!
** \brief Channels to be compared
*/
enum PTB_ECompareChannels { PTB_eChannelsRGB          /*! RGB channels (including spots) */
						  , PTB_eChannelsCMYK         /*! CMYK channels (including spots) */
						  , PTB_eChannelsCMY          /*! CMY channels (including spots) */
						  , PTB_eChannelsProcessCMYK  /*! CMYK channels (without spots) */
						  , PTB_eChannelsProcessCMY   /*! CMY channels (without spots) */
						  , PTB_eChannelsProcessC     /*! C channel only */
						  , PTB_eChannelsProcessM     /*! M channel only */
						  , PTB_eChannelsProcessY     /*! Y channel only */
						  , PTB_eChannelsProcessK     /*! K channel only */
						  };

/*!
** \brief  Flags to control some aspects during comparing files
*/
enum PTB_ECompareFlags { PTB_ecfCompareNone                 = 0x00  /*!< None */
					   , PTB_ecfCompareOnlyDifferences      = 0x01  /*!< Only emit pages that have differences */
					   };

/*!
** \brief  Compare two documents and creates a differences report
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_CompareAsReport( const PTB_Path_t*         src1                /*!< Document */
				   , const PTB_Path_t*         src2                /*!< Document to compare with */
				   , enum PTB_ECompareChannels channels            /*!< Channels */
				   , PTB_float_t               threshold           /*!< Pixel threshold in % (e.g. 0)*/
				   , PTB_float_t               areathreshold       /*!< Area threshold in % (e.g. 0) */
				   , PTB_uint32_t              diffresolution      /*!< Resolution in ppi used for comparison (e.g. 72) */
				   , PTB_uint32_t              flags               /*!< Flags (any combination of PTB_ECompareFlags values) */
				   , const PTB_Path_t*         templateFolder      /*!< Full file path to the html template, might be NULL to use default */
				   , PTB_CompareCB             cbCompare           /*!< CB function to save the PDF report file, might be NULL */
				   , void*                     cbCompareUserData   /*!< User data submitted to PTB_CompareCB callback function */
				   , PTB_ProgressCB            cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
				   , void*                     cbProgressUserData  /*!< User data submitted to progress callback function */
				   );

PTB_FUNC_PROTO
enum PTB_EError
PTB_CompareAsReport2( const PTB_Path_t*         src1                /*!< Document */
				    , const PTB_Path_t*         src2                /*!< Document to compare with */
				    , enum PTB_ECompareChannels channels            /*!< Channels */
				    , PTB_float_t               threshold           /*!< Pixel threshold in % (e.g. 0)*/
				    , PTB_float_t               areathreshold       /*!< Area threshold in % (e.g. 0) */
				    , PTB_uint32_t              diffresolution      /*!< Resolution in ppi used for comparison (e.g. 72) */
					, enum PTB_EPlacement       anchor              /*!< Page box anchor point used to align the two page images for comparison */
					, enum PTB_EPageBox         anchorBox           /*!< Page box used to align the two page images for comparison */
					, enum PTB_EPageBox         pageBox             /*!< Render pages using a page geometry box */
					, PTB_float_t               xoffset             /*!< Move second document to the right in point */
					, PTB_float_t               yoffset             /*!< Move second document upwards in point */
				    , PTB_uint32_t              flags               /*!< Flags (any combination of PTB_ECompareFlags values) */
				    , const PTB_Path_t*         templateFolder      /*!< Full file path to the html template, might be NULL to use default */
				    , PTB_CompareCB             cbCompare           /*!< CB function to save the PDF report file, might be NULL */
				    , void*                     cbCompareUserData   /*!< User data submitted to PTB_CompareCB callback function */
				    , PTB_ProgressCB            cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
				    , void*                     cbProgressUserData  /*!< User data submitted to progress callback function */
				    );


/*!
** \brief  Compare two documents and creates a differences as individual images
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_CompareAsImage( const PTB_Path_t*         src1                  /*!< Document */
				  , const PTB_Path_t*         src2                  /*!< Document to compare with */
				  , enum PTB_ECompareChannels channels              /*!< Channels */
				  , PTB_float_t               threshold             /*!< Pixel threshold in % (e.g. 0)*/
				  , PTB_float_t               areathreshold         /*!< Area threshold in % (e.g. 0) */
				  , PTB_uint32_t              diffresolution        /*!< Resolution in ppi used for comparison */
				  , PTB_uint32_t              compareFlags          /*!< Flags (any combination of PTB_ECompareFlags values) */
				  , enum PTB_EImageFormat     imageFormat           /*!< JPEG, PNG, TIFF or PDF */
				  , PTB_uint32_t              resolution            /*!< Not implmented yet - no effect, only diffresolution has an effect */
				  , enum PTB_EJPEGFormat      jpegFormat            /*!< JPEG format - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
				  , enum PTB_ECompression     compression           /*!< Compression - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
				  , enum PTB_EColorSpace      colorspace            /*!< Color space */
				  , const PTB_Path_t*         simulationProfile     /*!< Path to Simulation profile: Path to ICC profile or Description of ICC profile or OUTPUTINTENT or NULL */
				  , PTB_uint32_t              imageFlags            /*!< Flags (any combination of PTB_EImageFlags values) */
				  , const PTB_Path_t*         destFolder            /*!< Destination folder path (including folder name) */
				  , PTB_SaveAsImageCB         cbSaveAsImage         /*!< Callback function to be called for every created image, might be NULL */
				  , void*                     cbSaveAsImageUserData /*!< User data submitted to callback function */
				  , PTB_ProgressCB            cbProgress            /*!< Progress CB function to be called during processing, might be NULL */
				  , void*                     cbProgressUserData    /*!< User data submitted to progress callback function */
				  );

PTB_FUNC_PROTO
enum PTB_EError
PTB_CompareAsImage2( const PTB_Path_t*         src1                  /*!< Document */
				   , const PTB_Path_t*         src2                  /*!< Document to compare with */
				   , enum PTB_ECompareChannels channels              /*!< Channels */
				   , PTB_float_t               threshold             /*!< Pixel threshold in % (e.g. 0)*/
				   , PTB_float_t               areathreshold         /*!< Area threshold in % (e.g. 0) */
				   , PTB_uint32_t              diffresolution        /*!< Resolution in ppi used for comparison */
				   , enum PTB_EPlacement       anchor                /*!< Page box anchor point used to align the two page images for comparison */
				   , enum PTB_EPageBox         anchorBox             /*!< Page box used to align the two page images for comparison */
				   , enum PTB_EPageBox         pageBox               /*!< Render pages using a page geometry box */
				   , PTB_float_t               xoffset               /*!< Move second document to the right in point */
				   , PTB_float_t               yoffset               /*!< Move second document upwards in point */
				   , PTB_uint32_t              compareFlags          /*!< Flags (any combination of PTB_ECompareFlags values) */
				   , enum PTB_EImageFormat     imageFormat           /*!< JPEG, PNG, TIFF or PDF */
				   , PTB_uint32_t              resolution            /*!< Not implmented yet - no effect, only diffresolution has an effect */
				   , enum PTB_EJPEGFormat      jpegFormat            /*!< JPEG format - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
				   , enum PTB_ECompression     compression           /*!< Compression - only applied if 'imageFormat' is PTB_eJPEG or PTB_ePDF */
				   , enum PTB_EColorSpace      colorspace            /*!< Color space */
				   , const PTB_Path_t*         simulationProfile     /*!< Path to Simulation profile: Path to ICC profile or Description of ICC profile or OUTPUTINTENT or NULL */
				   , PTB_uint32_t              imageFlags            /*!< Flags (any combination of PTB_EImageFlags values) */
				   , const PTB_Path_t*         destFolder            /*!< Destination folder path (including folder name) */
				   , PTB_SaveAsImageCB         cbSaveAsImage         /*!< Callback function to be called for every created image, might be NULL */
				   , void*                     cbSaveAsImageUserData /*!< User data submitted to callback function */
				   , PTB_ProgressCB            cbProgress            /*!< Progress CB function to be called during processing, might be NULL */
				   , void*                     cbProgressUserData    /*!< User data submitted to progress callback function */
				   );

/*!
** \brief  Flags to control some aspects during embedding files
*/
enum PTB_ECollectionFlags { PTB_ecfNone                  = 0x00  /*!< None */
                          , PTB_ecfNoPDFAConversion      = 0x01  /*!< Deactivates PDF/A conversion, just creates a collection (will ignore 'version' parameter) */
                          , PTB_ecfNoPDFConversion       = 0x02  /*!< Deactivates non-PDF (Office, Image PostScript) to PDF conversion - only allowed in PDF/A-3 */
                          , PTB_ecfEmbedNonPDFasSource   = 0x04  /*!< Embed non PDF as source document - only allowed in PDF/A-3 */
                          };

/*!
** \brief  Creates a collection from files
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_CreateCollection( const PTB_Path_t**       srcFiles             /*!< the address of an array of strings describing the file paths of the source documents to embed */
                    , PTB_uint32_t             srcFilesCount        /*!< indicates the size of the srcFiles array */
                    , const PTB_Path_t*        srcIndex             /*!< Document where the files should be embedded
																	      OR if NULL a index PDF (classic layout) will be created on the fly 
					                                                      OR a folder with a HTML template to create a index PDF based on HTML
																	*/
                    , enum PTB_EPDFAVersion    version              /*!< PDF/A version */
                    , const PTB_utf8_char_t*   relationship         /*!< Standard values for the AFRelationship entry as defined in PDF/A-3 are: Source, Data, Alternative, Supplement or Unspecified, if NULL Unspecified is used */
                    , PTB_uint32_t             flags                /*!< Flags (any combination of PTB_ECollectionFlags values) */
                    , const PTB_Path_t*        dest                 /*!< Destination path (including file name) */
                    , PTB_ProgressCB           cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                    , void*                    cbProgressUserData   /*!< User data submitted to progress callback function */
                   );


/*!
** \brief Extract embedded files callback for the function 'PTB_ExtractEmbeddedFiles'; to write the file return 'true' otherwise 'false'
*/
typedef PTB_bool_t (*PTB_EmbeddedFilesCB)( PTB_StringID    idFile       /*!< File      */
                                         , void*           userData     /*!< User data */
                                         );

/*!
** \brief  Extract embedded files from the PDF 
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ExtractEmbeddedFiles( const PTB_Path_t*      src                 /*!< PDF Document */
                        , const PTB_utf8_char_t* filter              /*!< RegEx based file name filter, e.g. =.*.doc" */
                        , PTB_bool_t             plain               /*!< Files are extracted directly into the destination folder */
                        , const PTB_Path_t*      destFolder          /*!< Destination folder path (including folder name) */
						, PTB_EmbeddedFilesCB    cbEmbeddedFiles     /*!< Callback function to be called for every file, might be NULL */
                        , void*                  cbEmbeddedFilesData /*!< User data submitted to callback function */
                        , PTB_ProgressCB         cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                        , void*                  cbProgressUserData  /*!< User data submitted to progress callback function */
                        );

/*!
** \brief Create ZUGFeRD invoices
**        Embeds a specified ZUGFeRD XML invoice and required XMP metadata.
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_CreateZUGFeRD( const PTB_Path_t*        pdf                 /*!< PDF Document to convert to ZUGFeRD */
                 , const PTB_Path_t*        xml                 /*!< ZUGFeRD XML invoice to embed into */
                 , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
                 , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                 , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
                 );

/*!
** \brief Create ZUGFeRD invoices with embedded Girocode barcode based on the ZUGFeRD XML invoice
**        Embeds a specified ZUGFeRD XML invoice, Girocode barcode and required XMP metadata.
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_CreateZUGFeRDwithGiroCode( const PTB_Path_t*        pdf                 /*!< PDF Document to convert to ZUGFeRD */
                             , const PTB_Path_t*        xml                 /*!< ZUGFeRD XML invoice to embed into */
							 , PTB_float_t              posx                /*!< Position (lower left x) of the resulting GiroCode in point */
							 , PTB_float_t              posy                /*!< Position (lower left y) of the resulting GiroCode in point */
							 , PTB_float_t              size                /*!< Size of the resulting GiroCode (always quadratic) */
							 , PTB_int32_t              page                /*!< Page, where the GiroCode will be positioned */
							 , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
							 , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
							 , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
							 );
/*!
** \brief  A bitmask to select which parts of a ZUGFeRD export part should be written
*/
enum PTB_EExportZUGFeRDParts { PTB_ezpImages           = 0x01    /*! Images from the PDF pages */
                             , PTB_ezpXML              = 0x02    /*! XML invoice */
                             , PTB_ezpHTML             = 0x04    /*! HTML view of the XML */
                             , PTB_ezpHTMLSynopsis     = 0x08    /*! HTML synopsis of PDF images and XML */
                             , PTB_ezpValidationReport = 0x10    /*! ZUGFeRD validation report */

                             , PTB_ezpAll              = PTB_ezpImages | PTB_ezpXML | PTB_ezpHTML | PTB_ezpHTMLSynopsis | PTB_ezpValidationReport
                             };

/*!
** \brief Export ZUGFeRD invoices
**        Creates several reports from a ZUGFeRD invoice.
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ExportZUGFeRD( const PTB_Path_t*          pdf                 /*!< Document */
                 , PTB_uint32_t               parts               /*!< Which parts (any combination of PTB_ExportZUGFeRDParts values) */
                 , const PTB_Path_t*          destFolder          /*!< Destination folder path (including folder name) */
                 , PTB_CreatedFileCB          cbExportZUGFeRD     /*!< Callback function to be called for every created file, might be NULL */
                 , void*                      cbExportZUGFeRDData /*!< User data submitted to callback function */
                 , PTB_ProgressCB             cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                 , void*                      cbProgressUserData  /*!< User data submitted to progress callback function */
                 );


/*!
** \brief  Callback for PTB_QuickInfoCB. Called for every info of the document (key-value pairs)
*/
typedef void(*PTB_QuickInfoCB)( PTB_StringID    idKey     /*!< Key, which info (e.g author, page number) */
							  , PTB_StringID    idValue   /*!< Value that corresponding with the key */
							  , void*           userData  /*!< User data */
							  );

/*!
** \brief  Get some (basic) information about the PDF
**
*/PTB_FUNC_PROTO 
enum PTB_EError 
PTB_QuickInfo( const PTB_Path_t*          pdf                 /*!< Document */
			 , PTB_QuickInfoCB            cbQuickInfo         /*!< Callback function to be called for every info, might be NULL */
			 , void*                      cbQuickInfoUserData /*!< User data submitted to callback function */
			 , PTB_ProgressCB             cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
			 , void*                      cbProgressUserData  /*!< User data submitted to progress callback function */
			 );

/*!
** \brief  Merge XMP Metadata
**         Merge XMP metadata into existing XMP metadata if present.
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_MergeXMPMetadata( const PTB_Path_t*        src                 /*!< Document */
                    , const PTB_Path_t*        xmp                 /*!< Full path to a XMP file, the XMP metadata is merged into existing XMP metadata if present */
                    , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
                    , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                    , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
                    );

/*!
** \brief Embed CxF files
**        Embeds CxF files into the Output Intent of a PDF and adds corresponding XMP metadata
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_EmbedCxF( const PTB_Path_t*        pdf                 /*!< Document */
            , const PTB_Path_t**       srcFiles            /*!< the address of an array of strings describing the file paths of the source files to embed */
            , PTB_uint32_t             srcFilesCount       /*!< indicates the size of the srcFiles array */
            , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
            , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
            , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
            );

/*!
** \brief Extract CxF files
**        Extracts all CxF data from a PDF into XML files
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ExtractCxF( const PTB_Path_t*       pdf                  /*!< Document */
              , const PTB_Path_t*       destFolder           /*!< Destination folder path (including folder name) */
              , PTB_CreatedFileCB       cbExtractedFiles     /*!< Callback function to be called for every file, might be NULL */
              , void*                   cbExtractedFilesData /*!< User data submitted to callback function */
              , PTB_ProgressCB          cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
              , void*                   cbProgressUserData   /*!< User data submitted to progress callback function */
              );

/*!
** \brief Tiling Horizontal construction type
*/
enum PTB_EContructionHorizontal { PTB_eLeftToRight  /*! Left to right */
                                , PTB_eRightToLeft  /*! Right to left */
                                };
/*!
** \brief Tiling vertical construction type
*/
enum PTB_EContructionVertical { PTB_eTopToBottom  /*! Top to bottom */
                              , PTB_eBottomToTop  /*! Bottom to top */
                              };
/*!
** \brief Flags to control some aspects during tiling
*/
enum PTB_ETilingFlags { PTB_etfNone                    = 0x00 /*! None */
                      , PTB_etfConstructionInformation = 0x01 /*! Add construction information */
                      };

/*!
** \brief Tiling by number
**        Creates tiles from the input PDF by number of tiles
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_TilingByNumber( const PTB_Path_t*                pdf                 /*!< Document */
                  , PTB_uint32_t                     hNumber             /*!< Defines the number of tiles horizontally */
                  , PTB_uint32_t                     vNumber             /*!< Defines the number of tiles vertically */
                  , enum PTB_EContructionHorizontal  hConstruction       /*!< Horizontal type of construction */
                  , enum PTB_EContructionVertical    vConstruction       /*!< Vertical type of construction */
                  , PTB_float_t                      hOverlap            /*!< Horizontal overlap of the tiles in pt */
                  , PTB_float_t                      vOverlap            /*!< Vertical overlap of the tiles in pt */
                  , enum PTB_ETilingFlags            flags               /*!< Flags (any combination of PTB_ETilingFlags values) */
		          , const PTB_Path_t*                dest                /*!< Destination path (including file name) */
                  , PTB_ProgressCB                   cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                  , void*                            cbProgressUserData  /*!< User data submitted to progress callback function */
                  );

/*!
** \brief Tiling by size
**        Creates tiles from the input PDF by dimension of created tiles
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_TilingBySize( const PTB_Path_t*                pdf                 /*!< Document */
                , PTB_float_t                      hSize               /*!< Defines the horizontal size of the tiles in pt */
                , PTB_float_t                      vSize               /*!< Defines the vertical size of the tiles in pt */
                , enum PTB_EContructionHorizontal  hConstruction       /*!< Horizontal type of construction */
                , enum PTB_EContructionVertical    vConstruction       /*!< Vertical type of construction */
                , PTB_float_t                      hOverlap            /*!< Horizontal overlap of the tiles in pt */
                , PTB_float_t                      vOverlap            /*!< Vertical overlap of the tiles in pt */
                , enum PTB_ETilingFlags            flags               /*!< Flags (any combination of PTB_ETilingFlags values) */
		        , const PTB_Path_t*                dest                /*!< Destination path (including file name) */
                , PTB_ProgressCB                   cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                , void*                            cbProgressUserData  /*!< User data submitted to progress callback function */
                );

/*!
** \brief  Performs a QuickCheck
**         Performs a QuickCheck filtered by JSON configuration and creates a JSON results
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_QuickCheck( const PTB_Path_t*        src                 /*!< Document */
              , const PTB_Path_t*        cfg                 /*!< Full path to a JSON file with configuration */
              , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
              , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
              , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
              );

/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_DOCUMENTS_H*/

/******************************************************************************/
/* EOF */
