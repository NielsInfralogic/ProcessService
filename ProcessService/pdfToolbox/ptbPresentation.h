/*!
**
** \file ptbPresentation.h
**
** \brief callas pdfEngine SDK: Presentation actions
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_PRESENTATION_H)
/*! \def PTB_PRESENTATION_H header define */
#define PTB_PRESENTATION_H

/******************************************************************************/

#include "ptbTypes.h"
#include "ptbProgress.h"

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** Presentation
*/

/*!
** \brief  An enum to define the transition
*/
enum PTB_ETransition { PTB_eBlinds     /*!< Use blinds */
                     , PTB_eBox        /*!< Use box */
                     , PTB_eComb       /*!< Use comb */
                     , PTB_eCover      /*!< Use cover */
                     , PTB_eDissolve   /*!< Use dissolve */
                     , PTB_eFade       /*!< Use fade */
                     , PTB_eGlitter    /*!< Use glitter */
                     , PTB_ePush       /*!< Use push */
                     , PTB_eRandom     /*!< Use random */
                     , PTB_eReplace    /*!< Use replace */
                     , PTB_eSplit      /*!< Use split */
                     , PTB_eUncover    /*!< Use uncover */
                     , PTB_eWipe       /*!< Use wipe */
                     , PTB_eZoomIn     /*!< Use zoom in */
                     , PTB_eZoomOut    /*!< Use zoom out */
                     };

/*!
** \brief  Prepare for presentation in Acrobat
**         Prepares a PDF for use as a slide presentation.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Presentation( const PTB_Path_t*        src                 /*!< Document */
                , enum PTB_ETransition     transition          /*!< Transition to use in presentation */
                , PTB_uint32_t             selfrunning         /*!< Make self running in seconds, 0 for manually */
                , PTB_bool_t               fullscreen          /*!< Open in fullscreen modus */
                , PTB_bool_t               blackpage           /*!< Add a black slide at the end of the document */
                , PTB_bool_t               progress            /*!< Add a progress thermometer at the bottom of the pages of the document */
                , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
                , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
                );


/*!
** \brief  An enum to define the slot config
*/
enum PTB_ESlotConfig { PTB_e2Up        /*!< 2 */
                     , PTB_e3Up        /*!< 3 */
                     , PTB_e2by2       /*!< 2x2 */
                     , PTB_e2by3       /*!< 2x3 (3x2 with notes) */
                     };

/*!
** \brief  An enum to define the page format
*/
enum PTB_EPageFormat { PTB_eDinA4        /*!< DinA4 */
                     , PTB_eLetter       /*!< Letter */
                     };

/*!
** \brief  Create handout from PDF presentation
**         Creates a handout containing several slides on one page and optionally 
**         some lines for notes.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Handout( const PTB_Path_t*        src                 /*!< Document */
           , enum PTB_ESlotConfig     slotconfig          /*!< Slot config to use */
           , PTB_bool_t               notes               /*!< Space for notes */
           , PTB_bool_t               slidefirstpage      /*!< Only one slide on the first page */
           , enum PTB_EPageFormat     pageformat          /*!< Page format */ 
           , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
           , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
           , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
           );


/*!
** \brief  Create a Passe Partout
**         Adds a background border around the page content.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PassePartout( const PTB_Path_t*        src                 /*!< Document to impose to an N-Up */
                , const PTB_Path_t*        background          /*!< Path to a pdf file with the background pattern (including file name) */
                , PTB_float_t              borderwidth         /*!< Width of background border around the new PDF */
                , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
                , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
                );


/*!
** \brief  Position pages on a virtual light table
**         Puts several pages on one new page to give the impression of a light table.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LightTable( const PTB_Path_t*        src                 /*!< Document */
              , PTB_float_t              pagewidth           /*!< Page width of the new page in point */
              , PTB_float_t              pageheight          /*!< Page height of the new page in point */
              , PTB_uint32_t             columns             /*!< Number of columns */
              , const PTB_Path_t*        background          /*!< Path to a pdf file with the background pattern (including file name), might be NULL */
              , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
              , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
              , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
              );




/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_PRESENTATION_H*/

/******************************************************************************/
/* EOF */
