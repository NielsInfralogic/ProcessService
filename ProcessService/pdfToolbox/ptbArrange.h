/*!
**
** \file ptbArrange.h
**
** \brief callas pdfEngine SDK: Arrange actions
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_ARRANGE_H)
/*! \def PTB_ARRANGE_H header define */
#define PTB_ARRANGE_H

/******************************************************************************/

#include "ptbTypes.h"
#include "ptbProgress.h"

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** Arrange
*/
/*!
** \brief  Impose based on sheet template and runlist
**         Imposes the PDF based on rules defined in run list and sheet setup.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Impose( const PTB_Path_t*        src                 /*!< Document to impose */
          , const PTB_Path_t*        runListFile         /*!< Path to runlist file */
          , const PTB_Path_t*        sheetConfigFile     /*!< Path to sheetconfig file */
          , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
          , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
          , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
          );


/*!
** \brief  Create booklet for printing
**         Prepares a document for double sided printing, such that the printout 
**         can be folded and saddle-stitched. 
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Booklet( const PTB_Path_t*        src                 /*!< Document to impose to a booklet */
           , PTB_bool_t               cutmarks            /*!< adds cutmarks */
           , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
           , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
           , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
           );


/*!
** \brief  Impose by N-Up
**         Puts several pages onto a new page. You have to define how many pages 
**         should be placed next to each other horizontally and vertically as well as 
**         the distance between the placed pages. 
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_NUp( const PTB_Path_t*        src                 /*!< Document to impose to an N-Up */
       , PTB_uint16_t             vtimes              /*!< Number of rows */
       , PTB_uint16_t             htimes              /*!< Number of columns */
       , PTB_float_t              distance            /*!< Distance between placed pages in point */
       , PTB_bool_t               cutmarks            /*!< adds cutmarks */
       , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
       , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
       , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
       );


/*!
** \brief  Impose by filling up a page
**         Puts several pages onto a new sheet with a defined page size. Distributes 
**         pages across/down as space permits.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_FillPage( const PTB_Path_t*        src                 /*!< Document to impose to fill page */
            , PTB_float_t              pagewidth           /*!< Page width of the new page in point" */
            , PTB_float_t              pageheight          /*!< Page height of the new page in point" */
            , PTB_float_t              distance            /*!< Distance between placed pages in point" */
            , PTB_bool_t               cutmarks            /*!< adds cutmarks */
            , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
            , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
            , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
            );


/*!
** \brief  Combine 2 pages to one spread 
**         Imposes a document by placing two contiguous pages next to each 
**         other.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ReaderSpreads( const PTB_Path_t*        src                 /*!< Document to impose to reader spreads */
                 , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
                 , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                 , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
                 );


/*!
** \brief  Create single pages from spread
**         Splits double pages into single pages. Recognizes if a document contains 
**         single pages as well as double pages, and then only splits the double 
**         pages, leaving the single pages as they are. 
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SplitHalf( const PTB_Path_t*        src                 /*!< Document to impose to split&half */
             , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
             , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
             , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
             );


/*!
** \brief  Impose by Step & Repeat
**         Imposes a PDF by placing a page multiple times onto a newly created 
**         page. 
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_StepRepeat( const PTB_Path_t*        src                 /*!< Document to impose to step and repeat */
              , PTB_uint16_t             vtimes              /*!< Number of rows */
              , PTB_uint16_t             htimes              /*!< Number of columns */
              , PTB_float_t              distance            /*!< Distance between placed pages in point */
              , PTB_bool_t               cutmarks            /*!< adds cutmarks */
              , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
              , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
              , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
              );


/*!
** \brief  Slice in 2 files by object type
**         Extracts objects from the current document defined by a preflight check 
**         and saves two files as a result (one containing the chosen objects, one 
**         containing the remaining objects).
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Slice( const PTB_Path_t*        src                 /*!< Document to impose to slice */
         , const PTB_Path_t*        kfp                 /*!< kfp profile to be used for slicing */
         , const PTB_Path_t*        destUpper           /*!< destination path for upper slice (including file name) */
         , const PTB_Path_t*        destLower           /*!< destination path for lower slice  (including file name) */
         , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
         , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
         );


/*!
** \brief  Callback for PTB_SplitPDF. Called for every created document.
*/
typedef void(*PTB_SplitPDFCB)( PTB_StringID    idFile    /*!< Created file */
                             , PTB_int32_t     page      /*!< Page */
                             , void*           userData  /*!< User data */
                             );

/*!
** \brief  Splits multipage documents into smaller packages
**
**         Custom split scheme:
**           expression = simple_expression_list | multi_expression.
**
**           simple_expression_list = simple_expression { "," simple_expression } [ "," joker ].
**           simple_expression = number [ "-" number ].
**           joker = "$".
**
**           multi_expression = even_pages | uneven_pages | package | intervall.
**           intervall = "*" number [ start_page | page_range ].
**           package = number "*" [ start_page ].
**           start_page = "(" number ")".
**           page_range = "(" number "," number ")".
**           uneven_pages = "uneven" | "odd".
**           even_pages = "even".
**
**           digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9".
**           unsigned = digit { digit }.2
**           number = [ "+" | "-" ] unsigned.
**
**         Examples:
**           "8"      : Page 8
**           "-1"     : Last page
**           "-3--1"  : Last three pages: [n, n-1, n-2]
**           "-1--3"  : Last three pages, same as -3--1: [n-2, n-1, n]
**           "-1-3"   : All but not the first two pages: [n, n-1,...,3]
**           "1-5"    : Page 1 to 5: [1,2,3,4,5]
**           "2*"     : Separate PDF files of two consecutive pages
**           "*2(1-5)": Every second page starting with the first page and ending with the fifth page as separate PDF file
**           "*2(2)"  : [2][4][6]...
**
**
**         Token for naming of destination file:
**           <docname>  : Defines the name of the original document
**           <firstpage>: Defines the first page number
**           <lastpage> : Defines the first page number
**         Info: 'destDigits' is used for page number format
**
**         Examples (Input: TestFile.pdf with 8 pages):
**           <docname>_<firstpage>_<lastpage> = TestFile_0001_0008.pdf
**           <docname>_ABC                    = TestFile_ABC.pdf
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SplitPDF( const PTB_Path_t*        src                 /*!< Document to split */
            , const PTB_sys_char_t*    splitscheme         /*!< Custom split scheme */
            , const PTB_utf8_char_t*   destToken           /*!< Token for naming output file, might be NULL */
            , PTB_uint16_t             destDigits          /*!< Defines the number of digits used for the page number - only applied if destToken != NULL */
            , const PTB_Path_t*        destFolder          /*!< Destination folder path (including folder name) */
            , PTB_SplitPDFCB           cbSplitPDF          /*!< Callback function to be called for every created PDF, might be NULL */
            , void*                    cbSplitPDFUserData  /*!< User data submitted to callback function */
            , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
            , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
            );


/*!
** \brief  Merges PDF files
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_MergePDF( const PTB_Path_t**       srcFilesFolders      /*!< the address of an array of strings describing the file paths of the source pdfs or folder containing pdfs */
            , PTB_uint32_t             srcFilesFoldersCount /*!< indicates the size of the srcFilesFolders array */
            , const PTB_Path_t*        dest                 /*!< Destination path (including file name) */
            , PTB_ProgressCB           cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
            , void*                    cbProgressUserData   /*!< User data submitted to progress callback function */
            );


/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_ARRANGE_H*/

/******************************************************************************/
/* EOF */
