/*!
**
** \file ptbPrint.h
**
** \brief callas pdfEngine SDK: Print actions (DEPRECATED)
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_PRINT_H)
/*! \def PTB_PRINT_H header define */
#define PTB_PRINT_H

/******************************************************************************/
#if defined(_WIN32) || defined(_M_X64)
#	pragma message("warning: (DEPRECATED) - Please use ptbExport.h")
#else
//#	warning("(DEPRECATED) - Please use ptbExport.h")
#endif
/******************************************************************************/

#include "ptbTypes.h"
#include "ptbProgress.h"
#include "ptbExport.h"

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** Print
*/

// (DEPRECATED) - Please use ptbExport.h!!
/*!
** \brief  Callback for PTB_CreateEPS. Called for every created document.
*/
typedef void(*PTB_CreateEPSCB)( PTB_StringID    idPath    /*!< Created file */
                              , PTB_int32_t     page      /*!< Page */
                              , void*           userData  /*!< User data */
                              );

/*!
** \brief Create EPS 
**        Converts the document into EPS.
** \note (DEPRECATED) replaced by PTB_SaveAsEPS()
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_CreateEPS( const PTB_Path_t*          src                        /*!< Document */
             , enum PTB_EPostScriptLevel  postscriptLevel            /*!< PostScript level [2|3] */
             , PTB_bool_t                 pageInfo                   /*!< Add page information */
             , PTB_bool_t                 colorbars                  /*!< Add color bars */
             , PTB_bool_t                 registrationMarks          /*!< Add registration marks */
             , PTB_bool_t                 cutmarks                   /*!< Adds cutmarks */
             , PTB_float_t                lineWeight                 /*!< Line weight of cut marks in pt */
             , PTB_bool_t                 colormanagement            /*!< Apply host based color management */
             , PTB_bool_t                 applyOutputPreviewSettings /*!< Apply output preview settings - only applied if 'colormanagement' is true */
             , const PTB_utf8_char_t*     simulationProfile          /*!< Simulation profile, default: 'ISO Coated v2 (ECI)' - only applied if 'colormanagement' is true */
             , PTB_uint16_t               transparencyQuality        /*!< Transparency quality in % */
             , PTB_uint32_t               gradientResolution         /*!< Gradient resolution in ppi */
             , PTB_uint32_t               bitmapResolution           /*!< Bitmap resolution in ppi */
             , PTB_bool_t                 simulateOverprint          /*!< Simulate overprint */
             , PTB_uint32_t               firstPage                  /*!< First page, this is a zero based index, i.e. first page has value of 0 (zero) */
             , PTB_uint32_t               lastPage                   /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
             , const PTB_Path_t*          destFolder                 /*!< Destination folder path (including folder name) */
             , PTB_CreateEPSCB            cbCreateEPS                /*!< Callback function to be called for every created PDF, might be NULL */
             , void*                      cbCreateEPSUserData        /*!< User data submitted to callback function */
             , PTB_ProgressCB             cbProgress                 /*!< Progress CB function to be called during processing, might be NULL */
             , void*                      cbProgressUserData         /*!< User data submitted to progress callback function */
             );

/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_PRINT_H*/

/******************************************************************************/
/* EOF */
