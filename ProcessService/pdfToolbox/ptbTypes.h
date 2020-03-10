/*!
**
** \file ptbTypes.h
**
** \brief callas pdfEngine SDK: Type definitions
** 
** This defines a types used throughout the callas pdfEngine SDK. 
** 
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_TYPES_H)
/*! \def PTB_TYPES_H header define */
#define PTB_TYPES_H

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** IDs
*/

/*! Declares an ID type */
#define PTB_DECLARE_ID(pfx,id) struct CALS_##id##_Tag; \
                               typedef struct CALS_##id##_Tag* pfx##_##id

/*! Invalid ID value */
#if defined(__cplusplus)
#	define PTB_INVALID_ID       0
#else /*defined(__cplusplus)*/
#	define PTB_INVALID_ID       ((void*)0)
#endif /*defined(__cplusplus)*/

/*******************************************************************************/
/*! 
** These are the current callas pdfEngine SDK library version numbers
*/
#define dPTBSDK_pdfEngineVersion                       10
#define dPTBSDK_APIMainVersion                          2
#define dPTBSDK_APIVersionIteration                     0

/*******************************************************************************
** Basic types
*/

typedef int                   PTB_bool_t;       /*!< Boolean type          */

typedef short                 PTB_int16_t;      /*!< 16 bit signed int     */
typedef unsigned short        PTB_uint16_t;     /*!< 16 bit unsigned int   */

typedef int                   PTB_int32_t;      /*!< 32 bit signed int     */
typedef unsigned int          PTB_uint32_t;     /*!< 32 bit unsigned int   */

typedef long long             PTB_int64_t;      /*!< 64 bit signed int     */
typedef unsigned long long    PTB_uint64_t;     /*!< 64 bit unsigned int   */

typedef double                PTB_float_t;      /*!< floating point type   */

typedef PTB_uint32_t          PTB_size_t;       /*!< size type             */

typedef char                  PTB_sys_char_t;   /*!< system character type */
typedef unsigned char         PTB_utf8_char_t;  /*!< UTF-8 character type  */
typedef PTB_uint16_t          PTB_utf16_char_t; /*!< UTF-16 character type */

typedef PTB_utf8_char_t       PTB_Path_t;       /*!< File path type        */

/*******************************************************************************/
/*!
** \brief  The largest possible end page number is defined here
*/
#define PTB_MAX_END_PAGE                            0xFFFFFFFF
#define PTB_DOCUMENT_HIT                            0xFFFFFFFF
#define PTB_MAX_REPORT_NUM                          0xFFFFFFFF
#define PTB_MAX_HIT_NUM                             0xFFFFFFFF
#define PTB_MAX_PAGE_NUM                            1000        // ** \note (DEPRECATED) replaced by PTB_MAX_PAGE_DEFAULT_NUM
#define PTB_MAX_PAGE_DEFAULT_NUM                    1000

/*******************************************************************************
** IDs
*/
/* General IDs */
PTB_DECLARE_ID(PTB,StringID);                /*!< String ID type          */
PTB_DECLARE_ID(PTB,LoggingID);               /*!< Logging ID type         */
PTB_DECLARE_ID(PTB,OutIntID);                /*!< OutputIntent ID type    */

/* PRC IDs */
PTB_DECLARE_ID(PTB,PRCMetaDataID);           /*!< Meta data ID type       */
PTB_DECLARE_ID(PTB,PRCEngineID);             /*!< Engine ID type          */
PTB_DECLARE_ID(PTB,PRCCondID);               /*!< Condition ID type       */
PTB_DECLARE_ID(PTB,PRCRuleID);               /*!< Rule ID type            */
PTB_DECLARE_ID(PTB,PRCRSetID);               /*!< RuleSet ID type         */
PTB_DECLARE_ID(PTB,PRCFxupID);               /*!< Fixup ID type           */
PTB_DECLARE_ID(PTB,PRCFSetID);               /*!< FixupSet ID type        */
PTB_DECLARE_ID(PTB,PRCActnID);               /*!< Action ID type          */
PTB_DECLARE_ID(PTB,PRCProfID);               /*!< Profile ID type         */
PTB_DECLARE_ID(PTB,PRCMPrfID);               /*!< MetaProfile ID type     */
PTB_DECLARE_ID(PTB,PRCPGrpID);               /*!< ProfGroup ID type       */
PTB_DECLARE_ID(PTB,PRCDynParamID);           /*!< Dyn Param ID type       */

/* Preflighting IDs */
PTB_DECLARE_ID(PTB,HitID);                   /*!< Hit ID type             */
PTB_DECLARE_ID(PTB,ResultID);                /*!< Result ID type          */

/*******************************************************************************
** Error codes
*/

//! callas pdfEngine SDK library error codes
enum PTB_EError
{ PTB_eerrNone                     = 0x0000 /*!< No error                                               */

  /* general errors                                                                                     */
, PTB_eerr_                        = 0x1000 /*!< don't use                                              */
, PTB_eerrUnknown                           /*!< Unknown error                                          */
, PTB_eerrInternal                          /*!< Internal error                                         */
, PTB_eerrUnknownErrCode                    /*!< Error code not known                                   */
, PTB_eerrMemoryErr                         /*!< Not enough memory                                      */
, PTB_eerrDataSize                          /*!< A struct's size member is too small                    */
, PTB_eerrParamFailure                      /*!< A parameter is unknown or incorrect                    */
, PTB_eerrIsBusy                            /*!< The preflight machine is busy                          */
, PTB_eerrCancel                            /*!< The process was canceled                               */
, PTB_eerrKeyCodeFail                       /*!< The keycode is incorrect                               */
, PTB_eerrKeyCodeExpire                     /*!< The keycode has expired                                */
, PTB_eerrNotSupported                      /*!< The operation is not supported                         */
, PTB_eerrSystemOverload                    /*!< A system overload condition has been detected          */
, PTB_eerrProcessCommunication              /*!< A process communication error has been detected        */
, PTB_eerrInvalidID                         /*!< Invalid  ID                                            */

  /* string errors                                                                                      */
, PTB_eerrStr_                     = 0x1800 /*!< don't use                                              */
, PTB_eerrInvalidStringID                   /*!< Invalid string ID                                      */
, PTB_eerrStrBufferTooSmall                 /*!< Too small a buffer passed                              */

  /* file name errors                                                                                   */
, PTB_eerrFile_                    = 0x2000 /*!< don't use                                              */
, PTB_eerrFileNameInvalid                   /*!< Invalid file name                                      */
, PTB_eerrFileDestVolumeExist               /*!< Dest volume does not exist                             */
, PTB_eerrFileDestFolderExist               /*!< Dest folder does not exist                             */
, PTB_eerrFileDestFolderCreate              /*!< Cannot create dest folder                              */
, PTB_eerrFileDestWrite                     /*!< Cannot write destination file                          */
, PTB_eerrFileSourceVolumeExist             /*!< Source volume does not exist                           */
, PTB_eerrFileSourceFolderExist             /*!< Source folder does not exist                           */
, PTB_eerrFileSourceFileExist               /*!< Source file does not exist                             */
, PTB_eerrFileSourceRead                    /*!< Cannot read from source                                */

  /* initialization errors                                                                              */
, PTB_eerrLib_                     = 0x2800 /*!< don't use                                              */
, PTB_eerrLibIncompatibleVersion            /*!< Lib already initialized                                */
, PTB_eerrLibAlreadyInitalized              /*!< Lib already initialized                                */
, PTB_eerrLibNotInitialized                 /*!< Lib not initialized                                    */
, PTB_eerrLibFileExist                      /*!< library path/ file does not exist.                     */

  /* logging errors                                                                                     */
, PTB_eerrLog_                     = 0x3000 /*!< don't use                                              */
, PTB_eerrLogInvalidLoggingID               /*!< Invalid logging ID                                     */
, PTB_eerrLogInvalidLevel                   /*!< Invalid logging level                                  */

  /* language errors                                                                                    */
, PTB_eerrLang_                    = 0x3800 /*!< don't use                                              */
, PTB_eerrLangInvalidFile                   /*!< Invalid language file format                           */
, PTB_eerrLangInvalidID                     /*!< Invalid language ID                                    */
, PTB_eerrLangInUse                         /*!< Language is in use                                     */
, PTB_eerrLangFileExist                     /*!< Language file does not exist                           */

  /* Output intent errors                                                                               */
, PTB_eerrOI_                      = 0x4000 /*!< don't use                                              */
, PTB_eerrOIInvalidOutIntID                 /*!< Invalid output intent ID                               */
, PTB_eerrOIInvalidDataType                 /*!< Invalid data type                                      */
, PTB_eerrOIInvalidFile                     /*!< Invalid output intent file                             */

  /* Result errors                                                                                      */
, PTB_eerrResult_                  = 0x4100 /* < don't use                                              */
, PTB_eerrResultInvalidHitID                /*!< Invalid hit ID                                         */
, PTB_eerrResultBufferTooSmall              /*!< Too small a buffer passed                              */

  /* Preflight errors                                                                                   */
, PTB_eerrPreflight_               = 0x4800 /*!< don't use                                              */
, PTB_eerrPreflightInvalidID                /*!< Invalid preflight ID                                   */
, PTB_eerrPreflightNoPDFFile                /*!< Not a PDF file (no root object)                        */
, PTB_eerrPreflightPDFHasNoPages            /*!< Page doesn't exist                                     */
, PTB_eerrPreflightOpenFileErr              /*!< PDF file corrupt                                       */
, PTB_eerrPreflightEncrypted                /*!< PDF file is encrypted                                  */
, PTB_eerrPreflightContStrmFailure          /*!< Error parsing content stream                           */
, PTB_eerrPreflightPDFX3CreateFail          /*!< Cannot convert to PDF/X                                */
, PTB_eerrPreflightPDFxCreateFail = PTB_eerrPreflightPDFX3CreateFail /*!< Cannot convert to PDF/x       */
, PTB_eerrPreflightFileError                /*!< Cannot write report file                               */
, PTB_eerrPreflightFirst                    /*!< No Preflight analysis performed                        */
, PTB_eerrPreflightTooManyProblems          /*!< To many problems found                                 */
, PTB_eerrPreflightPDFCorrupt               /*!< The document structure is corrupt                      */
, PTB_eerrPreflightReportType               /*!< The report type is unknown                             */
, PTB_eerrPreflightOINotFound               /*!< Output intent not found for PDF/X creation             */
, PTB_eerrPreflightOfficeError              /*!< Error during office conversion                         */
, PTB_eerrPreflightCertificateError         /*!< Error during Preflight Certificate processing          */
, PTB_eerrPreflightFixupFailure             /*!< Error occurred during Fixup process                    */
, PTB_eerrPreflightZUGFeRDFailure           /*!< Error occurred during ZUGFeRD process                  */
, PTB_eerrPreflightEmailFailure             /*!< Error occurred during Email process                    */
, PTB_eerrPreflightNoTaggingInfo            /*!< PDF has no tagging information                         */

  /* PRC errors                                                                                         */
, PTB_eerrPRC_                     = 0x5000 /*!< don't use                                              */
, PTB_eerrPRCNoRepository          = 0x5209 /*!< Repository does not exist                              */
, PTB_eerrPRCRepositoryIncomplete  = 0x520A /*!< Repository only partially loaded                       */
, PTB_eerrPRCRepositoryDefect      = 0x520B /*!< Repository defect, not loaded                          */
, PTB_eerrPRCInvalidEngineID       = 0x5408 /*!< Engine ID is invalid                                   */
, PTB_eerrPRCEngineOpNotSupported  = 0x5409 /*!< Operation not supported by internal engine             */
, PTB_eerrPRCEngineInUse           = 0x5420 /*!< Global PRC engine already in use (Acrobat plugin only) */
, PTB_eerrPRCLocked                = 0x5430 /*!< Object cannot be modified because it is locked         */
, PTB_eerrPRCInvalidCondID         = 0x5648 /*!< Condition ID is invalid                                */
, PTB_eerrPRCInvalidRuleID         = 0x5688 /*!< Rule ID is invalid                                     */
, PTB_eerrPRCInvalidRSetID         = 0x56C8 /*!< RuleSet ID is invalid                                  */
, PTB_eerrPRCInvalidFxupID         = 0x5708 /*!< Fixup ID is invalid                                    */
, PTB_eerrPRCInvalidFSetID         = 0x5788 /*!< FixupSet ID is invalid                                 */
, PTB_eerrPRCInvalidActnID         = 0x57CA /*!< Action ID is invalid                                   */
, PTB_eerrPRCInvalidProfID         = 0x57C8 /*!< Profile ID is invalid                                  */
, PTB_eerrPRCInvalidMPrfID         = 0x57CB /*!< MetaProfile ID is invalid                              */
, PTB_eerrPRCInvalidPGrpID         = 0x5808 /*!< ProfGroup ID is invalid                                */
, PTB_eerrPRCInvalidDataType       = 0x5610 /*!< Data type wrong/not applicable                         */
, PTB_eerrPRCInvalidCondDataType   = 0x5658 /*!< Condition's data type is invalid                       */
, PTB_eerrPRCInvalidRuleCheckSev   = 0x56D9 /*!< Rule's check severity is invalid                       */
, PTB_eerrPRCWizardCheckData       = 0x5791 /*!< Profile Wizard Check does not have additional data     */
, PTB_eerrPRCInvalidMetaDataID     = 0x5809 /*!< Meta Data ID is invalid                                */
, PTB_eerrPRCInvalidMetaDataIdx    = 0x580A /*!< Meta Data index is invalid                             */
, PTB_eerrPRCInvalidDictkey        = 0x580B /*!< Dict key is invalid                                    */
, PTB_eerrPRCInvalidDynParamID     = 0x580C /*!< Dyn param ID is invalid                                */

/* Resource errors */
, PTB_eerrResource_                = 0x6000 /*!< don't use                                              */
, PTB_eerrResourcePDFX                      /*!< resource for pdfx not found                            */

/* Plugin errors (Acrobat plugin only)                                                                  */
, PTB_eerrPlugin_                  = 0x6200 /*!< don't use                                              */
, PTB_eerrPluginHFT                         /*!< Cannot setup HFT                                       */
, PTB_eerrPluginDocFail                     /*!< PDDoc parameter is wrong                               */

/* Fixup errors */
, PTB_eerrFixup_                   = 0x7000 /*!< don't use                                              */
, PTB_eerrFixupCfgFileNotFound              /*!< the config file was not found                          */
, PTB_eerrFixupCfgFileWrongFormat           /*!< the config file does not have the expected format      */
, PTB_eerrFixupFeatureWrongFormat           /*!< a given fixup feature does not have the expected format*/

/* License Server errors */
, PTB_eerrLicense                   = 0x8000 /*!< don't use                                              */
, PTB_eerrLicenseCommunication               /*!< License server communication error has been detected   */
, PTB_eerrLicenseNoServer                    /*!< No license server found                                */
, PTB_eerrLicenseNoCartridge                 /*!< License server found but no matching cartridge installed */
, PTB_eerrLicenseNoLicense                   /*!< License server with matching cartridge found, but no free license */

, PTB_eerr_Max                     = 0xFFFF /*!< don't use                                              */

};

/******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif // PTB_TYPES_H

/******************************************************************************/
/* EOF */
