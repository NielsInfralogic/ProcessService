/*!
**
** \file ptbColors.h
**
** \brief callas pdfEngine SDK: Color actions
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_COLORS_H)
/*! \def PTB_COLORS_H header define */
#define PTB_COLORS_H

/******************************************************************************/

#include "ptbTypes.h"
#include "ptbProgress.h"

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** Colors
*/

/*!
** \brief  Color Conversion
**         Prepares the PDF for the chosen printing condition and carries out the 
**         necessary color conversion.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertColors( const PTB_Path_t**      cfgFiles            /*!< the address of an array of strings describing the filepathes of the config files */
                 ,       PTB_uint32_t      cfgFilesCount	   /*!< indicates the size of the cfgFiles array */
                 , const PTB_Path_t*       srcFile			   /*!< Document */
                 , const PTB_Path_t*       dstFile			   /*!< Destination path (including file name) */
				 , PTB_ProgressCB          cbProgress		   /*!< Progress CB function to be called during processing, might be NULL */
				 , void*                   cbProgressUserData  /*!< User data submitted to progress callback function */
                 );


/*!
** \brief
*/
typedef PTB_Path_t* (*PTB_ICCSaveCB)( PTB_StringID          currentPath  /*!< current icc file                    */
                                    , void*                 userData     /*!< User data                           */
                                    );

/*!
** \brief  Extract ICC profiles
**         Saves all embedded ICC profiles from the document. Profiles of ICC based 
**         color spaces as well as ICC profiles used in Output Intents are extracted.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ExtractICCProfile( const PTB_Path_t*        src                 /*!< Document */
                     , PTB_ICCSaveCB            cbIccSave           /*!< CB function to save the icc profiles, might be NULL  */
                     , void*                    cbIccSaveUserData   /*!< User data submitted to PTB_ICCSaveCB callback function */
					 , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                     , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
                     );


/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_COLORS_H*/

/******************************************************************************/
/* EOF */
