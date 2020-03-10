/*!
**
** \file ptbOutInt.h
**
** \brief callas pdfEngine SDK: Output Intent
** 
** The callas pdfEngine SDK maintains a repository of Output Intents that can 
** be used. This file contains some functions which can be used to manipulate 
** this Output Intent repository. 
** 
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_OUTINT_H)
/*! \def PTB_OUTINT_H header define */
#define PTB_OUTINT_H

/******************************************************************************/

#include "ptbTypes.h"

/******************************************************************************/

#include "ptbProlog.h"

/******************************************************************************/

/*!
** Output Intent callback type
** \sa PTB_OIEnum()
*/
typedef void(*PTB_OIEnumCallback)( PTB_OutIntID    idOI      /*!< ID of Output Intent */
                                 , void*           userData  /*!< User data           */
                                 );

/*!
** \brief  Enumerate Output Intents
** 
**         Enumerates all Output Intents calling the user-supplied callback. 
** 
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_OIEnum( PTB_OIEnumCallback  cb        /*!< Callback that gets called for every Output Intent  */
          , void*               userData  /*!< User data submitted to callback                    */
          );


/*! Data type */
enum PTB_OIEDataType { PTB_OIEdtName                       = 0x0001 /*!< Output Intent's name                */
                     , PTB_OIEdtRegistry                   = 0x0003 /*!< Output Intent's registry            */
                     , PTB_OIEdtOutputCondition            = 0x0004 /*!< Output Intent's output condition    */
                     , PTB_OIEdtOutputConditionIdentifier  = 0x0005 /*!< Output Intent's output condition ID */
                     , PTB_OIEdtICCProfileName             = 0x0006 /*!< Output Intent's ICC profile name    */
                     , PTB_OIEdtICCProfileURL              = 0x0007 /*!< Output Intent's ICC profile URL for PDF/X-4p    */
                     };

/*!
** \brief  Get Output Intent data
** 
**         Retrieves Output Intent data depending on \a edt parameter. 
** 
** \retval  PTB_eerrOIInvalidOutIntID  Invalid Output Intent ID
** \retval  PTB_eerrOIInvalidDataType  Invalid data type
** 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_OIGetData( PTB_OutIntID         idOI   /*!< ID of Output Intent to get data from */
             , enum PTB_OIEDataType edt    /*!< Data type to retrieve                */
             , PTB_StringID*        idStr  /*!< Result string                        */
             );

/*******************************************************************************
** Manipulation of Output Intents
*/

/*! Output Intent data */
typedef struct {
	PTB_size_t                          size;                      /*!< Must be sizeof(PTB_OIData)    */
	const PTB_utf8_char_t*              name;                      /*!< Name of Output Intent         */
	const PTB_utf8_char_t*              registry;                  /*!< Output Intent registry        */
	const PTB_utf8_char_t*              outputCondition;           /*!< Output condition              */
	const PTB_utf8_char_t*              outputConditionIdentifier; /*!< Output condition identifier   */
	const PTB_Path_t*                   ICCProfilePath;            /*!< Path to ICC profile to import */
	const PTB_utf8_char_t*              ICCProfileURL;             /*!< ICC profile URL for PDF/X-4p  */
} PTB_OIData;

/*!
** \brief  Add an Output Intent
** 
**         This adds a new Output Intent to the callas pdfEngine SDK's 
**         Output Intent repository. 
** 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_OIAdd( const PTB_OIData* data   /*!< Output Intent data                */
         , PTB_OutIntID*     idNew  /*!< ID of newly created Output Intent */
         );

/*!
** \brief  Remove an Output Intent
**
**         This removes an Output Intent from the callas pdfEngine SDK's
**         Output Intent repository.
**
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_OIRemove( PTB_OutIntID     id  /*!< ID of the Output Intent */
            );

/*******************************************************************************
** exporting and importing Output Intents
*/

/*! 
** \brief  Import an Output Intent
** 
**         Imports an Output Intent from a file. 
** 
** \retval       PTB_eerrFileNameInvalid  Invalid file name
** \retval PTB_eerrFileSourceVolumeExist  Source volume does not exist
** \retval PTB_eerrFileSourceFolderExist  Source folder does not exist
** \retval   PTB_eerrFileSourceFileExist  Source file does not exist
** \retval        PTB_eerrFileSourceRead  No read access to source
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_OIImport( const PTB_Path_t* file   /*!< File path to import Output Intent from */
            , PTB_OutIntID*     idNew  /*!< ID of imported Output Intent           */
            );

/*! 
** \brief  Export an Output Intent
** 
**         Exports an Output Intent to a file. 
** 
** \retval    PTB_eerrOIInvalidOutIntID  Invalid Output Intent ID
** \retval      PTB_eerrFileNameInvalid  Invalid file name
** \retval  PTB_eerrFileDestVolumeExist  Destination volume does not exist
** \retval  PTB_eerrFileDestFolderExist  Destination folder does not exist
** \retval        PTB_eerrFileDestWrite  No write access to destination
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_OIExport( PTB_OutIntID      id     /*!< ID of Output Intent to export        */
            , const PTB_Path_t* file   /*!< File path to export Output Intent to */
            );

/*! 
** \brief  Export an Output Intent's ICC profile
** 
**         Exports an Output Intent's ICC profile to a file. 
** 
** \retval    PTB_eerrOIInvalidOutIntID  Invalid Output Intent ID
** \retval      PTB_eerrFileNameInvalid  Invalid file name
** \retval  PTB_eerrFileDestVolumeExist  Destination volume does not exist
** \retval  PTB_eerrFileDestFolderExist  Destination folder does not exist
** \retval PTB_eerrFileDestFolderCreate  Cannot create destination folder
** \retval        PTB_eerrFileDestWrite  No write access to destination
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_OIExportICC( PTB_OutIntID      id    /*!< ID of Output Intent to export      */
               , const PTB_Path_t* file  /*!< File path to export ICC profile to */
               );

/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_OUTINT_H */

/******************************************************************************/
/* EOF */
