/*!
**
** \file ptbPDFA.h
**
** \brief callas pdfEngine SDK: PDF/A action
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_PDFA_H)
/*! \def PTB_PDFA_H header define */
#define PTB_PDFA_H

/******************************************************************************/

#include "ptbTypes.h"
#include "ptbProgress.h"
#include "ptbPreflight.h"

/******************************************************************************/

#include "ptbProlog.h"

/******************************************************************************/

/*******************************************************************************
** PDF/A
*/

/*!
** \brief  Runs PDF/A check
**         Runs a PDF/A for a given document with a certain 
**         PDF/A version.
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
** \retval PTB_eerrPreflightEncrypted        The PDF file is encrypted (processing of encrypted files in pdfaPilot lib not supported)
** \retval PTB_eerrPreflightContStrmFailure  An error occurred while parsing the contents stream (unable to analyse the PDF file)
** \retval PTB_eerrCancel                    The caller has canceled the process.
** \retval PTB_eerrPreflightPDFACreateFail   Unable to convert the PDF file to PDF/A (internal error/unknown exception)
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_CheckPDFA( const PTB_Path_t*          doc                  /*!< Document to check for PDF/A */
             , enum PTB_EPDFAVersion      version              /*!< PDF/A version to check for */
             , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
             , void*                      cbHitUserData        /*!< User data submitted to hit callback function */
             , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
             , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
             , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
             , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
             , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check, must not be NULL */
             , void*                      reserved             /*!< currently unused for future used */
             );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_CheckPDFA2( const PTB_Path_t*          doc                  /*!< Document to check for PDF/A */
              , enum PTB_EPDFAVersion      version              /*!< PDF/A version to check for */
              , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
              , void*                      cbHitUserData        /*!< User data submitted to hit callback function */
              , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
              , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
              , PTB_uint32_t               hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
              , PTB_uint32_t               hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
              , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
              , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
              , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check, must not be NULL */
              , void*                      reserved             /*!< currently unused for future used */
              );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_CheckPDFA3( const PTB_Path_t*          doc                  /*!< Document to check for PDF/A */
              , enum PTB_EPDFAVersion      version              /*!< PDF/A version to check for */
              , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
              , void*                      cbHitUserData        /*!< User data submitted to hit callback function */
              , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
              , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
              , PTB_uint32_t               hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
              , PTB_uint32_t               hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
			  , PTB_uint32_t               maxpages             /*!< Maximum number of pages - after this limit only pages that have problems will be stored, might be PTB_MAX_PAGE_NUM */
			  , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
              , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
              , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check, must not be NULL */
              , void*                      reserved             /*!< currently unused for future used */
              );

/*!
** \brief  Flags to control some aspects during PDF/A conversion
*/
enum PTB_EConvertPDFAFlags { PTB_ecpfNone                  = 0x00  /*!< None */
                           , PTB_ecpfNoOptimization        = 0x01  /*!< Deactivates optimization of the internal structure when saving th PDF */
						   , PTB_ecpfNoXMPMetadataRemoval  = 0x02  /*!< Deactivates removal of XMP Metadata that isnot compliant with PDF/A */
                           , PTB_ecpfNoFlattenTransparency = 0x04  /*!< Deactivates transparency flattening (applied only on PDF/A-1a and PDF/A-1b) */

                           , PTB_ecpfForceConvRedistill    = 0x08  /*!< Re-convert via PostScript if regular conversion fails */
                           , PTB_ecpfForceConvPageToImage  = 0x10  /*!< Convert pages with problems into images if regular conversion fails */
                           , PTB_ecpfForceConvDocToImage   = 0x20  /*!< Convert all pages into images if regular conversion fails */
                           };

/*!
** \brief  Convert a file to PDF/A
**
** \note   The results ID obtained by this call must be released using the 
**         PTB_PreflightRelease() function.
**
** \note   All progress information is formatted in current language
**
** general errors
** \retval PTB_eerrNone                          No error
** \retval PTB_eerrUnknown                       Unknown error
** \retval PTB_eerrInternal                      Internal error
** \retval PTB_eerrMemoryErr                     Not enough memory
** \retval PTB_eerrParamFailure                  A parameter is unknown or incorrect
** \retval PTB_eerrIsBusy                        The preflight engine is busy
** \retval PTB_eerrPreflightOfficeError          Error during office conversion
** file name errors
** \retval PTB_eerrFileNameInvalid               Invalid file name
** \retval PTB_eerrDestVolumeExist               Destination volume does not exist
** \retval PTB_eerrDestFolderExist               Destination folder does not exist
** \retval PTB_eerrDestFolderCreate              Cannot create destination folder
** \retval PTB_eerrDestWrite                     No write access to destination
** initialization errors
** \retval PTB_eerrNotInitialized                Lib not initialized
** PRC errors
** \retval PTB_eerrInvalidEngineID               PRC Engine ID is invalid
** \retval PTB_eerrInvalidProfID                 Profile ID is invalid
** Preflight errors
** \retval PTB_eerrPreflightNoPDFFile            The file is not a PDF file (unable to locate the root object)
** \retval PTB_eerrPreflightPDFHasNoPages        Unable to find the specified page
** \retval PTB_eerrPreflightOpenFileErr          The PDF file is corrupt (it could not be opened)
** \retval PTB_eerrPreflightEncrypted            The PDF file is encrypted (processing of encrypted files in pdfaPilot lib not supported)
** \retval PTB_eerrPreflightContStrmFailure      An error occurred while parsing the contents stream (unable to analyse the PDF file)
** \retval PTB_eerrCancel                        The caller has canceled the process
** \retval PTB_eerrPreflightPDFACreateFail       Unable to convert the PDF file to PDF/A (internal error/unknown exception)
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertPDFA( const PTB_Path_t*          doc                  /*!< Document to convert to PDF/A */
               , enum PTB_EPDFAVersion      version              /*!< PDF/A version to convert to */
               , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
               , void*                      cbHitUserData        /*!< user data submitted to hit callback function */
               , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
               , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
               , PTB_SaveCB                 cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
               , void*                      cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
               , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID*/
               , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID*/
               , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check , must not be NULL */
               , void*                      reserved             /*!< currently unused for future used */
              );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertPDFA2( const PTB_Path_t*          doc                  /*!< Document to convert to PDF/A */
                , enum PTB_EPDFAVersion      version              /*!< PDF/A version to convert to */
                , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
                , void*                      cbHitUserData        /*!< user data submitted to hit callback function */
                , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
                , PTB_SaveCB                 cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
                , void*                      cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
                , const PTB_Path_t*          fontfolder           /*!< Search path for font files, if NULL, only system font folder are used */
                , const PTB_Path_t*          fontcachefolder      /*!< Path for the font cache, if NULL, FontCache in user preferences is used */
                , const PTB_Path_t*          fontsubstitutionfile /*!< Full file path to the font substitution file (we recommend to use ./etc/FontSubstitution/pdfa.cfg), might be NULL */
                , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
                , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
                , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check , must not be NULL */
                , void*                      reserved             /*!< currently unused for future used */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertPDFA3( const PTB_Path_t*          doc                  /*!< Document to convert to PDF/A */
                , enum PTB_EPDFAVersion      version              /*!< PDF/A version to convert to */
                , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
                , void*                      cbHitUserData        /*!< user data submitted to hit callback function */
                , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
                , PTB_SaveCB                 cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
                , void*                      cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
                , const PTB_Path_t*          fontfolder           /*!< Search path for font files, if NULL, only system font folder are used */
                , const PTB_Path_t*          fontcachefolder      /*!< Path for the font cache, if NULL, FontCache in user preferences is used */
                , const PTB_Path_t*          fontsubstitutionfile /*!< Full file path to the font substitution file (we recommend to use ./etc/FontSubstitution/pdfa.cfg), might be NULL */
                , const PTB_Path_t*          outputintentfile     /*!< Full file path to a PDF file with an OutputIntent; forces use of this OutputIntent, might be NULL */
                , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
                , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
                , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check , must not be NULL */
                , void*                      reserved             /*!< currently unused for future used */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertPDFA4( const PTB_Path_t*          doc                  /*!< Document to convert to PDF/A */
                , enum PTB_EPDFAVersion      version              /*!< PDF/A version to convert to */
                , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
                , void*                      cbHitUserData        /*!< user data submitted to hit callback function */
                , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
                , PTB_SaveCB                 cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
                , void*                      cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
                , const PTB_Path_t*          fontfolder           /*!< Search path for font files, if NULL, only system font folder are used */
                , const PTB_Path_t*          fontcachefolder      /*!< Path for the font cache, if NULL, FontCache in user preferences is used */
                , const PTB_Path_t*          fontsubstitutionfile /*!< Full file path to the font substitution file (we recommend to use ./etc/FontSubstitution/pdfa.cfg), might be NULL */
                , const PTB_Path_t*          outputintentfile     /*!< Full file path to a PDF file with an OutputIntent; forces use of this OutputIntent, might be NULL */
                , PTB_bool_t                 removexmpmetadata    /*!< Remove XMP Metadata that prevents a PDF from being PDF/A compliant */
                , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
                , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
                , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check , must not be NULL */
                , void*                      reserved             /*!< currently unused for future used */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertPDFA5( const PTB_Path_t*          doc                  /*!< Document to convert to PDF/A */
                , enum PTB_EPDFAVersion      version              /*!< PDF/A version to convert to */
                , PTB_FixupCB                cbFixup              /*!< CB function to be called for every fixup, might be NULL */
                , void*                      cbFixupUserData      /*!< user data submitted to fixup callback function */
                , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
                , void*                      cbHitUserData        /*!< user data submitted to hit callback function */
                , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
                , PTB_SaveCB                 cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
                , void*                      cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
                , const PTB_Path_t*          fontfolder           /*!< Search path for font files, if NULL, only system font folder are used */
                , const PTB_Path_t*          fontcachefolder      /*!< Path for the font cache, if NULL, FontCache in user preferences is used */
                , const PTB_Path_t*          fontsubstitutionfile /*!< Full file path to the font substitution file (we recommend to use ./etc/FontSubstitution/pdfa.cfg), might be NULL */
                , const PTB_Path_t*          outputintentfile     /*!< Full file path to a PDF file with an OutputIntent; forces use of this OutputIntent, might be NULL */
                , PTB_bool_t                 removexmpmetadata    /*!< Remove XMP Metadata that prevents a PDF from being PDF/A compliant */
                , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
                , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
                , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check , must not be NULL */
                , void*                      reserved             /*!< currently unused for future used */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertPDFA6( const PTB_Path_t*          doc                  /*!< Document to convert to PDF/A */
                , enum PTB_EPDFAVersion      version              /*!< PDF/A version to convert to */
                , PTB_FixupCB                cbFixup              /*!< CB function to be called for every fixup, might be NULL */
                , void*                      cbFixupUserData      /*!< user data submitted to fixup callback function */
                , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
                , void*                      cbHitUserData        /*!< user data submitted to hit callback function */
                , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
                , PTB_SaveCB                 cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
                , void*                      cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
                , const PTB_Path_t*          fontfolder           /*!< Search path for font files, if NULL, only system font folder are used */
                , const PTB_Path_t*          fontcachefolder      /*!< Path for the font cache, if NULL, FontCache in user preferences is used */
                , const PTB_Path_t*          fontsubstitutionfile /*!< Full file path to the font substitution file (we recommend to use ./etc/FontSubstitution/pdfa.cfg), might be NULL */
                , const PTB_Path_t*          outputintentfile     /*!< Full file path to a PDF file with an OutputIntent; forces use of this OutputIntent, might be NULL */
                , PTB_bool_t                 removexmpmetadata    /*!< Remove XMP Metadata that prevents a PDF from being PDF/A compliant */
                , PTB_bool_t                 flattentransparency  /*!< Apply flatten transparency */
                , const PTB_Path_t*          defaultprofile_gray  /*!< Full file path to Gray ICC profile, the profile is embedded as default profile making device dependent Gray page objects device independent, might be NULL */
                , const PTB_Path_t*          defaultprofile_rgb   /*!< Full file path to RGB ICC profile, the profile is embedded as default profile making device dependent RGB page objects device independent, might be NULL */
                , const PTB_Path_t*          defaultprofile_cmyk  /*!< Full file path to CMYK ICC profile, the profile is embedded as default profile making device dependent CMYK page objects device independent, might be NULL */
                , const PTB_Path_t*          bookmarkfile         /*!< Full file path to a XML file containing bookmarks, the bookmarks are embedded in the PDF file, might be NULL */
                , const PTB_Path_t*          xmpfile              /*!< Full file path to a XMP file, the XMP metadata is merged into existing XMP metadata if present, might be NULL */
                , PTB_bool_t                 topdf_print          /*!< Images have better quality, size of result files is higher */
                , PTB_uint32_t               topdf_firstPage      /*!< First page; this is a zero based index, i.e. first page has value of 0 (zero) */
                , PTB_uint32_t               topdf_lastPage       /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
                , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
                , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check , must not be NULL */
                , void*                      reserved             /*!< currently unused for future used */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertPDFA7( const PTB_Path_t*          doc                  /*!< Document to convert to PDF/A */
                , enum PTB_EPDFAVersion      version              /*!< PDF/A version to convert to */
                , PTB_FixupCB                cbFixup              /*!< CB function to be called for every fixup, might be NULL */
                , void*                      cbFixupUserData      /*!< user data submitted to fixup callback function */
                , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
                , void*                      cbHitUserData        /*!< user data submitted to hit callback function */
                , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
                , PTB_SaveCB                 cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
                , void*                      cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
                , PTB_uint32_t               flags                /*!< Flags (any combination of PTB_EConvertPDFAFlags values)  */
                , const PTB_Path_t*          fontfolder           /*!< Search path for font files, if NULL, only system font folder are used */
                , const PTB_Path_t*          fontcachefolder      /*!< Path for the font cache, if NULL, FontCache in user preferences is used */
                , const PTB_Path_t*          fontsubstitutionfile /*!< Full file path to the font substitution file (we recommend to use ./etc/FontSubstitution/pdfa.cfg), might be NULL */
                , const PTB_Path_t*          outputintentfile     /*!< Full file path to a PDF file with an OutputIntent; forces use of this OutputIntent, might be NULL */
                , const PTB_Path_t*          defaultprofile_gray  /*!< Full file path to Gray ICC profile, the profile is embedded as default profile making device dependent Gray page objects device independent, might be NULL */
                , const PTB_Path_t*          defaultprofile_rgb   /*!< Full file path to RGB ICC profile, the profile is embedded as default profile making device dependent RGB page objects device independent, might be NULL */
                , const PTB_Path_t*          defaultprofile_cmyk  /*!< Full file path to CMYK ICC profile, the profile is embedded as default profile making device dependent CMYK page objects device independent, might be NULL */
                , const PTB_Path_t*          bookmarkfile         /*!< Full file path to a XML file containing bookmarks, the bookmarks are embedded in the PDF file, might be NULL */
                , const PTB_Path_t*          xmpfile              /*!< Full file path to a XMP file, the XMP metadata is merged into existing XMP metadata if present, might be NULL */
                , PTB_bool_t                 topdf_print          /*!< Images have better quality, size of result files is higher */
                , PTB_uint32_t               topdf_firstPage      /*!< First page; this is a zero based index, i.e. first page has value of 0 (zero) */
                , PTB_uint32_t               topdf_lastPage       /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
                , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
                , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check , must not be NULL */
                , void*                      reserved             /*!< currently unused for future used */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertPDFA8( const PTB_Path_t*          doc                  /*!< Document to convert to PDF/A */
                , enum PTB_EPDFAVersion      version              /*!< PDF/A version to convert to */
                , PTB_FixupCB                cbFixup              /*!< CB function to be called for every fixup, might be NULL */
                , void*                      cbFixupUserData      /*!< user data submitted to fixup callback function */
                , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
                , void*                      cbHitUserData        /*!< user data submitted to hit callback function */
                , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
                , PTB_SaveCB                 cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
                , void*                      cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
                , PTB_uint32_t               flags                /*!< Flags (any combination of PTB_EConvertPDFAFlags values)  */
                , const PTB_Path_t*          fontfolder           /*!< Search path for font files, if NULL, only system font folder are used */
                , const PTB_Path_t*          fontcachefolder      /*!< Path for the font cache, if NULL, FontCache in user preferences is used */
                , const PTB_Path_t*          fontsubstitutionfile /*!< Full file path to the font substitution file (we recommend to use ./etc/FontSubstitution/pdfa.cfg), might be NULL */
                , const PTB_Path_t*          outputintentfile     /*!< Full file path to a PDF file with an OutputIntent; forces use of this OutputIntent, might be NULL */
                , const PTB_Path_t*          defaultprofile_gray  /*!< Full file path to Gray ICC profile, the profile is embedded as default profile making device dependent Gray page objects device independent, might be NULL */
                , const PTB_Path_t*          defaultprofile_rgb   /*!< Full file path to RGB ICC profile, the profile is embedded as default profile making device dependent RGB page objects device independent, might be NULL */
                , const PTB_Path_t*          defaultprofile_cmyk  /*!< Full file path to CMYK ICC profile, the profile is embedded as default profile making device dependent CMYK page objects device independent, might be NULL */
                , const PTB_Path_t*          bookmarkfile         /*!< Full file path to a XML file containing bookmarks, the bookmarks are embedded in the PDF file, might be NULL */
                , const PTB_Path_t*          xmpfile              /*!< Full file path to a XMP file, the XMP metadata is merged into existing XMP metadata if present, might be NULL */
                , PTB_uint32_t               forceconv_resolution /*!< Image resolution in ppi for the force conversion steps */
                , PTB_bool_t                 topdf_print          /*!< Images have better quality, size of result files is higher */
                , PTB_uint32_t               topdf_firstPage      /*!< First page; this is a zero based index, i.e. first page has value of 0 (zero) */
                , PTB_uint32_t               topdf_lastPage       /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
                , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
                , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check , must not be NULL */
                , void*                      reserved             /*!< currently unused for future used */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertPDFA9( const PTB_Path_t*          doc                  /*!< Document to convert to PDF/A */
                , enum PTB_EPDFAVersion      version              /*!< PDF/A version to convert to */
                , PTB_FixupCB                cbFixup              /*!< CB function to be called for every fixup, might be NULL */
                , void*                      cbFixupUserData      /*!< user data submitted to fixup callback function */
                , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
                , void*                      cbHitUserData        /*!< user data submitted to hit callback function */
                , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
                , PTB_SaveCB                 cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
                , void*                      cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
                , PTB_uint32_t               flags                /*!< Flags (any combination of PTB_EConvertPDFAFlags values)  */
                , const PTB_Path_t*          fontfolder           /*!< Search path for font files, if NULL, only system font folder are used */
                , const PTB_Path_t*          fontcachefolder      /*!< Path for the font cache, if NULL, FontCache in user preferences is used */
                , const PTB_Path_t*          fontsubstitutionfile /*!< Full file path to the font substitution file (we recommend to use ./etc/FontSubstitution/pdfa.cfg), might be NULL */
                , const PTB_Path_t*          outputintentfile     /*!< Full file path to a PDF file with an OutputIntent; forces use of this OutputIntent, might be NULL */
                , const PTB_Path_t*          defaultprofile_gray  /*!< Full file path to Gray ICC profile, the profile is embedded as default profile making device dependent Gray page objects device independent, might be NULL */
                , const PTB_Path_t*          defaultprofile_rgb   /*!< Full file path to RGB ICC profile, the profile is embedded as default profile making device dependent RGB page objects device independent, might be NULL */
                , const PTB_Path_t*          defaultprofile_cmyk  /*!< Full file path to CMYK ICC profile, the profile is embedded as default profile making device dependent CMYK page objects device independent, might be NULL */
                , const PTB_Path_t*          bookmarkfile         /*!< Full file path to a XML file containing bookmarks, the bookmarks are embedded in the PDF file, might be NULL */
                , const PTB_Path_t*          xmpfile              /*!< Full file path to a XMP file, the XMP metadata is merged into existing XMP metadata if present, might be NULL */
                , PTB_uint32_t               forceconv_resolution /*!< Image resolution in ppi for the force conversion steps */
                , PTB_uint32_t               hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
                , PTB_uint32_t               hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
                , PTB_bool_t                 topdf_print          /*!< Images have better quality, size of result files is higher */
                , PTB_uint32_t               topdf_firstPage      /*!< First page; this is a zero based index, i.e. first page has value of 0 (zero) */
                , PTB_uint32_t               topdf_lastPage       /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
                , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
                , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check , must not be NULL */
                , void*                      reserved             /*!< currently unused for future used */
                );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ConvertPDFA10( const PTB_Path_t*          doc                  /*!< Document to convert to PDF/A */
                 , enum PTB_EPDFAVersion      version              /*!< PDF/A version to convert to */
                 , PTB_FixupCB                cbFixup              /*!< CB function to be called for every fixup, might be NULL */
                 , void*                      cbFixupUserData      /*!< user data submitted to fixup callback function */
                 , PTB_HitCB                  cbHit                /*!< CB function to be called for every hit, might be NULL */
                 , void*                      cbHitUserData        /*!< user data submitted to hit callback function */
                 , PTB_Preflight_ProgressCB   cbProgress           /*!< Progress CB function to be called during processing, might be NULL */
                 , void*                      cbProgressUserData   /*!< User data submitted to progress callback function */
                 , PTB_SaveCB                 cbSave               /*!< CB function to be called immediately before and after fixup process, if NULL the corrections will be applied to the original 'doc' */
                 , void*                      cbSaveUserData       /*!< User data submitted to PTB_SaveCB callback function */
                 , PTB_uint32_t               flags                /*!< Flags (any combination of PTB_EConvertPDFAFlags values)  */
                 , const PTB_Path_t*          fontfolder           /*!< Search path for font files, if NULL, only system font folder are used */
                 , const PTB_Path_t*          fontcachefolder      /*!< Path for the font cache, if NULL, FontCache in user preferences is used */
                 , const PTB_Path_t*          fontsubstitutionfile /*!< Full file path to the font substitution file (we recommend to use ./etc/FontSubstitution/pdfa.cfg), might be NULL */
                 , const PTB_Path_t*          outputintentfile     /*!< Full file path to a PDF file with an OutputIntent; forces use of this OutputIntent, might be NULL */
                 , const PTB_Path_t*          defaultprofile_gray  /*!< Full file path to Gray ICC profile, the profile is embedded as default profile making device dependent Gray page objects device independent, might be NULL */
                 , const PTB_Path_t*          defaultprofile_rgb   /*!< Full file path to RGB ICC profile, the profile is embedded as default profile making device dependent RGB page objects device independent, might be NULL */
                 , const PTB_Path_t*          defaultprofile_cmyk  /*!< Full file path to CMYK ICC profile, the profile is embedded as default profile making device dependent CMYK page objects device independent, might be NULL */
                 , const PTB_Path_t*          bookmarkfile         /*!< Full file path to a XML file containing bookmarks, the bookmarks are embedded in the PDF file, might be NULL */
                 , const PTB_Path_t*          xmpfile              /*!< Full file path to a XMP file, the XMP metadata is merged into existing XMP metadata if present, might be NULL */
                 , PTB_uint32_t               forceconv_resolution /*!< Image resolution in ppi for the force conversion steps */
                 , PTB_uint32_t               hitsperpage          /*!< Maximum number of hits in reports per page, might be PTB_MAX_HIT_NUM */
                 , PTB_uint32_t               hitsperdoc           /*!< Maximum number of hits in reports per document, might be PTB_MAX_HIT_NUM */
				 , PTB_uint32_t               maxpages             /*!< Maximum number of pages - after this limit only pages that have problems will be stored, might be PTB_MAX_PAGE_NUM */
				 , PTB_bool_t                 topdf_print          /*!< Images have better quality, size of result files is higher */
                 , PTB_uint32_t               topdf_firstPage      /*!< First page; this is a zero based index, i.e. first page has value of 0 (zero) */
                 , PTB_uint32_t               topdf_lastPage       /*!< Last page; might be PTB_MAX_END_PAGE for all pages */
                 , PTB_PRCEngineID            idEng                /*!< ID of PRC Engine that holds custom profile, can be PTB_INVALID_ID */
                 , PTB_PRCProfID              idProf               /*!< ID of custom profile, can be PTB_INVALID_ID */
                 , PTB_ResultID*              idResult             /*!< On return ID for results from this preflight check , must not be NULL */
                 , void*                      reserved             /*!< currently unused for future used */
                 );

/******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif // PTB_PDFA_H

/******************************************************************************/
/* EOF */
