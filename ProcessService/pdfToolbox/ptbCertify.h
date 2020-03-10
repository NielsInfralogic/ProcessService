/*!
**
** \file ptbCertify.h
**
** \brief callas pdfEngine SDK: Preflight Certificate
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_CERTIFY_H)
/*! \def PTB_CERTIFY_H header define */
#define PTB_CERTIFY_H

/******************************************************************************/

#include "ptbTypes.h"

/******************************************************************************/

#include "ptbProlog.h"

/******************************************************************************/

/*!
** \brief Embed a Preflight Certificate
** If you not used the flag 'PTB_epfCertify' in function 'PTB_Preflight5()', 
** you can use this function to embed the Preflight Certificate afterwards.
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PCertEmbed( PTB_ResultID                idResult             /*!< Result ID as returned by PTB_Preflight(), PTB_CheckPDFA() or PTB_ConvertPDFA() */
              , const PTB_Path_t*           dest                 /*!< Destination path (including file name) */
              );

/*!
** \brief 
*/
enum PTB_EPCertStatus { PTB_ecsError                 // Signature validity is unknown, An error occurred while attempting to validate the signature.
                      , PTB_ecsValidDocChanged       // Signature is valid. Document has been altered or corrupted since it was signed.
                      , PTB_ecsValidDocUnchanged     // Signature is valid. Document has not been changed since this signature was applied.
                      , PTB_ecsInvalidDocChanged     // Signature is invalid. Document has been altered or corrupted since it was signed.
                      , PTB_ecsInvalidDocUnchanged   // Signature is invalid. Document has not been changed since this signature was applied.
					  };

/*!
** \brief 
*/
enum PTB_EPCertResults { PTB_ecrErrors
                       , PTB_ecrWarnings
                       , PTB_ecrInfos
					   };

/*!
** \brief 
** 
*/
typedef void (*PTB_PCertCB)( enum PTB_EPCertStatus   status                        /*!< Signature status   */
                           , PTB_StringID            profile_name                  /*!< Profile name       */
                           , PTB_StringID            profile_creator               /*!< Creator            */
                           , PTB_StringID            profile_creator_version       /*!< Creator version    */
                           , PTB_StringID            profile_fingerprint           /*!< Fingerprint        */
						   , enum PTB_EPCertResults  result                        /*!< Preflight result   */
                           , PTB_StringID            preflight_results_string      /*!< Result string      */
                           , PTB_StringID            preflight_results_description /*!< Result description */
                           , PTB_StringID            preflight_executed_date       /*!< Executed date      */
                           , void*                   userData                      /*!< User data          */
                           );

/*!
** \brief Validate a Preflight Certificate
** 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PCertValidate( const PTB_Path_t*        doc                /*!< PDF Document */
                 , PTB_PCertCB              cbCertify          /*!< CB function */
                 , void*                    cbCertifyUserData  /*!< User data submitted to PTB_PCertCB callback function */
                 );

/*!
** \brief Remove a Preflight Certificate if present
** 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PCertRemove( const PTB_Path_t*          doc                  /*!< PDF Document */
               , const PTB_Path_t*          dest                 /*!< Destination path (including file name) */
               );

/******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_CERTIFY_H*/

/******************************************************************************/
/* EOF */
