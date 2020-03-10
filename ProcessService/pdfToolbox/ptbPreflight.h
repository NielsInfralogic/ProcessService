/*!
**
** \file ptbPreflight.h
**
** \brief callas pdfEngine SDK: Preflight API
** 
** The Preflight API has functions for preflighting documents and creating
** preflight reports. 
** 
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_PREFLIGHT_H)
/*! \def PTB_PREFLIGHT_H header define */
#define PTB_PREFLIGHT_H

/******************************************************************************/

#include "ptbTypes.h"
#include "ptbPRCTypes.h"
#include "ptbProgress.h"

/******************************************************************************/

#include "ptbProlog.h"

/******************************************************************************/

/*******************************************************************************
** Preflight
*/

/*!
** \brief Status of fixup execution
*/
enum PTB_EFixupState { PTB_efsFixupSuccess      /*!< A fixup succeeded         */
                     , PTB_efsFixupFailure      /*!< A fixup failed            */
                     , PTB_efsFixupNotRequired  /*!< A fixup was not required  */
                     };

/*!
** \brief  Fixup callback
** 
**          Prototype to create a callas pdfEngine SDK fixups callback function
** 
** \note   The PRC Engine ID passed to this callback function refers to an 
**         immutable internal engine. Any attempt to change the content of 
**         the engine passed to this function will result in a 
**         ::PTB_eerrPRCEngineOpNotSupported error. 
*/
typedef void (*PTB_FixupCB)( PTB_PRCEngineID           idEng     /*!< PRC Engine ID     */
                           , PTB_PRCFxupID             idFxup    /*!< PRC Fixup ID      */
                           , enum PTB_EFixupState      state     /*!< Fixup state       */
                           , PTB_StringID              idReason  /*!< Reason text       */
                           , void*                     userData  /*!< user data         */
                           );

/*!
** \brief  PTB_EHitCBResult provides the hit callback functions with return values
*/
enum PTB_EHitCBResult { PTB_ehcbrCancel      = 0x00  /*!< Cancel preflighting                      */
                      , PTB_ehcbrContinue    = 0x01  /*!< Continue preflighting                    */
                      , PTB_ehcbrSkipDetails = 0x03  /*!< Continue and skip result details for this hit only */
                      };

/*!
** \brief  Prototype to create a callas pdfEngine SDK hit callback function
** \note   On document hits is pageNum PTB_DOCUMENT_HIT
*/
typedef enum PTB_EHitCBResult (*PTB_HitCB)( PTB_PRCEngineID                idEng     /*!< PRC Engine ID       */
                                          , PTB_PRCProfID                  idProf    /*!< PRC Profile ID      */
                                          , PTB_PRCRuleID                  idRule    /*!< PRC Rule ID         */
                                          , PTB_ResultID                   idResult  /*!< Result ID           */
                                          , PTB_HitID                      idHit     /*!< Hit ID              */
                                          , enum PTB_PRCERuleCheckSeverity ruleSev   /*!< PRC Rule severity   */
                                          , PTB_uint32_t                   pageNum   /*!< Current page number */
                                          , void*                          userData  /*!< user data           */
                                          );

/*!
** \brief  PTB_ESaveAsReason reason why callas pdfEngine SDK wants to save the current file 
*/
enum PTB_ESaveAsReason { PTB_esarDirty         /*!< The file is dirty and must be saved before fixup             */
                       , PTB_esarReadOnly      /*!< The file is write protected and must be saved before fixup   */
                       , PTB_esarAlways        /*!< The file must be saved always before fixup                   */
                                               /* Save after Fixup processing (no PDF/X or PDF/A conversion):    */
                       , PTB_esarFixupSuccess  /*!< All fixups succeeded                                         */
                       , PTB_esarFixupError    /*!< At least one fixup failed */
                       };

/*!
** \brief  PTB_ESaveAsReasonPDFX reason why callas pdfEngine SDK wants to save the current file 
**                               only applicable if PTB_ESaveAsReason is PTB_esarFixupSuccess or PTB_esarFixupError
**                               and a PDF-X conversion was performed
*/
enum PTB_ESaveAsReasonPDFX { PTB_esarxNone     /*!< No PDF/X conversion        */
                           , PTB_esarxSuccess  /*!< PDF/X conversion succeeded */
                           , PTB_esarxError    /*!< PDF/X conversion failed    */
                           };


/*!
** \brief  PTB_ESaveAsReasonPDFA reason why callas pdfEngine SDK wants to save the current file 
**                               only applicable if PTB_ESaveAsReason is PTB_esarFixupSuccess or PTB_esarFixupError
**                               and a PDF-A conversion was performed
*/
enum PTB_ESaveAsReasonPDFA { PTB_esaraNone     /*!< No PDF/A conversion        */
                           , PTB_esaraSuccess  /*!< PDF/A conversion succeeded */
                           , PTB_esaraError    /*!< PDF/A conversion failed    */
                           };

/*!
** \brief  An enum to define the version of PDF/X 
*/
enum PTB_EPDFXVersion { PTB_epdfx1a_2001  /*!< PDF/X-1a:2001 */
                      , PTB_epdfx3_2002   /*!< PDF/X-3:2002  */
                      , PTB_epdfx1a_2003  /*!< PDF/X-1a:2003 */
                      , PTB_epdfx3_2003   /*!< PDF/X-3:2003  */
                      , PTB_epdfx4        /*!< PDF/X-4       */
                      };

/*!
** \brief  An enum to define the version of PDF/A
*/
enum PTB_EPDFAVersion { PTB_epdfa1a_2005   /*!< PDF/A-1a:2005 */
                      , PTB_epdfa1b_2005   /*!< PDF/A-1b:2005 */
                      , PTB_epdfa2a        /*!< PDF/A-2a      */
                      , PTB_epdfa2b        /*!< PDF/A-2b      */
                      , PTB_epdfa2u        /*!< PDF/A-2u      */
                      , PTB_epdfa3a        /*!< PDF/A-3a      */
                      , PTB_epdfa3b        /*!< PDF/A-3b      */
                      , PTB_epdfa3u        /*!< PDF/A-3u      */
                      };

/*!
** \brief    Prototype to create a callas pdfEngine SDK Save as callback function
**           The callback function shall return a new file spec (full path) for the 
**           current save as reason.
** \warning  The document must not be saved inside the callback!
*/
typedef PTB_Path_t* (*PTB_SaveCB)( PTB_ResultID               idResult     /*!< Result ID                           */
                                 , enum PTB_ESaveAsReason     reason       /*!< Reason why callas pdfEngine SDK wants to save the current file */
                                 , enum PTB_ESaveAsReasonPDFX reasonPDFX   /*!<  */
                                 , enum PTB_EPDFXVersion      versionPDFX  /*!<  */
                                 , enum PTB_ESaveAsReasonPDFA reasonPDFA   /*!<  */
                                 , enum PTB_EPDFAVersion      versionPDFA  /*!<  */
                                 , PTB_StringID               currentPath  /*!< current path of PDF document        */
                                 , void*                      userData     /*!< User data                           */
                                 );

/*!
** \brief  Flags to control some aspects during preflighting
*/
enum PTB_EPreflightFlags { PTB_epfNone             = 0x000  /*!< None */
                         , PTB_epfNoOptimization   = 0x001  /*!< Deactivates optimization of the internal structure when saving the PDF */
						 , PTB_epfFontQuickEmbed   = 0x002  /*!< (obsolete) Ignore missing glyphs or width mismatches */
                         , PTB_epfUncompressImages = 0x004  /*!< Analyzes image pixels for plate count */
                         , PTB_epfAnalyzeOnly      = 0x008  /*!< Analyze only; Fixups will not be applied */
                         , PTB_epfCertify          = 0x010  /*!< Embed a Preflight certificate after processing */
                         , PTB_epfCustomDict       = 0x040  /*!< Custom Dict: make localizable objects */
                         , PTB_epfEnableResDemux   = 0x080  /*!< Enable enhanced resource handling in the form of per reference processing of resources  */
                         , PTB_epfDisableResDemux  = 0x100  /*!< NOT IMPLEMENTED YET: Disable enhanced resource handling in the form of per reference processing of resources */
                         , PTB_epfLogExecution     = 0x200  /*!< Logs the files created during profile execution into a separate folder  */
                         };

/*! Syntax check severity */
enum PTB_ESyntaxCheckSeverity { PTB_escsDefault = -1 /*!< Default: use severity from the Profile */
                              , PTB_escsOff     = 0  /*!< Off     */
                              , PTB_escsInfo    = 1  /*!< Info    */
                              , PTB_escsWarning = 2  /*!< Warning */
                              , PTB_escsError   = 3  /*!< Error   */
                              };

/*!
** \brief  Runs preflight check
**         Runs a preflight check for a given document with a certain 
**         preflight profile
**
** \note   The results ID obtained by this call must be released using the
**         PTB_PreflightRelease() function.
**
** \note   All progress information is formatted in the current language
**
** General errors
** \retval PTB_eerrNone                      No error
** \retval PTB_eerrUnknown                   Unknown error
** \retval PTB_eerrInternal                  Internal error
** \retval PTB_eerrMemoryErr                 Not enough memory
** \retval PTB_eerrParamFailure              A parameter is unknown or incorrect
** \retval PTB_eerrIsBusy                    The preflight engine is busy
** File name errors
** \retval PTB_eerrFileNameInvalid           Invalid file name
** \retval PTB_eerrDestVolumeExist           Destination volume does not exist
** \retval PTB_eerrDestFolderExist           Destination folder does not exist
** \retval PTB_eerrDestFolderCreate          Cannot create destination folder 
** \retval PTB_eerrDestWrite                 No write access to destination 
** Initialization errors
** \retval PTB_eerrNotInitialized            Lib not initialized
** PRC errors
** \retval PTB_eerrInvalidEngineID           PRC Engine ID is invalid
** \retval PTB_eerrInvalidProfID             Profile ID is invalid
** Preflight errors
** \retval PTB_eerrPreflightNoPDFFile        The file is not a PDF file, unable to locate root object
** \retval PTB_eerrPreflightPDFHasNoPages    Unable to find the specified page
** \retval PTB_eerrPreflightOpenFileErr      The PDF file is corrupt (could not be opened)
** \retval PTB_eerrPreflightEncrypted        The PDF file is encrypted (processing of encrypted files in callas pdfEngine SDK not supported)
** \retval PTB_eerrPreflightContStrmFailure  An error occurred while parsing the contents stream (unable to analyse the PDF file)
** \retval PTB_eerrCancel                    The caller has canceled the process.
** \retval PTB_eerrPreflightPDFX3CreateFail  unable to convert the PDF file to PDF/X-3 (internal error/unknown exception)
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Preflight( const PTB_Path_t*         doc                  /*!< Document to preflight */
             , PTB_PRCEngineID           idEng                /*!< ID of PRC Engine that holds preflight profile */
             , PTB_PRCProfID             idProf               /*!< ID of PRC profile to be used for preflight check */
             , PTB_HitCB                 cbHit                /*!< CB function to be called for every hit, might be NULL */
             , void*                     cbHitUserData        /*!< User data submitted to hit callback function */
             , PTB_Preflight_ProgressCB  cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
             , void*                     cbProgressUserData   /*!< User data submitted to progress callback function */
             , PTB_SaveCB                cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
             , void*                     cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
             , PTB_uint32_t              firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
             , PTB_uint32_t              lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
             , PTB_ResultID*             idResult             /*!< On return ID for results from this preflight check, can be NULL to omit preflight check (if possible), please note that then no creation of preflight reports is possible */
             , void*                     reserved             /*!< currently unused for future used */
             );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Preflight2( const PTB_Path_t*          doc                  /*!< Document to preflight */
             , PTB_PRCEngineID             idEng                /*!< ID of PRC Engine that holds preflight profile */
             , PTB_PRCProfID               idProf               /*!< ID of PRC profile to be used for preflight check */
             , PTB_PRCEnumDynParamCallback cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */ 
             , void*                       cbDynParamUserData   /*!< User data submitted to dyn param callback function */
             , PTB_FixupCB                 cbFixup              /*!< CB function to be called for every hit, might be NULL */
             , void*                       cbFixupUserData      /*!< User data submitted to hit callback function */
             , PTB_HitCB                   cbHit                /*!< CB function to be called for every hit, might be NULL */
             , void*                       cbHitUserData        /*!< User data submitted to hit callback function */
             , PTB_Preflight_ProgressCB    cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
             , void*                       cbProgressUserData   /*!< User data submitted to progress callback function */
             , PTB_SaveCB                  cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
             , void*                       cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
             , PTB_uint32_t                firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
             , PTB_uint32_t                lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
             , PTB_ResultID*               idResult             /*!< On return ID for results from this preflight check, can be NULL to omit preflight check (if possible), please note that then no creation of preflight reports is possible */
             , void*                       reserved             /*!< currently unused for future used */
             );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Preflight3( const PTB_Path_t*          doc                  /*!< Document to preflight */
             , PTB_PRCEngineID             idEng                /*!< ID of PRC Engine that holds preflight profile */
             , PTB_PRCProfID               idProf               /*!< ID of PRC profile to be used for preflight check */
             , PTB_PRCEnumDynParamCallback cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */ 
             , void*                       cbDynParamUserData   /*!< User data submitted to dyn param callback function */
             , PTB_FixupCB                 cbFixup              /*!< CB function to be called for every hit, might be NULL */
             , void*                       cbFixupUserData      /*!< User data submitted to hit callback function */
             , PTB_HitCB                   cbHit                /*!< CB function to be called for every hit, might be NULL */
             , void*                       cbHitUserData        /*!< User data submitted to hit callback function */
             , PTB_Preflight_ProgressCB    cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
             , void*                       cbProgressUserData   /*!< User data submitted to progress callback function */
             , PTB_SaveCB                  cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
             , void*                       cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
             , PTB_bool_t                  fontquickembed       /*!< Ignore missing glyphs or width mismatches */
             , PTB_uint32_t                firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
             , PTB_uint32_t                lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
             , PTB_ResultID*               idResult             /*!< On return ID for results from this preflight check, can be NULL to omit preflight check (if possible), please note that then no creation of preflight reports is possible */
             , void*                       reserved             /*!< currently unused for future used */
             );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Preflight4( const PTB_Path_t*          doc                  /*!< Document to preflight */
             , PTB_PRCEngineID             idEng                /*!< ID of PRC Engine that holds preflight profile */
             , PTB_PRCProfID               idProf               /*!< ID of PRC profile to be used for preflight check */
             , PTB_PRCEnumDynParamCallback cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */ 
             , void*                       cbDynParamUserData   /*!< User data submitted to dyn param callback function */
             , PTB_FixupCB                 cbFixup              /*!< CB function to be called for every hit, might be NULL */
             , void*                       cbFixupUserData      /*!< User data submitted to hit callback function */
             , PTB_HitCB                   cbHit                /*!< CB function to be called for every hit, might be NULL */
             , void*                       cbHitUserData        /*!< User data submitted to hit callback function */
             , PTB_Preflight_ProgressCB    cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
             , void*                       cbProgressUserData   /*!< User data submitted to progress callback function */
             , PTB_SaveCB                  cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
             , void*                       cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
             , PTB_bool_t                  fontquickembed       /*!< Ignore missing glyphs or width mismatches */
             , PTB_bool_t                  uncompressimages     /*!< Analyzes image pixels for plate count */
             , PTB_uint32_t                firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
             , PTB_uint32_t                lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
             , PTB_ResultID*               idResult             /*!< On return ID for results from this preflight check, can be NULL to omit preflight check (if possible), please note that then no creation of preflight reports is possible */
             , void*                       reserved             /*!< currently unused for future used */
             );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Preflight5( const PTB_Path_t*          doc                  /*!< Document to preflight */
             , PTB_PRCEngineID             idEng                /*!< ID of PRC Engine that holds preflight profile */
             , PTB_PRCProfID               idProf               /*!< ID of PRC profile to be used for preflight check */
             , PTB_PRCEnumDynParamCallback cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */ 
             , void*                       cbDynParamUserData   /*!< User data submitted to dyn param callback function */
             , PTB_FixupCB                 cbFixup              /*!< CB function to be called for every hit, might be NULL */
             , void*                       cbFixupUserData      /*!< User data submitted to hit callback function */
             , PTB_HitCB                   cbHit                /*!< CB function to be called for every hit, might be NULL */
             , void*                       cbHitUserData        /*!< User data submitted to hit callback function */
             , PTB_Preflight_ProgressCB    cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
             , void*                       cbProgressUserData   /*!< User data submitted to progress callback function */
             , PTB_SaveCB                  cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
             , void*                       cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
             , PTB_uint32_t                flags                /*!< Flags (any combination of PTB_EPreflightFlags values) */
             , PTB_uint32_t                firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
             , PTB_uint32_t                lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
             , PTB_ResultID*               idResult             /*!< On return ID for results from this preflight check, can be NULL to omit preflight check (if possible), please note that then no creation of preflight reports is possible */
             , void*                       reserved             /*!< currently unused for future used */
             );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Preflight6( const PTB_Path_t*            doc                  /*!< Document to preflight */
			   , PTB_PRCEngineID             idEng                /*!< ID of PRC Engine that holds preflight profile */
			   , PTB_PRCProfID               idProf               /*!< ID of PRC profile to be used for preflight check */
			   , PTB_PRCEnumDynParamCallback cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */ 
			   , void*                       cbDynParamUserData   /*!< User data submitted to dyn param callback function */
			   , PTB_FixupCB                 cbFixup              /*!< CB function to be called for every hit, might be NULL */
			   , void*                       cbFixupUserData      /*!< User data submitted to hit callback function */
			   , PTB_HitCB                   cbHit                /*!< CB function to be called for every hit, might be NULL */
			   , void*                       cbHitUserData        /*!< User data submitted to hit callback function */
			   , PTB_Preflight_ProgressCB    cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
			   , void*                       cbProgressUserData   /*!< User data submitted to progress callback function */
			   , PTB_SaveCB                  cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
			   , void*                       cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
			   , PTB_uint32_t                flags                /*!< Flags (any combination of PTB_EPreflightFlags values) */
			   , PTB_uint32_t                hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
			   , PTB_uint32_t                hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
			   , PTB_uint32_t                firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
			   , PTB_uint32_t                lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
			   , PTB_ResultID*               idResult             /*!< On return ID for results from this preflight check, can be NULL to omit preflight check (if possible), please note that then no creation of preflight reports is possible */
			   , void*                       reserved             /*!< currently unused for future used */
			   );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Preflight7( const PTB_Path_t*            doc                  /*!< Document to preflight */
			   , PTB_PRCEngineID             idEng                /*!< ID of PRC Engine that holds preflight profile */
			   , PTB_PRCProfID               idProf               /*!< ID of PRC profile to be used for preflight check */
			   , PTB_PRCEnumDynParamCallback cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */ 
			   , void*                       cbDynParamUserData   /*!< User data submitted to dyn param callback function */
			   , PTB_FixupCB                 cbFixup              /*!< CB function to be called for every hit, might be NULL */
			   , void*                       cbFixupUserData      /*!< User data submitted to hit callback function */
			   , PTB_HitCB                   cbHit                /*!< CB function to be called for every hit, might be NULL */
			   , void*                       cbHitUserData        /*!< User data submitted to hit callback function */
			   , PTB_Preflight_ProgressCB    cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
			   , void*                       cbProgressUserData   /*!< User data submitted to progress callback function */
			   , PTB_SaveCB                  cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
			   , void*                       cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
			   , PTB_uint32_t                flags                /*!< Flags (any combination of PTB_EPreflightFlags values) */
			   , PTB_uint32_t                hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
			   , PTB_uint32_t                hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
			   , PTB_uint32_t                maxpages             /*!< Maximum number of pages - after this limit only pages that have problems will be stored, might be PTB_MAX_PAGE_DEFAULT_NUM */
			   , PTB_uint32_t                firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
			   , PTB_uint32_t                lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
			   , PTB_ResultID*               idResult             /*!< On return ID for results from this preflight check, can be NULL to omit preflight check (if possible), please note that then no creation of preflight reports is possible */
			   , void*                       reserved             /*!< currently unused for future used */
			   );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Preflight8( const PTB_Path_t*            doc                  /*!< Document to preflight */
			   , PTB_PRCEngineID             idEng                /*!< ID of PRC Engine that holds preflight profile */
			   , PTB_PRCProfID               idProf               /*!< ID of PRC profile to be used for preflight check */
			   , PTB_PRCEnumDynParamCallback cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */ 
			   , void*                       cbDynParamUserData   /*!< User data submitted to dyn param callback function */
			   , PTB_FixupCB                 cbFixup              /*!< CB function to be called for every hit, might be NULL */
			   , void*                       cbFixupUserData      /*!< User data submitted to hit callback function */
			   , PTB_HitCB                   cbHit                /*!< CB function to be called for every hit, might be NULL */
			   , void*                       cbHitUserData        /*!< User data submitted to hit callback function */
			   , PTB_Preflight_ProgressCB    cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
			   , void*                       cbProgressUserData   /*!< User data submitted to progress callback function */
			   , PTB_SaveCB                  cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
			   , void*                       cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
			   , PTB_uint32_t                flags                /*!< Flags (any combination of PTB_EPreflightFlags values) */
			   , PTB_uint32_t                hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
			   , PTB_uint32_t                hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
			   , PTB_uint32_t                maxpages             /*!< Maximum number of pages - after this limit only pages that have problems will be stored, might be PTB_MAX_PAGE_DEFAULT_NUM */
			   , const PTB_Path_t*           refxobjectpath       /*!< Search path for use with property 'File referenced by reference XObject not found' only, might be NULL to search only beside the input file */
			   , PTB_uint32_t                firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
			   , PTB_uint32_t                lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
			   , PTB_ResultID*               idResult             /*!< On return ID for results from this preflight check, can be NULL to omit preflight check (if possible), please note that then no creation of preflight reports is possible */
			   , void*                       reserved             /*!< currently unused for future used */
			   );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Preflight9( const PTB_Path_t*              doc                  /*!< Document to preflight */
			   , PTB_PRCEngineID               idEng                /*!< ID of PRC Engine that holds preflight profile */
			   , PTB_PRCProfID                 idProf               /*!< ID of PRC profile to be used for preflight check */
			   , PTB_PRCEnumDynParamCallback   cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */
			   , void*                         cbDynParamUserData   /*!< User data submitted to dyn param callback function */
			   , PTB_FixupCB                   cbFixup              /*!< CB function to be called for every hit, might be NULL */
			   , void*                         cbFixupUserData      /*!< User data submitted to hit callback function */
			   , PTB_HitCB                     cbHit                /*!< CB function to be called for every hit, might be NULL */
			   , void*                         cbHitUserData        /*!< User data submitted to hit callback function */
			   , PTB_Preflight_ProgressCB      cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
			   , void*                         cbProgressUserData   /*!< User data submitted to progress callback function */
			   , PTB_SaveCB                    cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
			   , void*                         cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
			   , PTB_uint32_t                  flags                /*!< Flags (any combination of PTB_EPreflightFlags values) */
			   , PTB_uint32_t                  hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
			   , PTB_uint32_t                  hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
			   , PTB_uint32_t                  maxpages             /*!< Maximum number of pages - after this limit only pages that have problems will be stored, might be PTB_MAX_PAGE_DEFAULT_NUM */
			   , const PTB_Path_t*             refxobjectpath       /*!< Search path for use with property 'File referenced by reference XObject not found' only, might be NULL to search only beside the input file */
			   , enum PTB_ESyntaxCheckSeverity syntaxChecks         /*!< Syntax checks severity, see PTB_ESyntaxCheckSeverity */
			   , PTB_uint32_t                  firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
			   , PTB_uint32_t                  lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
			   , PTB_ResultID*                 idResult             /*!< On return ID for results from this preflight check, can be NULL to omit preflight check (if possible), please note that then no creation of preflight reports is possible */
			   , void*                         reserved             /*!< currently unused for future used */
			   );

/*!
** \brief Status of sequence
*/
enum PTB_ESequenceState { PTB_essBegin          /*!< Sequence begins                             */
                        , PTB_essNone           /*!< Sequence Profile or Check does not hit      */
                        , PTB_essInfo           /*!< Sequence Profile or Check hits with info    */
                        , PTB_essWarning        /*!< Sequence Profile or Check hits with warning */
                        , PTB_essError          /*!< Sequence Profile or Check hits with error   */
                        , PTB_essSuccess        /*!< Sequence Fixup or Action succeeded          */
                        , PTB_essFailure        /*!< Sequence Fixup or Action failed             */
                        , PTB_essEnd            /*!< Sequence ends                               */
                        };

/*!
** \brief  Prototype to create a callas pdfEngine SDK sequence callback function
*/
typedef void (*PTB_SequenceCB)( PTB_PRCEngineID            idEng      /*!< PRC Engine ID   */
                              , PTB_PRCProfID              idProf     /*!< If idProf unequal PTB_INVALID_ID sequence is a Profile */
                              , PTB_PRCRuleID              idRule     /*!< If idRule unequal PTB_INVALID_ID sequence is a Rule    */
                              , PTB_PRCFxupID              idFxup     /*!< If idFxup unequal PTB_INVALID_ID sequence is a Fixup   */
                              , PTB_PRCActnID              idActn     /*!< If idActn unequal PTB_INVALID_ID sequence is an Action */
                              , enum PTB_ESequenceState    state      /*!< Sequence state, see PTB_ESequenceState */
                              , void*                      userData   /*!< User data       */
                              );

/*!
** \brief  Inspk_ESequenceReason reason why the callas pdfEngine SDK wants to save the current file 
*/
enum PTB_ESequenceReason { PTB_esrSequenceStep   /*!< The file must be saved for the sequence                      */
                         , PTB_esrSequenceReport /*!< The file is a sequence report and must be saved              */
                         , PTB_esrSequenceAction /*!< The file is a sequence action result and was saved, the file will be moved if you change the file */
                         };

/*!
** \brief  Prototype to create a callas pdfEngine SDK sequence save callback function
*/
typedef PTB_Path_t* (*PTB_SequenceSaveCB)( PTB_ESequenceReason reason         /*!< Reason why wants to save the current file */
                                         , PTB_StringID        idCurrentPath  /*!< current path of document                  */
                                         , void*               userData       /*!< User data                                 */
                                         );

/*!
** \brief  Run a meta profile (Process plan) 
**         Runs a preflight check for a given document with a certain 
**         preflight profile
**
** \note   The results ID obtained by this call must be released using the
**         PTB_PreflightRelease() function.
**
** \note   All progress information is formatted in the current language
**
** General errors
** \retval PTB_eerrNone                      No error
** \retval PTB_eerrUnknown                   Unknown error
** \retval PTB_eerrInternal                  Internal error
** \retval PTB_eerrMemoryErr                 Not enough memory
** \retval PTB_eerrParamFailure              A parameter is unknown or incorrect
** \retval PTB_eerrIsBusy                    The preflight engine is busy
** File name errors
** \retval PTB_eerrFileNameInvalid           Invalid file name
** \retval PTB_eerrDestVolumeExist           Destination volume does not exist
** \retval PTB_eerrDestFolderExist           Destination folder does not exist
** \retval PTB_eerrDestFolderCreate          Cannot create destination folder 
** \retval PTB_eerrDestWrite                 No write access to destination 
** Initialization errors
** \retval PTB_eerrNotInitialized            Lib not initialized
** PRC errors
** \retval PTB_eerrInvalidEngineID           PRC Engine ID is invalid
** \retval PTB_eerrInvalidProfID             Profile ID is invalid
** Preflight errors
** \retval PTB_eerrPreflightNoPDFFile        The file is not a PDF file, unable to locate root object
** \retval PTB_eerrPreflightPDFHasNoPages    Unable to find the specified page
** \retval PTB_eerrPreflightOpenFileErr      The PDF file is corrupt (could not be opened)
** \retval PTB_eerrPreflightEncrypted        The PDF file is encrypted (processing of encrypted files in callas pdfEngine SDK not supported)
** \retval PTB_eerrPreflightContStrmFailure  An error occurred while parsing the contents stream (unable to analyse the PDF file)
** \retval PTB_eerrCancel                    The caller has canceled the process.
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PreflightMetaProfile( const PTB_Path_t*           doc                  /*!< Document to preflight */
                        , PTB_PRCEngineID             idEng                /*!< ID of PRC Engine that holds preflight meta profile */
                        , PTB_PRCMPrfID               idMPrf               /*!< ID of PRC meta profile to be used for preflight check */
                        , PTB_PRCEnumDynParamCallback cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */ 
                        , void*                       cbDynParamUserData   /*!< User data submitted to dyn param callback function */
                        , PTB_SequenceCB              cbSequence           /*!< CB function to be called for every sequence, might be NULL */
                        , void*                       cbSequenceUserData   /*!< User data submitted to sequence callback function */
                        , PTB_FixupCB                 cbFixup              /*!< CB function to be called for every hit, might be NULL */
                        , void*                       cbFixupUserData      /*!< User data submitted to hit callback function */
                        , PTB_HitCB                   cbHit                /*!< CB function to be called for every hit, might be NULL */
                        , void*                       cbHitUserData        /*!< User data submitted to hit callback function */
                        , PTB_Preflight_ProgressCB    cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                        , void*                       cbProgressUserData   /*!< User data submitted to progress callback function */
                        , PTB_SequenceSaveCB          cbSave               /*!< CB function to be called if the SDK needs to save th document or a report, if NULL the original 'doc' will be overwritten */
                        , void*                       cbSaveUserData       /*!< User data submitted to PTB_SequenceSaveCB callback function */
                        , PTB_uint32_t                flags                /*!< Flags (any combination of PTB_EPreflightFlags values) */
                        , PTB_uint32_t                firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
                        , PTB_uint32_t                lastPage             /*!< Last page to be preflighted; might be PTB_MAX_PAGE_DEFAULT_NUM for all pages */
                        , PTB_ResultID*               idResult             /*!< On return ID for results from this preflight run, please note only the report data of the last sequence step is available */
                        , void*                       reserved             /*!< currently unused for future used */
                        );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PreflightMetaProfile2( const PTB_Path_t*           doc                  /*!< Document to preflight */
						 , PTB_PRCEngineID             idEng                /*!< ID of PRC Engine that holds preflight meta profile */
						 , PTB_PRCMPrfID               idMPrf               /*!< ID of PRC meta profile to be used for preflight check */
						 , PTB_PRCEnumDynParamCallback cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */ 
						 , void*                       cbDynParamUserData   /*!< User data submitted to dyn param callback function */
						 , PTB_SequenceCB              cbSequence           /*!< CB function to be called for every sequence, might be NULL */
						 , void*                       cbSequenceUserData   /*!< User data submitted to sequence callback function */
						 , PTB_FixupCB                 cbFixup              /*!< CB function to be called for every hit, might be NULL */
						 , void*                       cbFixupUserData      /*!< User data submitted to hit callback function */
						 , PTB_HitCB                   cbHit                /*!< CB function to be called for every hit, might be NULL */
						 , void*                       cbHitUserData        /*!< User data submitted to hit callback function */
						 , PTB_Preflight_ProgressCB    cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
						 , void*                       cbProgressUserData   /*!< User data submitted to progress callback function */
						 , PTB_SequenceSaveCB          cbSave               /*!< CB function to be called if the SDK needs to save th document or a report, if NULL the original 'doc' will be overwritten */
						 , void*                       cbSaveUserData       /*!< User data submitted to PTB_SequenceSaveCB callback function */
						 , PTB_uint32_t                flags                /*!< Flags (any combination of PTB_EPreflightFlags values) */
						 , PTB_uint32_t                hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
						 , PTB_uint32_t                hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
						 , PTB_uint32_t                firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
						 , PTB_uint32_t                lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
						 , PTB_ResultID*               idResult             /*!< On return ID for results from this preflight run, please note only the report data of the last sequence step is available */
						 , void*                       reserved             /*!< currently unused for future used */
						 );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PreflightMetaProfile3( const PTB_Path_t*           doc                  /*!< Document to preflight */
						 , PTB_PRCEngineID             idEng                /*!< ID of PRC Engine that holds preflight meta profile */
						 , PTB_PRCMPrfID               idMPrf               /*!< ID of PRC meta profile to be used for preflight check */
						 , PTB_PRCEnumDynParamCallback cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */ 
						 , void*                       cbDynParamUserData   /*!< User data submitted to dyn param callback function */
						 , PTB_SequenceCB              cbSequence           /*!< CB function to be called for every sequence, might be NULL */
						 , void*                       cbSequenceUserData   /*!< User data submitted to sequence callback function */
						 , PTB_FixupCB                 cbFixup              /*!< CB function to be called for every hit, might be NULL */
						 , void*                       cbFixupUserData      /*!< User data submitted to hit callback function */
						 , PTB_HitCB                   cbHit                /*!< CB function to be called for every hit, might be NULL */
						 , void*                       cbHitUserData        /*!< User data submitted to hit callback function */
						 , PTB_Preflight_ProgressCB    cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
						 , void*                       cbProgressUserData   /*!< User data submitted to progress callback function */
						 , PTB_SequenceSaveCB          cbSave               /*!< CB function to be called if the SDK needs to save th document or a report, if NULL the original 'doc' will be overwritten */
						 , void*                       cbSaveUserData       /*!< User data submitted to PTB_SequenceSaveCB callback function */
						 , PTB_uint32_t                flags                /*!< Flags (any combination of PTB_EPreflightFlags values) */
						 , PTB_uint32_t                hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
						 , PTB_uint32_t                hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
						 , PTB_uint32_t                maxpages             /*!< Maximum number of pages - after this limit only pages that have problems will be stored, might be PTB_MAX_PAGE_DEFAULT_NUM */
						 , PTB_uint32_t                firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
						 , PTB_uint32_t                lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
						 , PTB_ResultID*               idResult             /*!< On return ID for results from this preflight run, please note only the report data of the last sequence step is available */
						 , void*                       reserved             /*!< currently unused for future used */
						 );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PreflightMetaProfile4( const PTB_Path_t*             doc                  /*!< Document to preflight */
						 , PTB_PRCEngineID               idEng                /*!< ID of PRC Engine that holds preflight meta profile */
						 , PTB_PRCMPrfID                 idMPrf               /*!< ID of PRC meta profile to be used for preflight check */
						 , PTB_PRCEnumDynParamCallback   cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */
						 , void*                         cbDynParamUserData   /*!< User data submitted to dyn param callback function */
						 , PTB_SequenceCB                cbSequence           /*!< CB function to be called for every sequence, might be NULL */
						 , void*                         cbSequenceUserData   /*!< User data submitted to sequence callback function */
						 , PTB_FixupCB                   cbFixup              /*!< CB function to be called for every hit, might be NULL */
						 , void*                         cbFixupUserData      /*!< User data submitted to hit callback function */
						 , PTB_HitCB                     cbHit                /*!< CB function to be called for every hit, might be NULL */
						 , void*                         cbHitUserData        /*!< User data submitted to hit callback function */
						 , PTB_Preflight_ProgressCB      cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
						 , void*                         cbProgressUserData   /*!< User data submitted to progress callback function */
						 , PTB_SequenceSaveCB            cbSave               /*!< CB function to be called if the SDK needs to save th document or a report, if NULL the original 'doc' will be overwritten */
						 , void*                         cbSaveUserData       /*!< User data submitted to PTB_SequenceSaveCB callback function */
						 , PTB_uint32_t                  flags                /*!< Flags (any combination of PTB_EPreflightFlags values) */
						 , PTB_uint32_t                  hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
						 , PTB_uint32_t                  hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
						 , PTB_uint32_t                  maxpages             /*!< Maximum number of pages - after this limit only pages that have problems will be stored, might be PTB_MAX_PAGE_DEFAULT_NUM */
						 , enum PTB_ESyntaxCheckSeverity syntaxChecks         /*!< Syntax checks severity, see PTB_ESyntaxCheckSeverity */
						 , PTB_uint32_t                  firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
						 , PTB_uint32_t                  lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
						 , PTB_ResultID*                 idResult             /*!< On return ID for results from this preflight run, please note only the report data of the last sequence step is available */
						 , void*                         reserved             /*!< currently unused for future used */
						 );


/*!
** \brief  Prototype to create a callas pdfEngine SDK sequence callback function
*/
typedef void (*PTB_SequenceCB2)( PTB_PRCEngineID            idEng      /*!< ID of PRC Engine where Meta Profile is stored              */
                               , PTB_PRCMPrfID              idMPrf     /*!< If idMPrf unequal PTB_INVALID_ID sequence is a MetaProfile */
                               , PTB_PRCProfID              idProf     /*!< If idProf unequal PTB_INVALID_ID sequence is a Profile     */
                               , PTB_PRCRuleID              idRule     /*!< If idRule unequal PTB_INVALID_ID sequence is a Rule        */
                               , PTB_PRCFxupID              idFxup     /*!< If idFxup unequal PTB_INVALID_ID sequence is a Fixup       */
                               , PTB_PRCActnID              idActn     /*!< If idActn unequal PTB_INVALID_ID sequence is an Action     */
                               , PTB_PRCDynParamID          idParam    /*!< If idParam unequal PTB_INVALID_ID sequence is an Variable  */
                               , PTB_StringID               idOper     /*!< If idOper unequal PTB_INVALID_ID sequence is an Operation (File pickup or Create PDF copy) */
                               , enum PTB_ESequenceState    state      /*!< Sequence state, see PTB_ESequenceState */
                               , void*                      userData   /*!< User data       */
                               );

PTB_FUNC_PROTO
enum PTB_EError
PTB_PreflightMetaProfile5( const PTB_Path_t*             doc                  /*!< Document to preflight */
						 , PTB_PRCEngineID               idEng                /*!< ID of PRC Engine that holds preflight meta profile */
						 , PTB_PRCMPrfID                 idMPrf               /*!< ID of PRC meta profile to be used for preflight check */
						 , PTB_PRCEnumDynParamCallback   cbDynParam           /*!< CB function to be called for every dynamic param in profile, if NULL, the default values are used */
						 , void*                         cbDynParamUserData   /*!< User data submitted to dyn param callback function */
						 , PTB_SequenceCB2               cbSequence           /*!< CB function to be called for every sequence, might be NULL */
						 , void*                         cbSequenceUserData   /*!< User data submitted to sequence callback function */
						 , PTB_FixupCB                   cbFixup              /*!< CB function to be called for every hit, might be NULL */
						 , void*                         cbFixupUserData      /*!< User data submitted to hit callback function */
						 , PTB_HitCB                     cbHit                /*!< CB function to be called for every hit, might be NULL */
						 , void*                         cbHitUserData        /*!< User data submitted to hit callback function */
						 , PTB_Preflight_ProgressCB      cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
						 , void*                         cbProgressUserData   /*!< User data submitted to progress callback function */
						 , PTB_SequenceSaveCB            cbSave               /*!< CB function to be called if the SDK needs to save th document or a report, if NULL the original 'doc' will be overwritten */
						 , void*                         cbSaveUserData       /*!< User data submitted to PTB_SequenceSaveCB callback function */
						 , PTB_uint32_t                  flags                /*!< Flags (any combination of PTB_EPreflightFlags values) */
						 , PTB_uint32_t                  hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
						 , PTB_uint32_t                  hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
						 , PTB_uint32_t                  maxpages             /*!< Maximum number of pages - after this limit only pages that have problems will be stored, might be PTB_MAX_PAGE_DEFAULT_NUM */
						 , enum PTB_ESyntaxCheckSeverity syntaxChecks         /*!< Syntax checks severity, see PTB_ESyntaxCheckSeverity */
						 , PTB_uint32_t                  firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
						 , PTB_uint32_t                  lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
						 , PTB_ResultID*                 idResult             /*!< On return ID for results from this preflight run, please note only the report data of the last sequence step is available */
						 , void*                         reserved             /*!< currently unused for future used */
						 );

/*!
** \brief  Release all storage associated with a preflight check's results
**
** General errors
** \retval PTB_eerrNone                 No error
** \retval PTB_eerrUnknown              Unknown error
** \retval PTB_eerrInternal             Internal error
** Initialization errors
** \retval PTB_eerrNotInitialized       Lib not initialized
** Preflight errors
** \retval PTB_eerrPreflightInvalidID   Invalid preflight ID
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PreflightRelease( PTB_ResultID          idResult          /*!< Result ID as returned by PTB_Preflight(), PTB_PreflightMetaProfile(), PTB_CheckPDFA() or PTB_ConvertPDFA() */
                    , void*                 reserved          /*!< currently unused for future used */
                    );

/******************************************************************************/
// Reports 

/*!
** \brief  An enum to define all available report types
*/
enum PTB_EReportType { PTB_eTextSummary      = 0x000     /*!< A compact summary text report */
                     , PTB_eTextResults                  /*!< A full text report, like in Preflight for Acrobat 7 */
                     , PTB_eXMLResults       = 0x100     /*!< A full XML report, like in Preflight for Acrobat 7 */
                     , PTB_eXMLResults_V2                /*!< XML report based on XML Schema http://www.callassoftware.com/namespace/pi4 */
                     , PTB_eXSLT                         /*!< XSLT based report */
                     , PTB_eXMLMetadata                  /*!< XML Metadata report (currently not supported) */
                     , PTB_ePDFBreakout      = 0x200     /*!< A PDF breakout report, like in Preflight for Acrobat 6 */
                     , PTB_ePDFSummary                   /*!< A compact summary PDF report */
                     , PTB_ePDFResult                    /*!< A PDF result report, like in Preflight for Acrobat 7 (PDF Summary)  */
                     , PTB_ePDFImageMask                 /*!< A PDF image mask report, like in Preflight for Acrobat 7 */
                     , PTB_ePDFComment                   /*!< A PDF comment report, like in Preflight for Acrobat 7 */
                     , PTB_ePDFLayer                     /*!< A PDF layer report */
                     , PTB_eInventory                    /*!< A PDF report contains all used resources on separate pages, like in Preflight for Acrobat 8 */
                     , PTB_eHTML             = 0x300     /*!< A HTML report, like in pdfaPilot */
					 , PTB_eCChip            = 0x400     /*!< A PDF based on HTML template created with pdfChip engine */
					 , PTB_eVarDump          = 0x500     /*!< Dumps the app.vars object into JSON */
                    };

/*!
** \brief  An enum to define the text report line endings
**         NOTE: Used only if the report type is PTB_eTextSummary or PTB_eTextResults
*/
enum PTB_ETextLineEndings { PTB_eCR       /*!< Set the line ending to 0x0d (carriage return), like Macintosh */
                          , PTB_eLF       /*!< Set the line ending to 0x0a (linefeed), like Unix */
                          , PTB_eCRLF     /*!< Set the line ending to 0x0d 0x0a (carriage return, linefeed), like Windows */
                          };

/*!
** \brief  An enum to define the text report encoding format
**         NOTE: Used only if the report type is PTB_eTextSummary or PTB_eTextResults
*/
enum PTB_ETextEncoding { PTB_eUTF8        /*!< Use UTF8 as text encoding, the file begins with (0xEF 0xBB 0xBF) */
                       , PTB_eUTF16BE     /*!< Use UTF16 with big endian (like powerpc) as text encoding, the file begins with (0xFE 0xFF) */
                       , PTB_eUTF16LE     /*!< Use UTF16 with little endian (like intel) as text encoding, the file begins with (0xFF 0xFE) */
                       };

/*!
** \brief  A bitmask to select which parts of a report should be written
*/
enum PTB_EResultParts { PTB_eHits                  = 0x0000001  /*!< Include the preflight results part in the report */
                      , PTB_ePage                  = 0x0000002  /*!< Include the page by page overview part to report */
                      , PTB_eDoc                   = 0x0000004  /*!< Include the document overview part to report */
                      , PTB_eICCNames              = 0x0000008  /*!< Include the icc colorspace names as part of report (can only used in pdf layer report) */
                      , PTB_eSpotNames             = 0x0000010  /*!< Include the spot colorspace names as part of report (can only used in pdf layer report) */
                      , PTB_eFontNames             = 0x0000020  /*!< Include the font names as part of report (can only used in pdf layer report) */
                      , PTB_eWithSummary           = 0x0000040  /*!< Prepands pdf result report to layer, imagemask or comment report */
                      , PTB_eExtImgMask            = 0x0000080  /*!< Will create an extended image mask report */
                      , PTB_eInvFonts              = 0x0000100  /*!< Include the fonts as part of the inventory report */
                      , PTB_eInvColors             = 0x0000200  /*!< Include the colors as part of the inventory report */
                      , PTB_eInvSmoothShades       = 0x0000400  /*!< Include the smooths shades as part of the inventory report */
                      , PTB_eInvPattern            = 0x0000800  /*!< Include the pattern as part of the inventory report */
                      , PTB_eInvImages             = 0x0001000  /*!< Include the image as part of the inventory report */
                      , PTB_eInvFormXObjects       = 0x0002000  /*!< Include the FormXObjects as part of the inventory report */
                      , PTB_eInvXMP                = 0x0004000  /*!< Include the XMP as part of the inventory report */
                      , PTB_eInvAdvancedXMP        = 0x0008000  /*!< Include the advanced XMP as part of the inventory report */
                      , PTB_eHTMLasMHT             = 0x0010000  /*!< Creates the HTML report as MHT report */
                      , PTB_eHTMLNoIcons           = 0x0020000  /*!< A HTML/MHT report without images */
                      , PTB_eHTMLNoExplanations    = 0x0040000  /*!< A HTML/MHT report without explanation pages (currently not supported) */
                      , PTB_eHTMLNoCorrections     = 0x0080000  /*!< A HTML/MHT report without corrections */
                      , PTB_eHTMLNoDetails         = 0x0100000  /*!< A HTML/MHT report without details for the occurances */
                      , PTB_eHTMLOpenResults       = 0x0200000  /*!< All entries in the HTML/MHT report are opend in the initial view (if JavaScript is enabled they are otherwise closed) */
					  , PTB_eHideInfos             = 0x0400000  /*!< Hide all checks with severity info */
					  , PTB_eHideWarnings          = 0x0800000  /*!< Hide all checks with severity warning */
					  , PTB_eHideErrors            = 0x1000000  /*!< Hide all checks with severity error */
                      , PTB_eWithCChipSummary      = 0x2000000  /*!< Prepands pdf result based on HTML report to layer, imagemask or comment report */
};

/*!
** \brief  An enum to define the amount of detail written to the report
*/
enum PTB_EResultDetails { PTB_eNoDetails            /*!< No details will be written to report */
                        , PTB_eImportantDetails     /*!< Only important details be written to report  */
                        , PTB_eAllDetails           /*!< All details will be written to report */
                        };

/*!
** \brief  Create a report for a preflight check's results
**
** \note   The report and all progress information is formatted in the current language
**
** General errors
** \retval PTB_eerrNone                     No error                          
** \retval PTB_eerrUnknown                  Unknown error                     
** \retval PTB_eerrInternal                 Internal error                    
** \retval PTB_eerrMemoryErr                Not enough memory                 
** \retval PTB_eerrParamFailure             A parameter is unknown or incorrect
** \retval PTB_eerrIsBusy                   The preflight engine is busy    
** File name errors
** \retval PTB_eerrFileNameInvalid          Invalid file name                 
** \retval PTB_eerrDestVolumeExist          Destination volume does not exist        
** \retval PTB_eerrDestFolderExist          Destination folder does not exist        
** \retval PTB_eerrDestFolderCreate         Cannot create destination folder         
** \retval PTB_eerrDestWrite                No write access to destination           
** initialization errors
** \retval PTB_eerrNotInitialized           Lib not initialized               
** Preflight errors
** \retval PTB_eerrPreflightInvalidID       Invalid preflight ID
** \retval PTB_eerrCancel                   The caller has canceled the process.
** \retval PTB_eerrPreflightFileError       Unable to write the report file.
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PreflightReport( PTB_ResultID              idResult             /*!< Result ID as returned by PTB_Preflight(), PTB_CheckPDFA() or PTB_ConvertPDFA() */
                   , const PTB_Path_t*         dest                 /*!< Full file path for the report file */
                   , enum PTB_EReportType      type                 /*!< Text, TextSummary, XML, PDFBreakout, PDFMask, ... */
                   , enum PTB_ETextLineEndings txtending            /*!< Text reports only: CR LF CRLF */
                   , enum PTB_ETextEncoding    txtencode            /*!< Text reports only: UTF8 UTF 16,... */
                   , PTB_uint32_t              parts                /*!< Which part (any combination of enum PTB_EResultParts) */
                   , enum PTB_EResultDetails   details              /*!< Which amount of details to include in report */
                   , PTB_Preflight_ProgressCB  cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                   , void*                     cbProgressUserData   /*!< User data submitted to progress callback function */
                   , PTB_uint32_t              firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
                   , PTB_uint32_t              lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
                   , void*                     reserved             /*!< currently unused for future used */
                   );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Report( PTB_ResultID              idResult             /*!< Result ID as returned by PTB_Preflight(), PTB_CheckPDFA() or PTB_ConvertPDFA() */
          , const PTB_Path_t*         dest                 /*!< Full file path for the report file */
          , enum PTB_EReportType      type                 /*!< Text, TextSummary, XML, PDFBreakout, PDFMask, ...*/
          , enum PTB_ETextLineEndings txtending            /*!< Text reports only: CR LF CRLF */
          , enum PTB_ETextEncoding    txtencode            /*!< Text reports only: UTF8 UTF 16,... */
          , PTB_uint32_t              parts                /*!< Which parts (any combination of enum PTB_EResultParts) */
          , enum PTB_EResultDetails   details              /*!< Which amount of details to include in the reports */
          , const PTB_Path_t*         externalResource     /*!< This parameter can be used for additional parameters, might be NULL
			                                                      HTML reports only : If used links are being inserted into HTML reports, but referenced 
			                                                                         objects are not exported. This option requires a path (URL) to a folder
			                                                                         where all standard references reside 
			                                                      XSLT reports only : Full file path to the xslt file that be applied to the xml report 
																  CCHIP reports only: Full file path to the html template 
															*/
          , PTB_Preflight_ProgressCB  cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
          , void*                     cbProgressUserData   /*!< User data submitted to progress callback function */
          , PTB_uint32_t              firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
          , PTB_uint32_t              lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
          , void*                     reserved             /*!< currently unused for future used */
          );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_Report2( PTB_ResultID              idResult             /*!< Result ID as returned by PTB_Preflight(), PTB_CheckPDFA() or PTB_ConvertPDFA() */
           , const PTB_Path_t*         dest                 /*!< Full file path for the report file */
           , enum PTB_EReportType      type                 /*!< Text, TextSummary, XML, PDFBreakout, PDFMask, ...*/
           , enum PTB_ETextLineEndings txtending            /*!< Text reports only: CR LF CRLF */
           , enum PTB_ETextEncoding    txtencode            /*!< Text reports only: UTF8 UTF 16,... */
           , PTB_uint32_t              parts                /*!< Which parts (any combination of enum PTB_EResultParts) */
           , enum PTB_EResultDetails   details              /*!< Which amount of details to include in the reports */
           , const PTB_Path_t*         externalResource     /*!< This parameter can be used for additional parameters, might be NULL
			                                                      HTML reports only : If used links are being inserted into HTML reports, but referenced 
			                                                                          objects are not exported. This option requires a path (URL) to a folder
			                                                                          where all standard references reside 
			                                                      XSLT reports only : Full file path to the xslt file that be applied to the xml report 
																  CCHIP reports only: Full file path to the html template 
															*/
           , PTB_uint32_t              downsampling         /*!< Downsample images in PDF reports to <PPI>  */
           , PTB_Preflight_ProgressCB  cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
           , void*                     cbProgressUserData   /*!< User data submitted to progress callback function */
           , PTB_uint32_t              firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
           , PTB_uint32_t              lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
           , void*                     reserved             /*!< currently unused for future used */
           );

/*!
** \brief  Create a XMLV2 report for the preflight results with extended parameter to adjust the content
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ReportXML( PTB_ResultID              idResult             /*!< Result ID as returned by PTB_Preflight(), PTB_CheckPDFA() or PTB_ConvertPDFA() */
             , const PTB_Path_t*         dest                 /*!< Full file path for the report file */
             , PTB_uint32_t              maxNumPages          /*!< Include first <NUM> pages; might be PTB_MAX_REPORT_NUM for all pages */ 
             , PTB_uint32_t              maxNumImages         /*!< Include first <NUM> images; might be PTB_MAX_REPORT_NUM for all images */ 
             , PTB_uint32_t              maxNumFonts          /*!< Include first <NUM> fonts; might be PTB_MAX_REPORT_NUM for all fonts */ 
             , PTB_uint32_t              maxNumColorSpaces    /*!< Include first <NUM> color spaces; might be PTB_MAX_REPORT_NUM for all olor spaces */ 
             , PTB_uint32_t              maxNumSmoothShades   /*!< Include first <NUM> smooth shades; might be PTB_MAX_REPORT_NUM for all smooth shades */ 
             , PTB_uint32_t              maxNumPatterns       /*!< Include first <NUM> patterns; might be PTB_MAX_REPORT_NUM for all patterns */ 
             , PTB_uint32_t              maxNumFormXObjects   /*!< Include first <NUM> form xobjects; might be PTB_MAX_REPORT_NUM for all form xobjects */ 
             , PTB_Preflight_ProgressCB  cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
             , void*                     cbProgressUserData   /*!< User data submitted to progress callback function */
             , PTB_uint32_t              firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
             , PTB_uint32_t              lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
             , void*                     reserved             /*!< currently unused for future used */
             );

/*!
** \brief  Flags to control some aspects during xml creation
*/
enum PTB_EReportXMLFlags { PTB_erxfNone             = 0x00  /*!< None */
                         , PTB_erxfInkCoverage      = 0x01  /*!< Include ink coverage for every page */
                         };

/*!
** \brief  Ignore Objects outside page box
*/
enum PTB_EReportXMLPageBox { PTB_erxpbCropBox            /*!< CropBox  */
                           , PTB_erxpbTrimBox            /*!< TrimBox  */
                           , PTB_erxpbBleedBox           /*!< BleedBox */
                           , PTB_erxpbMediaBox           /*!< MediaBox */
                           , PTB_erxpbArtBox             /*!< ArtBox   */
                           };
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ReportXML2( PTB_ResultID               idResult             /*!< Result ID as returned by PTB_Preflight(), PTB_CheckPDFA() or PTB_ConvertPDFA() */
              , const PTB_Path_t*          dest                 /*!< Full file path for the report file */
              , PTB_uint32_t               maxNumPages          /*!< Include first <NUM> pages; might be PTB_MAX_REPORT_NUM for all pages */ 
              , PTB_uint32_t               maxNumImages         /*!< Include first <NUM> images; might be PTB_MAX_REPORT_NUM for all images */ 
              , PTB_uint32_t               maxNumFonts          /*!< Include first <NUM> fonts; might be PTB_MAX_REPORT_NUM for all fonts */ 
              , PTB_uint32_t               maxNumColorSpaces    /*!< Include first <NUM> color spaces; might be PTB_MAX_REPORT_NUM for all olor spaces */ 
              , PTB_uint32_t               maxNumSmoothShades   /*!< Include first <NUM> smooth shades; might be PTB_MAX_REPORT_NUM for all smooth shades */ 
              , PTB_uint32_t               maxNumPatterns       /*!< Include first <NUM> patterns; might be PTB_MAX_REPORT_NUM for all patterns */ 
              , PTB_uint32_t               maxNumFormXObjects   /*!< Include first <NUM> form xobjects; might be PTB_MAX_REPORT_NUM for all form xobjects */ 
              , PTB_uint32_t               inkCovResolution     /*!< Resolution for ink coverage calculation in PPI - only applied if PTB_erxInkCoverage is set */ 
              , enum PTB_EReportXMLPageBox inkCovBox            /*!< Ignore Objects outside page box - only applied if PTB_erxInkCoverage is set */
              , PTB_uint32_t               flags                /*!< Flags (any combination of PTB_EReportXMLFlags values) */ 
              , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
              , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
              , PTB_uint32_t               firstPage            /*!< First page to be preflighted, this is a zero based index, i.e. first page has value of 0 (zero) */
              , PTB_uint32_t               lastPage             /*!< Last page to be preflighted; might be PTB_MAX_END_PAGE for all pages */
              , void*                      reserved             /*!< currently unused for future used */
              );

/******************************************************************************/
// Hit 

enum PTB_EHitType { PTB_ehtUnknown        /*!<  */
                  , PTB_ehtDocument       /*!<  */
                  , PTB_ehtPage           /*!<  */
                  // Unknown content stream objects
                  , PTB_ehtInvalidCmd     /*!<  */
                  , PTB_ehtXObj           /*!<  */
                  // Content stream objects
                  , PTB_ehtVectorFill     /*!<  */
                  , PTB_ehtVectorStroke   /*!<  */
                  , PTB_ehtTextFill       /*!<  */
                  , PTB_ehtTextOutline    /*!<  */
                  , PTB_ehtTextInvisible  /*!<  */
                  , PTB_ehtInlineImage    /*!<  */
                  , PTB_ehtImage          /*!<  */
                  , PTB_ehtFormXObj       /*!<  */
                  , PTB_ehtPostScript     /*!<  */
                  , PTB_ehtShade          /*!<  */
                  };

enum PTB_EHitDataType { PTB_ehdtType             /*!< intData: see PTB_EHitType */
                      , PTB_ehdtPageNumber       /*!< intData: Page number      */
                      , PTB_ehdtTriggerValue     /*!< strData: Trigger value, more than one value will be seperated by TAB */
                      , PTB_ehdtBoundingBox      /*!< floatData (Length: 4): viewer coordinates [llx, lly, urx, ury] */
                      , PTB_ehdtNumPathCommands  /*!< intData: number of stroke or fill path commands */
                      , PTB_ehdtNumPathParams    /*!< intData: number of stroke or fill path parameters */
                      , PTB_ehdtGetPathCommands  /*!< intData: array of PTB_int32_t, dataLength: length of array*/
                      , PTB_ehdtGetPathParams    /*!< floatData: array of PTB_float_t , dataLength: length of array */
                      , PTB_ehdtNumClipCommands  /*!< intData: number of clip commands */
                      , PTB_ehdtNumClipParams    /*!< intData: number of clip parameters */
                      , PTB_ehdtGetClipCommands  /*!< intData: array of PTB_int32_t, dataLength: length of array*/
                      , PTB_ehdtGetClipParams    /*!< floatData: array of PTB_float_t , dataLength: length of array */
                      };

enum PTB_EPathCommand { PTB_epcStroke                 /* 0 parameters, followed by n commands */
                      , PTB_epcFillEvenOdd            /* 0 parameters, followed by n commands */
                      , PTB_epcFillNonZeroWindingRule /* 0 parameters, followed by n commands */
                      , PTB_epcClipEvenOdd            /* 0 parameters, followed by n commands */
                      , PTB_epcClipNonZeroWindingRule /* 0 parameters, followed by n commands */
                      , PTB_epcMoveTo = 10            /* 2 parameters */
                      , PTB_epcLineTo                 /* 2 parameters */
                      , PTB_epcCurveTo                /* 6 parameters */
                      , PTB_epcRect                   /* 4 parameters */
                      , PTB_epcClose                  /* 0 parameters */
                      };

/*! 
** \brief  Get hit data
** 
**         Get the hit data
** \note   Possible values for \a ehdt are: 
**         PTB_ehdtType
**          - intData must point to an int (represent \a PTB_EHitType)
**          - floatData must be NULL
**          - idStr must be NULL
**         PTB_ehtPageNumber
**          - intData must point to an int
**          - floatData must be NULL
**          - idStr must be NULL
**         PTB_ehtTriggerValue
**          - intData must be NULL
**          - floatData must be NULL
**          - idStr must point to a string ID for the function to write the result to
**         PTB_ehtBoundingBox
**          - intData must be NULL
**          - floatData must point to a float array (dataLength <= 4)
**          - dataLength must be 4
**          - idStr must be NULL
**         PTB_ehdtNumPathCommands
**          - intData must point to an int
**          - floatData must be NULL
**          - dataLength must be NULL
**          - idStr must be NULL
**         PTB_ehdtNumPathParams
**          - intData must point to an int
**          - floatData must be NULL
**          - dataLength must be NULL
**          - idStr must be NULL
**         PTB_ehdtGetPathCommands
**          - intData must point to an int array (dataLength <= PTB_ehdtNumPathCommands)
**          - floatData must be NULL
**          - dataLength must be the result of PTB_ehdtNumPathCommands
**          - idStr must be NULL
**         PTB_ehdtGetPathParams
**          - intData must be NULL
**          - floatData must be to an float array (dataLength <= PTB_ehdtNumPathParams)
**          - dataLength must be the result of PTB_ehdtNumPathParams
**          - idStr must be NULL
**         PTB_ehdtNumClipCommands
**          - intData must point to an int
**          - floatData must be NULL
**          - dataLength must be NULL
**          - idStr must be NULL
**         PTB_ehdtNumClipParams
**          - intData must point to an int
**          - floatData must be NULL
**          - dataLength must be NULL
**          - idStr must be NULL
**         PTB_ehdtGetClipCommands
**          - intData must point to an int array (dataLength <= PTB_ehdtNumClipCommands)
**          - floatData must be NULL
**          - dataLength must be the result of PTB_ehdtNumClipCommands
**          - idStr must be NULL
**         PTB_ehdtGetClipParams
**          - intData must be NULL
**          - floatData must be to an float array (dataLength <= PTB_ehdtNumClipParams)
**          - dataLength must be the result of PTB_ehdtNumClipParams
**          - idStr must be NULL
**
** \retval PTB_eerrNone                     No error                          
** \retval PTB_eerrParamFailure             A parameter is unknown or incorrect
** \retval PTB_eerrResultBufferTooSmall     Buffer too small (If so, then \a dataLength will contain the needed buffer size)
** 
*/
PTB_FUNC_PROTO 
PTB_EError 
PTB_ResultGetHitData( PTB_ResultID             idResult    /*!< ID of Result to get Hit data from  */
                    , PTB_HitID                idHit       /*!< ID of Hit to get data from         */
                    , enum PTB_EHitDataType    ehdt        /*!< Data type to retrieve              */
                    , PTB_int32_t*             intData     /*!< Pointer to integer data            */
                    , PTB_float_t*             floatData   /*!< Pointer to float data              */
                    , PTB_int32_t*             dataLength  /*!< Buffer size; used for array values - can be NULL for simple data type */
                    , PTB_StringID*            idStr       /*!< String data                        */
                    );

/******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif // PTB_PREFLIGHT_H

/******************************************************************************/
/* EOF */
