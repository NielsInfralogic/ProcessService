/*!
**
** \file ptbPRCTypes.h
**
** \brief callas pdfEngine SDK: PRC API type definitions
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_PRCTYPES_H)
/*! \def PTB_PRCTYPES_H header define */
#define PTB_PRCTYPES_H

/******************************************************************************/

#include "ptbTypes.h"

/******************************************************************************/

#include "ptbProlog.h"

/*
********************************************************************************
** Meta data
********************************************************************************
*/

/*! Meta data key-value pair */
typedef struct {
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCMetaData)               */
	const PTB_utf8_char_t*            key;           /*!< Metadata key                                  */
	const PTB_utf8_char_t*            value;         /*!< Metadata value                                */
} PTB_PRCMetaData;

/*
********************************************************************************
** Managing PRC Engines
********************************************************************************
*/

/******************************************************************************/
/* Acrobat PRC repositories */

/*! Acrobat versions */
enum PTB_EAcrobatVersion { PTB_eavAcrobat6 = 6        /*!< Acrobat  6 */
                         , PTB_eavAcrobat7            /*!< Acrobat  7 */
                         , PTB_eavAcrobat8            /*!< Acrobat  8 */
                         , PTB_eavAcrobat9            /*!< Acrobat  9 */
                         , PTB_eavAcrobat10           /*!< Acrobat 10 */
						 , PTB_eavAcrobat11           /*!< Acrobat 11 */
						 , PTB_eavAcrobatDC           /*!< Acrobat DC (Continuous) */
						 , PTB_eavAcrobatDCClassic    /*!< Acrobat DC (Classic)    */
};

/*
********************************************************************************
** Managing PRC objects in PRC Engines
********************************************************************************
*/

/******************************************************************************/
/* Enumeration callbacks */

/*! Condition enumeration callback type */
typedef void(*PTB_PRCEnumCondCallback)( PTB_PRCEngineID idEng     /*!< ID of PRC Engine where Condition is stored */
                                      , PTB_PRCCondID   idCond    /*!< ID of found Condition                      */
                                      , void*           userData  /*!< User data                                  */
                                      );

/*! Rule enumeration callback type */
typedef void(*PTB_PRCEnumRuleCallback)( PTB_PRCEngineID idEng     /*!< ID of PRC Engine where Rule is stored */
                                      , PTB_PRCRuleID   idRule    /*!< ID of found Rule                      */
                                      , void*           userData  /*!< User data                             */
                                      );

/*! RuleSet enumeration callback type */
typedef void(*PTB_PRCEnumRSetCallback)( PTB_PRCEngineID idEng     /*!< ID of PRC Engine where RuleSet is stored */
                                      , PTB_PRCRSetID   idRSet    /*!< ID of found RuleSet                      */
                                      , void*           userData  /*!< User data                                */
                                      );

/*! Fixup enumeration callback type */
typedef void(*PTB_PRCEnumFxupCallback)( PTB_PRCEngineID idEng     /*!< ID of PRC Engine where Fixup is stored */
                                      , PTB_PRCFxupID   idFxup    /*!< ID of found Fixup                      */
                                      , void*           userData  /*!< User data                              */
                                      );

/*! Fixup enumeration callback type */
typedef void(*PTB_PRCEnumFSetCallback)( PTB_PRCEngineID idEng     /*!< ID of PRC Engine where FixupSet is stored */
                                      , PTB_PRCFSetID   idFxup    /*!< ID of found Fixup                         */
                                      , void*           userData  /*!< User data                                 */
                                      );

/*! Action enumeration callback type */
typedef void(*PTB_PRCEnumActnCallback)( PTB_PRCEngineID idEng     /*!< ID of PRC Engine where Action is stored */
                                      , PTB_PRCActnID   idActn    /*!< ID of found Action                      */
                                      , void*             userData  /*!< User data                             */
                                      );

/*! Profile enumeration callback type */
typedef void(*PTB_PRCEnumProfCallback)( PTB_PRCEngineID idEng     /*!< ID of PRC Engine where Profile is stored */
                                      , PTB_PRCProfID   idProf    /*!< ID of found Profile                      */
                                      , void*             userData  /*!< User data                              */
                                      );

/*! MetaProfile enumeration callback type */
typedef void(*PTB_PRCEnumMPrfCallback)( PTB_PRCEngineID idEng     /*!< ID of PRC Engine where MetaProfile is stored */
                                      , PTB_PRCMPrfID   idMPrf    /*!< ID of found MetaProfile                      */
                                      , void*             userData  /*!< User data                                  */
                                      );

/*! ProfGroup enumeration callback type */
typedef void(*PTB_PRCEnumPGrpCallback)( PTB_PRCEngineID idEng     /*!< ID of PRC Engine where Profile Group is stored */
                                      , PTB_PRCPGrpID   idPGrp    /*!< ID of found ProfGroup                          */
                                      , void*           userData  /*!< User data                                      */
                                      );

/*! Dyn param enumeration callback type */
typedef void(*PTB_PRCEnumDynParamCallback)( PTB_PRCDynParamID idParam   /*!< ID of found ProfGroup                          */
                                          , void*             userData  /*!< User data                                      */
                                          );

/*! MetaProfile param enumeration callback type */
typedef void(*PTB_PRCEnumSequenceStepsCallback)( PTB_PRCEngineID            idEng      /*!< ID of PRC Engine where Meta Profile is stored             */
                                               , PTB_PRCProfID              idProf     /*!< If idProf unequal PTB_INVALID_ID sequence is a Profile    */
                                               , PTB_PRCRuleID              idRule     /*!< If idRule unequal PTB_INVALID_ID sequence is a Rule       */
                                               , PTB_PRCFxupID              idFxup     /*!< If idFxup unequal PTB_INVALID_ID sequence is a Fixup      */
                                               , PTB_PRCActnID              idActn     /*!< If idActn unequal PTB_INVALID_ID sequence is an Action    */
                                               , PTB_PRCDynParamID          idParam    /*!< If idParam unequal PTB_INVALID_ID sequence is an Variable */
											   , void*                      userData   /*!< User data                                                 */
                                               );

/*! MetaProfile sequence step enumeration callback type */
typedef void(*PTB_PRCEnumSequenceStepsCallback2)( PTB_PRCEngineID            idEng      /*!< ID of PRC Engine where Meta Profile is stored              */
                                                , PTB_PRCMPrfID              idMPrf     /*!< If idMPrf unequal PTB_INVALID_ID sequence is a MetaProfile */
                                                , PTB_PRCProfID              idProf     /*!< If idProf unequal PTB_INVALID_ID sequence is a Profile     */
                                                , PTB_PRCRuleID              idRule     /*!< If idRule unequal PTB_INVALID_ID sequence is a Rule        */
                                                , PTB_PRCFxupID              idFxup     /*!< If idFxup unequal PTB_INVALID_ID sequence is a Fixup       */
                                                , PTB_PRCActnID              idActn     /*!< If idActn unequal PTB_INVALID_ID sequence is an Action     */
                                                , PTB_PRCDynParamID          idParam    /*!< If idParam unequal PTB_INVALID_ID sequence is an Variable  */
                                                , PTB_StringID               idOper     /*!< If idOper unequal PTB_INVALID_ID sequence is an Operation (File pickup or Create PDF copy) */
											    , void*                      userData   /*!< User data                                                  */
                                                );

/******************************************************************************/
/* Accessing object data */

/*! Types of data to retrieve */
enum PTB_PRCEDataType { /* Common data                                                                                 */
                          PTB_PRCEdtCommon_            = 0x01000000 /* < don't use                                     */
                        , PTB_PRCEdtName               = 0x01000001 /*!< object's name                                 */
                        , PTB_PRCEdtComment            = 0x01000002 /*!< object's comment                              */
                        , PTB_PRCEdtDictKey            = 0x01000003 /*!< object's dict key                             */
                          /* Condition-only data                                                                       */
                        , PTB_PRCEdtCondition_         = 0x02000000 /* < don't use                                     */
                        , PTB_PRCEdtCondGroup          = 0x02010001 /*!< Condition's property group                    */
                        , PTB_PRCEdtCondProperty       = 0x02010002 /*!< Condition's property                          */
                        , PTB_PRCEdtCondOperator       = 0x02010004 /*!< Condition's operator                          */
                        , PTB_PRCEdtCondDataType       = 0x02020001 /*!< Condition's data type                         */
                        , PTB_PRCEdtCondText           = 0x02020101 /*!< Condition's text                              */
                        , PTB_PRCEdtCondNumText        = 0x02020201 /*!< Condition's number text                       */
                        , PTB_PRCEdtCondNumTolerance   = 0x02020202 /*!< Condition's number tolerance                  */
                        , PTB_PRCEdtCondRectWidth      = 0x02020401 /*!< Condition's rect width                        */
                        , PTB_PRCEdtCondRectHeight     = 0x02020402 /*!< Condition's rect height                       */
                        , PTB_PRCEdtCondRectTolerance  = 0x02020404 /*!< Condition's rect tolerance                    */
                        , PTB_PRCEdtCondRectIgnorOr    = 0x02020408 /*!< Condition's rect ignore orientation flag      */
                        , PTB_PRCEdtCondListSize       = 0x02020801 /*!< Condition's list size                         */
                        , PTB_PRCEdtCondListItem       = 0x02020802 /*!< Condition's list item                         */
                        , PTB_PRCEdtCondJobOptionText  = 0x02021001 /*!< Condition's joboption text                    */
                        , PTB_PRCEdtCondJobOptionData  = 0x02021002 /*!< Condition's joboption data                    */
                          /* Rule-only data                                                                            */
                        , PTB_PRCEdtRule_              = 0x04000000 /* < don't use                                     */
                        , PTB_PRCEdtRuleAnalysisCont   = 0x04010001 /*!< Rule's analysis content                       */
                        , PTB_PRCEdtRuleConditionLogic = 0x04010002 /*!< Rule's condition logic                        */
                        , PTB_PRCEdtRuleReportNoMatch  = 0x04020001 /*!< Rule's report-no-match flag                   */
                        , PTB_PRCEdtRuleNoMatchString  = 0x04020002 /*!< Rule's report-no-match string                 */
                          /* RuleSet-only data                                                                         */
                        , PTB_PRCEdtRuleSet_           = 0x08000000 /* < don't use                                     */
                        , PTB_PRCEdtRSetDefaultFlag    = 0x08000001 /*!< RuleSet's default flag                        */
                          /* Fixup-only data                                                                           */
                        , PTB_PRCEdtFxup_              = 0x10000000 /* < don't use                                     */
                        , PTB_PRCEdtFxupConfigString   = 0x10000001 /*!< Fixup's config string                         */
						, PTB_PRCEdtFxupScope          = 0x10000002 /*!< Fixups's scope                                */
						/* FixupSet-only data                                                                          */
                        , PTB_PRCEdtFixupSet_          = 0x20000000 /* < don't use                                     */
                        , PTB_PRCEdtFSetDefaultFlag    = 0x20000001 /*!< FixupSet's default flag                       */
                          /* Profile-only data                                                                         */
                        , PTB_PRCEdtProfile_           = 0x40000000 /* < don't use                                     */
                        , PTB_PRCEdtProfPDFXAConv      = 0x40000001 /*!< Profile's PDF/X and PDF/A conversion settings */
                        , PTB_PRCEdtProfMetaData       = 0x40000002 /*!< Profile's meta data                           */
                          /* ProfGroup-only data                                                                       */
                        , PTB_PRCEdtProfGroup_         = 0x80000000 /* < don't use                                     */
                          /* Dynamic param data                                                                        */
                        , PTB_PRCEdtDynParam_          = 0xA0000000 /* < don't use                                     */
                        , PTB_PRCEdtDynParamKey        = 0xA0000001 /* < Dynamic param key                             */
                        , PTB_PRCEdtDynParamDefValue   = 0xA0000002 /* < Dynamic param default value                   */
                        , PTB_PRCEdtDynParamName       = 0xA0000003 /* < Dynamic param display name                    */
                        };

/******************************************************************************/
/* Condition data */

/*! Condition data type */
enum PTB_PRCECondDataType { PTB_PRCEcdtInvalid   /*!< invalid data type         */
                          , PTB_PRCEcdtText      /*!< text data                 */
                          , PTB_PRCEcdtNumber    /*!< number data               */
                          , PTB_PRCEcdtRect      /*!< rect data                 */
                          , PTB_PRCEcdtList      /*!< list data                 */
                          , PTB_PRCEcdtJobOption /*!< distiller job option data */
                          };

/*! Text Condition's data element */
typedef struct {
	PTB_size_t                        size;               /*!< must be sizeof(PTB_PRCCondDataText)        */
	const PTB_utf8_char_t*            value;              /*!< Text                                       */
} PTB_PRCCondDataText;

/*! Number Condition's data element */
typedef struct {
	PTB_size_t                        size;               /*!< must be sizeof(PTB_PRCCondDataNumber)      */
	const PTB_utf8_char_t*            value;              /*!< Number                                     */
	const PTB_utf8_char_t*            tolerance;          /*!< Number tolerance                           */
} PTB_PRCCondDataNumber;

/*! Rect Condition's data element */
typedef struct {
	PTB_size_t                        size;               /*!< must be sizeof(PTB_PRCCondDataRect)        */
	const PTB_utf8_char_t*            width;              /*!< Width                                      */
	const PTB_utf8_char_t*            height;             /*!< Height                                     */
	const PTB_utf8_char_t*            tolerance;          /*!< Rect tolerance                             */
	PTB_bool_t                        ignoreOrientation;  /*!< Ignore orientation flag                    */
} PTB_PRCCondDataRect;

/*! List Condition's data element */
typedef struct {
	PTB_size_t                        size;               /*!< must be sizeof(PTB_PRCCondDataList)        */
	const PTB_utf8_char_t**           list;               /*!< List                                       */
	PTB_size_t                        listSize;           /*!< Number of objects in list                  */
} PTB_PRCCondDataList;

/*! Joboption Condition's data element */
typedef struct {
	PTB_size_t                        size;               /*!< must be sizeof(PTB_PRCCondDataJobOption)   */
	const PTB_utf8_char_t*            text;               /*!< Text                                       */
	char*                             data;               /*!< Joboption binary data                      */
	PTB_size_t                        dataSize;           /*!< Data size                                  */
} PTB_PRCCondDataJobOption;

/*! Unified Condition's data element */
typedef struct {
	PTB_size_t                        size;               /*!< must be sizeof(PTB_PRCCondData)   */
	const PTB_utf8_char_t*            name;               /*!< Name                              */
	const PTB_utf8_char_t*            comment;            /*!< Comment                           */
	const PTB_sys_char_t *            dictkey;            /*!< Dict key
	                                                             \note  Either dictkey or name and
	                                                                    comment must be NULL     */
	const char*                       group;              /*!< Property group                    */
	const char*                       property;           /*!< Property                          */
	const char*                       oper;               /*!< Operator                          */
	PTB_bool_t                        regardUserUnit;     /*!< Regard user unit flag             */
	enum PTB_PRCECondDataType         dataType;           /*!< Data type                         */
	union {
		PTB_PRCCondDataText           text;               /*!< Text data                         */
		PTB_PRCCondDataNumber         number;             /*!< Number data                       */
		PTB_PRCCondDataRect           rect;               /*!< Rect data                         */
		PTB_PRCCondDataList           list;               /*!< List data                         */
		PTB_PRCCondDataJobOption      jobOption;          /*!< Joboption data                    */
	}                                 data;               /*!< Data                              */
} PTB_PRCCondData;

/******************************************************************************/
/* Rule data */

/*! Rule analysis content */
enum PTB_PRCERuleAnalysisCont  { PTB_PRCEracPages         = 0x01 /*!< Analyze pages              */
                               , PTB_PRCEracAnnotations   = 0x02 /*!< Analyze annotations        */
                               , PTB_PRCEracFormFields    = 0x04 /*!< Analyze form fields        */
                               , PTB_PRCEracSoftMasks     = 0x08 /*!< Analyze soft masks         */
                               , PTB_PRCEracCurrentFile   = 0x10 /*!< Analyze current file       */
                               , PTB_PRCEracEmbeddedFiles = 0x20 /*!< Analyze all embedded files */
							   };

/*! Rule Condition logic */
enum PTB_PRCEConditionLogic { PTB_PRCEclAnd    = 0 // Fire if all conditions are met
                            , PTB_PRCEclOr     = 1 // Fire if any condition is met
                            , PTB_PRCEclAndNot = 2 // Fire if all negated conditions are met
                            , PTB_PRCEclOrNot  = 3 // Fire if any negated condition is met
                            };

/*! Rule data */
typedef struct {
	PTB_size_t                        size;            /*!< must be sizeof(PTB_PRCRuleData)                       */
	const PTB_utf8_char_t*            name;            /*!< Name                                                  */
	const PTB_utf8_char_t*            comment;         /*!< Comment                                               */
	const PTB_sys_char_t *            dictkey;         /*!< Dict key
	                                                          \note  Either dictkey or name and comment must be
	                                                                 NULL                                         */
	PTB_uint16_t                      analysisCont;    /*!< Analysis (cf. PTB_PRCERuleAnalysisCont)               */
	enum PTB_PRCEConditionLogic       conditionLogic;  /*!< Condition logic                                       */
	const PTB_utf8_char_t*            reportNoMatch;   /*!< String to show if Rule is not matching
	                                                          \note If NULL, report-no-match flag is set to false */
} PTB_PRCRuleData;

/******************************************************************************/
/* RuleSet data */

/*! RuleSet data
**  \note If both name and comment are NULL, PTB_PRCEngineAddRuleSet() will
**        create a default RuleSet. Note, however, that each Profile shall only
**        ever have one default RuleSet.
*/
typedef struct {
	PTB_size_t                        size;     /*!< must be sizeof(PTB_PRCRSetData)             */
	const PTB_utf8_char_t*            name;     /*!< Name                                        */
	const PTB_utf8_char_t*            comment;  /*!< Comment                                     */
	const PTB_sys_char_t *            dictkey;  /*!< Dict key
	                                                   \note  Either dictkey or name and comment
	                                                          must be NULL                       */
} PTB_PRCRSetData;

/*! Rule check severity */
enum PTB_PRCERuleCheckSeverity { PTB_PRCErcsInfo    = 1 /*!< Info severity            */
                               , PTB_PRCErcsWarning = 2 /*!< Warning severity         */
                               , PTB_PRCErcsError   = 3 /*!< Error severity           */
                               };

/******************************************************************************/
/* Fixup data */

/*! Fixup scope */
enum PTB_PRCEFixupScope  { PTB_PRCEfsCurrentFile     = 0x01 /*!< Current file         */
                         , PTB_PRCEfsEmbeddedFiles   = 0x02 /*!< Embedded file        */
                         };

/*! Fixup data */
typedef struct {
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCFxupData)             */
	const PTB_utf8_char_t*            name;          /*!< Name                                        */
	const PTB_utf8_char_t*            comment;       /*!< Comment                                     */
	const PTB_sys_char_t *            dictkey;       /*!< Dict key
	                                                        \note  Either dictkey or name and comment
	                                                               must be NULL                       */
	const PTB_utf8_char_t*            configString;  /*!< Config string                               */
	unsigned short                    scope;         /*!< Scope (cf. PTB_PRCEFixupScope)              */
} PTB_PRCFxupData;

/******************************************************************************/
/* FixupSet data */

/*! FixupSet data
**  \note If both name and comment are NULL, PTB_PRCEngineAddFixupSet() will
**        create a default FixupSet. Note, however, that each Profile shall only
**        ever have one default FixupSet.
*/
typedef struct {
	PTB_size_t                        size;     /*!< must be sizeof(PTB_PRCFSetData)             */
	const PTB_utf8_char_t*            name;     /*!< Name                                        */
	const PTB_utf8_char_t*            comment;  /*!< Comment                                     */
	const PTB_sys_char_t *            dictkey;  /*!< Dict key
	                                                   \note  Either dictkey or name and comment
	                                                          must be NULL                       */
} PTB_PRCFSetData;

/******************************************************************************/
/* Action data */

/*! Action data
*/
typedef struct {
	PTB_size_t                        size;     /*!< must be sizeof(PTB_PRCActnData)     */
	const PTB_utf8_char_t*            name;     /*!< Name                                      */
	const PTB_utf8_char_t*            comment;  /*!< Comment                                   */
	const PTB_sys_char_t *            dictkey;  /*!< Dict key
	                                                   \note  Either dictkey or name and comment
	                                                          must be NULL                       */
} PTB_PRCActnData;

/******************************************************************************/
/* Profile data */

/*! Profile PDF/X and PDF/A conversion status */
typedef struct {
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCProfPDFXAStatus)                   */
	PTB_uint32_t                      status;        /*!< PDF/X and PDF/A status flags (cf. PTB_PRCEPDFXAStatus) */
	const PTB_utf8_char_t*            outputIntent;  /*!< Name of output intent                                    */
} PTB_PRCProfPDFXAStatus;

/*! Profile data */
typedef struct {
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCProfData)           */
	const PTB_utf8_char_t*            name;          /*!< Name                                      */
	const PTB_utf8_char_t*            comment;       /*!< Comment                                   */
	const PTB_sys_char_t *            dictkey;       /*!< Dict key
	                                                        \note  Either dictkey or name and comment
	                                                               must be NULL                     */
	PTB_PRCProfPDFXAStatus            pdfxaStatus;   /*!< PDF/X and PDF/A status                    */
	PTB_PRCMetaData*                  metaDataList;  /*!< Metadata list                             */
	PTB_size_t                        metaDataSize;  /*!< Number of objects in metadata list        */
} PTB_PRCProfData;

/*! Profile PDF/X and PDF/A Status */
enum PTB_PRCEPDFXAStatus
{
  /* General values                                                                                       */
  PTB_epdfxaInvalid         = 0xFFFFFFF  /*!< Invalid status                                              */
, PTB_epdfxaNoConversions   = 0x0000000  /*!< No conversion                                               */

  /* PDF/X compliance                                                                                     */
, PTB_epdfxaXComplianceMask = 0x000000F  /*!< PDF/X compliance mask                                       */
, PTB_epdfxaXNone           = 0x0000000  /*!< No PDF/X conversion                                         */
, PTB_epdfxaX1a_2001        = 0x0000001  /*!< Convert to PDF/X-1a:2001                                    */
, PTB_epdfxaX3_2002         = 0x0000002  /*!< Convert to PDF/X-3:2002                                     */
, PTB_epdfxaX4              = 0x0000005  /*!< Convert to PDF/X-4
                                             \note Conversion to PDF/X-1a:2003 and PDF/X-3:2003
                                                   is not supported.                                      */
, PTB_epdfxaX4p             = 0x0000006  /*!< Convert to PDF/X-4p                                         */
, PTB_epdfxaX5n             = 0x0000007  /*!< Convert to PDF/X-5n                                         */

  /* PDF/X ask before conversion                                                                          */
, PTB_epdfxaXAskMask        = 0x0000010  /*!< Ask before conversion mask                                  */
, PTB_epdfxaXDontAsk        = 0x0000000  /*!< Don't ask before conversion                                 */
, PTB_epdfxaXAsk            = 0x0000010  /*!< Ask before conversion
                                             \note The pdfInspektor Library will ignore this flag.        */
  /* PDF/X Trapped status                                                                                 */
, PTB_epdfxaTrappedMask     = 0x0000020  /*!< Trapped status mask                                         */
, PTB_epdfxaNotTrapped      = 0x0000000  /*!< Set to not trapped if trapped neither true nor false        */
, PTB_epdfxaTrapped         = 0x0000020  /*!< Set to trapped if trapped neither true nor false            */

  /* PDF/X Apply fixups                                                                                   */
, PTB_epdfxaXApplyFixupsMask= 0x0000040  /*!< Apply fixups mask                                           */
, PTB_epdfxaXApplyFixups    = 0x0000000  /*!< Set Apply fixups to true                                    */
, PTB_epdfxaXApplyFixupsNone= 0x0000040  /*!< Set Apply fixups to false                                   */

  /* PDF/X Convert device independent RGB or Lab color into destination color space                                                  */
, PTB_epdfxaXRGB2DestMask   = 0x0000080  /*!< Convert device independent RGB or Lab color into destination color space mask          */
, PTB_epdfxaXRGB2DestNone   = 0x0000000  /*!< Set Convert device independent RGB or Lab color into destination color space to false  */
, PTB_epdfxaXRGB2Dest       = 0x0000080  /*!< Set Convert device independent RGB or Lab color into destination color space to true   */

  /* PDF/X Decalibrate device independent Gray or CMYK                                                    */
, PTB_epdfxaXDecCMYKMask    = 0x0000100  /*!< Decalibrate device independent Gray or CMYK mask            */
, PTB_epdfxaXDecCMYKNone    = 0x0000000  /*!< Set Decalibrate device independent Gray or CMYK to false    */
, PTB_epdfxaXDecCMYK        = 0x0000100  /*!< Set Decalibrate device independent Gray or CMYK to true     */

  /* PDF/X Color spaces                                                                                   */
, PTB_epdfxaColorMask       = 0x0000000  /*!< No color space
                                             \note Ignored during conversion, actual value is set
                                                   internally according to color space of ICC profile
                                                   from OutputIntent, may be set in an existing profile
                                                   though, \sa PTB_PRCGetProfileData,
                                                   PTB_PRCEdtProfPDFXAConv                                */
, PTB_epdfxaColorNone       = 0x0000000  /*!< No color space                                              */
, PTB_epdfxaColorCMYK       = 0x0000000  /*!< CMYK color space                                            */
, PTB_epdfxaColorRGB        = 0x0000000  /*!< RGB color space                                             */
, PTB_epdfxaColorGray       = 0x0000000  /*!< Gray color space                                            */

  /* PDF/A compliance                                                                                     */
, PTB_epdfxaAComplianceMask = 0x000F000  /*!< PDF/A compliance mask                                       */
, PTB_epdfxaANone           = 0x0000000  /*!< No PDF/A conversion                                         */
, PTB_epdfxaA1a_2005        = 0x0001000  /*!< convert to PDF/A-1A:2005                                    */
, PTB_epdfxaA1b_2005        = 0x0002000  /*!< convert to PDF/A-1B:2005                                    */
, PTB_epdfxaA2a             = 0x0003000  /*!< convert to PDF/A-2a                                         */
, PTB_epdfxaA2b             = 0x0004000  /*!< convert to PDF/A-2b                                         */
, PTB_epdfxaA2u             = 0x0005000  /*!< convert to PDF/A-2u                                         */
, PTB_epdfxaA3a             = 0x0006000  /*!< convert to PDF/A-3a                                         */
, PTB_epdfxaA3b             = 0x0007000  /*!< convert to PDF/A-3b                                         */
, PTB_epdfxaA3u             = 0x0008000  /*!< convert to PDF/A-3u                                         */

  /* PDF/A ask before conversion                                                                          */
, PTB_epdfxaAAskMask        = 0x0010000  /*!< Ask before conversion mask                                  */
, PTB_epdfxaADontAsk        = 0x0000000  /*!< Don't ask before conversion                                 */
, PTB_epdfxaAAsk            = 0x0010000  /*!< Ask before conversion
                                             \note The pdfInspektor Library will ignore this flag.        */
  /* PDF/A Apply fixups                                                                                   */
, PTB_epdfxaAApplyFixupsMask= 0x0020000  /*!< Apply fixups mask                                           */
, PTB_epdfxaAApplyFixups    = 0x0000000  /*!< Set Apply fixups to true                                    */
, PTB_epdfxaAApplyFixupsNone= 0x0020000  /*!< Set Apply fixups to false                                   */

  /* Output intent options                                                                                */
, PTB_epdfxaOIMask          = 0x0F00000  /*!< OutputIntent optins mask                                    */
, PTB_epdfxaOINone          = 0x0000000  /*!< Embed OutputIntent                                          */
, PTB_epdfxaOIOmitIfAllowed = 0x0100000  /*!< Don't embed ICC profile if allowed                          */
, PTB_epdfxaOIUseEmbedded   = 0x0200000  /*!< Use an existing output intent if available                  */
, PTB_epdfxaOIOmitOrUseEmb  = 0x0300000  /*!< Don't embed if allowed, otherwise use existing if available */
};

/*! Author metadata key constant (see \ref PRCMetaData_Intro_sec) */
#define PTB_PRC_PROFILE_METAKEY_AUTHOR "author"

/*! Email metadata key constant (see \ref PRCMetaData_Intro_sec) */
#define PTB_PRC_PROFILE_METAKEY_EMAIL  "email"

/*! Rating metadata key constant (see \ref PRCMetaData_Intro_sec) */
#define PTB_PRC_PROFILE_METAKEY_RATING "rating"

#define PTB_PRC_FIXUP_RULE_REFERENCE "CALS_CLM_REFRULE_IDX"
#define PTB_PRC_CONDITION_RULE_REFERENCE "CALS_SIFTER_REFRULE_IDX"

/*! Wizard Check state */
enum PTB_EWizardCheckState
{ PTB_ewcsInactive    = 0 /*!< Wizard Check turned off          */
, PTB_ewcsInfo        = 1 /*!< Hit causes informational message */
, PTB_ewcsWarning     = 2 /*!< Hit causes warning message       */
, PTB_ewcsError       = 3 /*!< Hit causes error message         */
};

/*! Wizard Check state data items to retrieve */
enum PTB_EWizardCheckStateData
{ /* General masks                                                                                                                                                       */
  PTB_ewcsdWizardCheckMask                  = 0xFFFF00                                                      /*   Don't use                                               */
, PTB_ewcsdInListMask                       = 0x000001                                                      /*   Don't use                                               */
, PTB_ewcsdListSizeMask                     = 0x000002                                                      /*   Don't use                                               */
, PTB_ewcsdListItemMask                     = 0x000004                                                      /*   Don't use                                               */
  /* Document checks                                                                                                                                                     */
, PTB_ewcsdDocumentRequiresAtLeast          = 0x010100                                                      /*!< RequiresAtLeast Wizard Check                            */
, PTB_ewcsdDocumentRequiresAtLeastVersion   = 0x010101                                                      /*!< RequiresAtLeast Wizard Check Acrobat versions           */
, PTB_ewcsdDocumentEncrypted                = 0x010200                                                      /*!< RequiresAtLeast Wizard Check                            */
, PTB_ewcsdDocumentDamaged                  = 0x010400                                                      /*!< RequiresAtLeast Wizard Check                            */
, PTB_ewcsdDocumentSyntaxChecks             = 0x010800                                                      /*!< RequiresAtLeast Wizard Check                            */
  /* Page checks                                                                                                                                                         */
, PTB_ewcsdPageSizeIsNot                    = 0x020100                                                      /*!< PageSizeIsNot Wizard Check                              */
, PTB_ewcsdPageSizeIsNotWidth               = 0x020101                                                      /*!< PageSizeIsNot Wizard Check Page width                   */
, PTB_ewcsdPageSizeIsNotHeight              = 0x020102                                                      /*!< PageSizeIsNot Wizard Check Page height                  */
, PTB_ewcsdPageSizeIsNotTolerance           = 0x020104                                                      /*!< PageSizeIsNot Wizard Check Page size tolerance          */
, PTB_ewcsdPageSizeIsNotUnit                = 0x020108                                                      /*!< PageSizeIsNot Wizard Check Length unit                  */
, PTB_ewcsdPageSizeIsNotOrientation         = 0x020110                                                      /*!< PageSizeIsNot Wizard Check Ignore-orientation flag      */
, PTB_ewcsdPageNumPages                     = 0x020200                                                      /*!< NumPages Wizard Check                                   */
, PTB_ewcsdPageNumPagesOperator             = 0x020201                                                      /*!< NumPages Wizard Check Operator                          */
, PTB_ewcsdPageNumPagesNumber               = 0x020202                                                      /*!< NumPages Wizard Check Page                              */
, PTB_ewcsdPageSizeOrientDifferent          = 0x020400                                                      /*!< SizeOrientDifferent Wizard Check                        */
, PTB_ewcsdPageOnePageEmpty                 = 0x020800                                                      /*!< OnePageEmpty Wizard Check                               */
, PTB_ewcsdPageOnePageEmptyIgnoreOutsideBox = 0x020801                                                      /*!< OnePageEmpty Wizard Check Ignore-outside-boxes flag     */
  /* Image checks                                                                                                                                                        */
, PTB_ewcsdImageResImgLower                 = 0x040100                                                      /*!< ImagResImgLower Wizard Check                            */
, PTB_ewcsdImageResImgLowerPPI              = 0x040101                                                      /*!< ImagResImgLower Wizard Check Pixels per inch            */
, PTB_ewcsdImageResImgUpper                 = 0x040200                                                      /*!< ImagResImgUpper Wizard Check                            */
, PTB_ewcsdImageResImgUpperPPI              = 0x040201                                                      /*!< ImagResImgUpper Wizard Check Pixels per inch            */
, PTB_ewcsdImageResBmpLower                 = 0x040400                                                      /*!< ImagResBmpLower Wizard Check                            */
, PTB_ewcsdImageResBmpLowerPPI              = 0x040401                                                      /*!< ImagResBmpLower Wizard Check Pixels per inch            */
, PTB_ewcsdImageResBmpUpper                 = 0x040800                                                      /*!< ImagResBmpUpper Wizard Check                            */
, PTB_ewcsdImageResBmpUpperPPI              = 0x040801                                                      /*!< ImagResBmpUpper Wizard Check Pixels per inch            */
, PTB_ewcsdImageUncompressed                = 0x041000                                                      /*!< Uncompressed Wizard Check                               */
, PTB_ewcsdImageUseLossyCompression         = 0x042000                                                      /*!< UseLossyCompression Wizard Check                        */
, PTB_ewcsdImageUseOPI                      = 0x044000                                                      /*!< UseOPI Wizard Check                                     */
  /* Color checks                                                                                                                                                        */
, PTB_ewcsdColorCMYSeparations              = 0x080100                                                      /*!< CMYSeparations Wizard Check                             */
, PTB_ewcsdColorMoreThan                    = 0x080200                                                      /*!< MoreThan Wizard Check                                   */
, PTB_ewcsdColorMoreThanPlates              = 0x080201                                                      /*!< MoreThan Wizard Check Spot color plates                 */
, PTB_ewcsdColorRGB                         = 0x080400                                                      /*!< RGB Wizard Check                                        */
, PTB_ewcsdColorDICS                        = 0x080800                                                      /*!< DICS Wizard Check                                       */
, PTB_ewcsdColorObjectUseSpotColors         = 0x081000                                                      /*!< ObjectUseSpotColors Wizard Check                        */
, PTB_ewcsdColorObjectUseSpotColorsInList   = PTB_ewcsdColorObjectUseSpotColors | PTB_ewcsdInListMask   /*!< ObjectUseSpotColors Wizard Check Is-in-list flag            */
, PTB_ewcsdColorObjectUseSpotColorsListSize = PTB_ewcsdColorObjectUseSpotColors | PTB_ewcsdListSizeMask /*!< ObjectUseSpotColors Wizard Check Spot color list size       */
, PTB_ewcsdColorObjectUseSpotColorsListItem = PTB_ewcsdColorObjectUseSpotColors | PTB_ewcsdListItemMask /*!< ObjectUseSpotColors Wizard Check Spot color list item       */
, PTB_ewcsdColorInconsistentNaming          = 0x082000                                                      /*!< InconsistentNaming Wizard Check                         */
  /* Font checks                                                                                                                                                         */
, PTB_ewcsdFontNotEmbedded                  = 0x100100                                                      /*!< FontNotEmbedded Wizard Check                            */
, PTB_ewcsdFontEmbedded                     = 0x100200                                                      /*!< FontEmbedded Wizard Check                               */
, PTB_ewcsdFontEmbeddedType                 = 0x100201                                                      /*!< FontEmbedded Wizard Check Embedded as subset/completely */
, PTB_ewcsdFontType1                        = 0x100400                                                      /*!< FontType1 Wizard Check                                  */
, PTB_ewcsdFontTrueType                     = 0x100800                                                      /*!< FontTrueType Wizard Check                               */
, PTB_ewcsdFontType1CID                     = 0x101000                                                      /*!< FontType1CID Wizard Check                               */
, PTB_ewcsdFontTrueTypeCID                  = 0x102000                                                      /*!< FontTrueTypeCID Wizard Check                            */
, PTB_ewcsdFontType3                        = 0x104000                                                      /*!< FontType3 Wizard Check                                  */
, PTB_ewcsdFontOpenType                     = 0x108000                                                      /*!< FontOpenType Wizard Check                               */
, PTB_ewcsdFontIsUsed                       = 0x110000                                                      /*!< FontIsUsed Wizard Check                                 */
, PTB_ewcsdFontIsUsedInList                 = PTB_ewcsdFontIsUsed | PTB_ewcsdInListMask                 /*!< FontIsUsed Wizard Check Is-in-list flag                     */
, PTB_ewcsdFontIsUsedListSize               = PTB_ewcsdFontIsUsed | PTB_ewcsdListSizeMask               /*!< FontIsUsed Wizard Check Font list size                      */
, PTB_ewcsdFontIsUsedListItem               = PTB_ewcsdFontIsUsed | PTB_ewcsdListItemMask               /*!< FontIsUsed Wizard Check Font list item                      */
  /* Rendering checks                                                                                                                                                    */
, PTB_ewcsdRenderingTransparency            = 0x200100                                                      /*!< RenderingTransparency Wizard Check                      */
, PTB_ewcsdRenderingHalftone                = 0x200200                                                      /*!< RenderingHalftone Wizard Check                          */
, PTB_ewcsdRenderingCurve                   = 0x200400                                                      /*!< RenderingCurve Wizard Check                             */
, PTB_ewcsdRenderingThickness               = 0x200800                                                      /*!< RenderingThickness Wizard Check                         */
, PTB_ewcsdRenderingThicknessPoints         = 0x200801                                                      /*!< RenderingThickness Wizard Check Thickness in points     */
, PTB_ewcsdRenderingPostscript              = 0x201000                                                      /*!< RenderingPostscript Wizard Check                        */
  /* Standards checks                                                                                                                                                    */
, PTB_ewcsdPDFXADocumentPDFX                = 0x400100                                                      /*!< DocumentPDFX Wizard Check                               */
, PTB_ewcsdPDFXADocumentPDFX1a2001          = 0x400101                                                      /*!< DocumentPDFX Wizard Check PDF/X-1a (2001)               */
, PTB_ewcsdPDFXADocumentPDFX1a2003          = 0x400102                                                      /*!< DocumentPDFX Wizard Check PDF/X-1a (2003)               */
, PTB_ewcsdPDFXADocumentPDFX32002           = 0x400104                                                      /*!< DocumentPDFX Wizard Check PDF/X-3 (2002)                */
, PTB_ewcsdPDFXADocumentPDFX32003           = 0x400108                                                      /*!< DocumentPDFX Wizard Check PDF/X-3 (2003)                */
, PTB_ewcsdPDFXADocumentPDFX4               = 0x400110                                                      /*!< DocumentPDFX Wizard Check PDF/X-4                       */
, PTB_ewcsdPDFXADocumentPDFX4p              = 0x400120                                                      /*!< DocumentPDFX Wizard Check PDF/X-4p                      */
, PTB_ewcsdPDFXADocumentPDFX5g              = 0x400140                                                      /*!< DocumentPDFX Wizard Check PDF/X-5g                      */
, PTB_ewcsdPDFXADocumentPDFX5pg             = 0x400180                                                      /*!< DocumentPDFX Wizard Check PDF/X-5pg                     */
, PTB_ewcsdPDFXADocumentPDFX5n              = 0x400181                                                      /*!< DocumentPDFX Wizard Check PDF/X-5n                     */
, PTB_ewcsdPDFXADocumentPDFA                = 0x400200                                                      /*!< DocumentPDFA Wizard Check                               */
, PTB_ewcsdPDFXADocumentPDFA1a2005          = 0x400201                                                      /*!< DocumentPDFA Wizard Check PDF/A-1a (2005)               */
, PTB_ewcsdPDFXADocumentPDFA1b2005          = 0x400202                                                      /*!< DocumentPDFA Wizard Check PDF/A-1b (2005)               */
, PTB_ewcsdPDFXADocumentPDFA2a              = 0x400204                                                      /*!< DocumentPDFA Wizard Check PDF/A-2a                      */
, PTB_ewcsdPDFXADocumentPDFA2u              = 0x400208                                                      /*!< DocumentPDFA Wizard Check PDF/A-2u                      */
, PTB_ewcsdPDFXADocumentPDFA2b              = 0x400210                                                      /*!< DocumentPDFA Wizard Check PDF/A-2b                      */
, PTB_ewcsdPDFXADocumentPDFA3a              = 0x400220                                                      /*!< DocumentPDFA Wizard Check PDF/A-3a                      */
, PTB_ewcsdPDFXADocumentPDFA3u              = 0x400240                                                      /*!< DocumentPDFA Wizard Check PDF/A-3u                      */
, PTB_ewcsdPDFXADocumentPDFA3b              = 0x400280                                                      /*!< DocumentPDFA Wizard Check PDF/A-3b                      */
, PTB_ewcsdPDFXADocumentPDFE                = 0x400300                                                      /*!< DocumentPDFE Wizard Check                               */
, PTB_ewcsdPDFXADocumentPDFE1               = 0x400301                                                      /*!< DocumentPDFE Wizard Check PDF/E-1                       */
, PTB_ewcsdPDFXADocumentPDFUA               = 0x400400                                                      /*!< DocumentPDFUA Wizard Check                              */
, PTB_ewcsdPDFXADocumentPDFUA1              = 0x400401                                                      /*!< DocumentPDFUA Wizard Check PDF/UA-1                     */
, PTB_ewcsdPDFXADocumentPDFVT               = 0x400500                                                      /*!< DocumentPDFVT Wizard Check                              */
, PTB_ewcsdPDFXADocumentPDFVT1              = 0x400501                                                      /*!< DocumentPDFVT Wizard Check PDF/VT-1                     */
, PTB_ewcsdPDFXADocumentPDFVT2              = 0x400502                                                      /*!< DocumentPDFVT Wizard Check PDF/VT-2                     */
, PTB_ewcsdPDFXADocumentRGB                 = 0x400700                                                      /*!< DocumentRGB Wizard Check                                */
, PTB_ewcsdPDFXAOutputCondition             = 0x400800                                                      /*!< OutputCondition Wizard Check                            */
, PTB_ewcsdPDFXAOutputConditionInList       = PTB_ewcsdPDFXAOutputCondition | PTB_ewcsdInListMask       /*!< OutputCondition Wizard Check Is-in-list flag                */
, PTB_ewcsdPDFXAOutputConditionListSize     = PTB_ewcsdPDFXAOutputCondition | PTB_ewcsdListSizeMask     /*!< OutputCondition Wizard Check Output Condition list size     */
, PTB_ewcsdPDFXAOutputConditionListItem     = PTB_ewcsdPDFXAOutputCondition | PTB_ewcsdListItemMask     /*!< OutputCondition Wizard Check Output Condition list item     */
};

/*! Acrobat version for RequiresAtLeast Wizard Check */
enum PTB_EWCAcrobatVersion
{ PTB_ewcav4 = 4 /*!< Acrobat 4 */
, PTB_ewcav5     /*!< Acrobat 5 */
, PTB_ewcav6     /*!< Acrobat 6 */
, PTB_ewcav7     /*!< Acrobat 7 */
};

/*! Font embedding for FontEmbedded Wizard Check */
enum PTB_EFontsEmbedded
{ PTB_efeAsSubset    /*!< Subset of font embedded */
, PTB_efeCompletely  /*!< Complete font embedded  */
};

/* Document Wizard Checks */
struct PTB_PRCWC_DocumentRequiresAtLeast     /*!< Document  Wizard Check RequiresAtLeast        */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ enum PTB_EWCAcrobatVersion eav           ; /*!< Acrobat version                    */ };
struct PTB_PRCWC_DocumentEncrypted           /*!< Document  Wizard Check Encrypted              */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_DocumentDamaged             /*!< Document  Wizard Check Damaged                */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_DocumentSyntaxChecks        /*!< Document  Wizard Check Syntax Checks          */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };

/* Page Wizard Checks */
struct PTB_PRCWC_PageSizeIsNot               /*!< Page      Wizard Check SizeIsNot              */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_float_t                width              ; /*!< Page width                         */ PTB_float_t               height    ; /*!< Page height                */ PTB_float_t       tolerance; /*!< Allowed tolerance         */ const PTB_utf8_char_t* unit      ; /*!< Size unit                 */ PTB_bool_t orientation; /*!< Ignore-orientation flag   */ };
struct PTB_PRCWC_PageNumPages                /*!< Page      Wizard Check NumPages               */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_utf8_char_t*           oper               ; /*!< Operator                           */ PTB_uint32_t              number    ; /*!< Number of pages            */ };
struct PTB_PRCWC_PageSizeOrientDifferent     /*!< Page      Wizard Check SizeOrientDifferent    */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_PageOnePageEmpty            /*!< Page      Wizard Check OnePageEmpty           */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_bool_t                 ignoreOutsideBoxes ; /*!< Ignore area outside trim/bleed box */ };

/* Image Wizard Checks */
struct PTB_PRCWC_ImageResImgLower            /*!< Image     Wizard Check ResImgLower            */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_float_t                pixelsPerInch      ; /*!< Image resolution                    */ };
struct PTB_PRCWC_ImageResImgUpper            /*!< Image     Wizard Check ResImgUpper            */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_float_t                pixelsPerInch      ; /*!< Image resolution                    */ };
struct PTB_PRCWC_ImageResBmpLower            /*!< Image     Wizard Check ResBmpLower            */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_float_t                pixelsPerInch      ; /*!< Bitmap resolution                   */ };
struct PTB_PRCWC_ImageResBmpUpper            /*!< Image     Wizard Check ResBmpUpper            */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_float_t                pixelsPerInch      ; /*!< Bitmap resolution                   */ };
struct PTB_PRCWC_ImageUncompressed           /*!< Image     Wizard Check Uncompressed           */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_ImageUseLossyCompression    /*!< Image     Wizard Check UseLossyCompression    */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_ImageUseOPI                 /*!< Image     Wizard Check UseOPI                 */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };

/* Color Wizard Checks */
struct PTB_PRCWC_ColorCMYSeparations         /*!< Color     Wizard Check CMYSeparations         */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_ColorMoreThan               /*!< Color     Wizard Check MoreThan               */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_uint32_t               spotColorSepsOnPage; /*!< Spot color plates                  */ };
struct PTB_PRCWC_ColorRGB                    /*!< Color     Wizard Check RGB                    */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_ColorDICS                   /*!< Color     Wizard Check DICS                   */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_ColorObjectUseSpotColors    /*!< Color     Wizard Check ObjectUseSpotColors    */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_bool_t                 inList             ; /*!< Is-in-list flag                    */ const PTB_utf8_char_t** list      ; /*!< Spot color list            */ PTB_size_t listSize ; /*!< Spot color list size      */ };
struct PTB_PRCWC_ColorInconsistentNaming     /*!< Color     Wizard Check InconsistentNaming     */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };

/* Font Wizard Checks */
struct PTB_PRCWC_FontNotEmbedded             /*!< Font      Wizard Check NotEmbedded            */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_FontEmbedded                /*!< Font      Wizard Check Embedded               */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ enum PTB_EFontsEmbedded    efe                ; /*!< Embedded as subset/completely      */ };
struct PTB_PRCWC_FontType1                   /*!< Font      Wizard Check Type1                  */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_FontTrueType                /*!< Font      Wizard Check TrueType               */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_FontType1CID                /*!< Font      Wizard Check Type1CID               */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_FontTrueTypeCID             /*!< Font      Wizard Check TrueTypeCID            */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_FontType3                   /*!< Font      Wizard Check Type3                  */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_FontOpenType                /*!< Font      Wizard Check OpenType               */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_FontIsUsed                  /*!< Font      Wizard Check FontIsUsed             */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_bool_t                 inList             ; /*!< Is-in-list flag                    */ const PTB_utf8_char_t** list      ; /*!< Font list                  */ PTB_size_t listSize ; /*!< Font list size            */ };

/* Rendering Wizard Checks */
struct PTB_PRCWC_RenderingTransparency       /*!< Rendering Wizard Check Transparency           */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_RenderingHalftone           /*!< Rendering Wizard Check Halftone               */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_RenderingCurve              /*!< Rendering Wizard Check Curve                  */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_RenderingThickness          /*!< Rendering Wizard Check Thickness              */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_float_t                points             ; /*!< Line thickness                     */ };
struct PTB_PRCWC_RenderingPostscript         /*!< Rendering Wizard Check Postscript             */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };

/* PDF/X/A Compliance Wizard Checks */
struct PTB_PRCWC_PDFXADocumentPDFX           /*!< PDF/X/A   Wizard Check DocumentPDFX           */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_bool_t                 pdfx1a2001         ; /*!< PDF/X-1A (2001) compliance         */ PTB_bool_t              pdfx1a2003; /*!< PDF/X-1A (2003) compliance */ PTB_bool_t pdfx32002; /*!< PDF/X-3 (2002) compliance  */ PTB_bool_t              pdfx32003 ; /*!< PDF/X-3 (2003) compliance */ PTB_bool_t pdfx4      ; /*!< PDF/X-4 compliance  */ PTB_bool_t pdfx4p   ; /*!< PDF/X-4p compliance  */ PTB_bool_t              pdfx5g    ; /*!< PDF/X-5g compliance */ PTB_bool_t pdfx5pg    ; /*!< PDF/X-5pg compliance */ ; PTB_bool_t pdfx5n    ;/*!< PDF/X-5n compliance */ };
struct PTB_PRCWC_PDFXADocumentPDFA           /*!< PDF/X/A   Wizard Check DocumentPDFA           */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_bool_t                 pdfa1a2005         ; /*!< PDF/A-1A (2005) compliance         */ PTB_bool_t              pdfa1b2005; /*!< PDF/A-1B (2005) compliance */ PTB_bool_t pdfa2a   ; /*!< PDF/A-2a compliance        */ PTB_bool_t              pdfa2u    ; /*!< PDF/A-2u compliance       */ PTB_bool_t pdfa2b     ; /*!< PDF/A-2b compliance */ PTB_bool_t pdfa3a   ; /*!< PDF/A-3a compliance  */ PTB_bool_t              pdfa3u    ; /*!< PDF/A-3u compliance */ PTB_bool_t pdfa3b     ; /*!< PDF/A-3b compliance  */ };
struct PTB_PRCWC_PDFXADocumentPDFE           /*!< PDF/X/A   Wizard Check DocumentPDFE           */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_bool_t                 pdfe1              ; /*!< PDF/E-1 (2008) compliance          */ };
struct PTB_PRCWC_PDFXADocumentPDFUA          /*!< PDF/X/A   Wizard Check DocumentPDFUA          */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_bool_t                 pdfua1             ; /*!< PDF/UA-1  compliance               */ };
struct PTB_PRCWC_PDFXADocumentPDFVT          /*!< PDF/X/A   Wizard Check DocumentPDFVT          */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_bool_t                 pdfvt1             ; /*!< PDF/VT-1 (2010) compliance         */ PTB_bool_t              pdfvt2    ; /*!< PDF/VT-2 (2010) compliance */ };
struct PTB_PRCWC_PDFXADocumentRGB            /*!< PDF/X/A   Wizard Check DocumentRGB            */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ };
struct PTB_PRCWC_PDFXAOutputCondition        /*!< PDF/X/A   Wizard Check OutputCondition        */ { enum PTB_EWizardCheckState ewcs; /*!< Wizard Check state */ PTB_bool_t                 inList             ; /*!< Is-in-list flag                    */ const PTB_utf8_char_t** list      ; /*!< Output Condition list      */ PTB_size_t listSize ; /*!< Output Condition list size */ };

/*! Profile's Wizard Check data */
struct PTB_PRCWizardChecks {
	struct PTB_PRCWC_DocumentRequiresAtLeast     documentRequiresAtLeast;      /*!< Document  Wizard Check RequiresAtLeast        */
	struct PTB_PRCWC_DocumentEncrypted           documentEncrypted;            /*!< Document  Wizard Check Encrypted              */
	struct PTB_PRCWC_DocumentDamaged             documentDamaged;              /*!< Document  Wizard Check Damaged                */
	struct PTB_PRCWC_DocumentSyntaxChecks        documentSyntaxChecks;         /*!< Document  Wizard Check SyntaxChecks           */
	struct PTB_PRCWC_PageSizeIsNot               pageSizeIsNot;                /*!< Page      Wizard Check SizeIsNot              */
	struct PTB_PRCWC_PageNumPages                pageNumPages;                 /*!< Page      Wizard Check NumPages               */
	struct PTB_PRCWC_PageSizeOrientDifferent     pageSizeOrientDifferent;      /*!< Page      Wizard Check SizeOrientDifferent    */
	struct PTB_PRCWC_PageOnePageEmpty            pageOnePageEmpty;             /*!< Page      Wizard Check OnePageEmpty           */
	struct PTB_PRCWC_ImageResImgLower            imageResImgLower;             /*!< Image     Wizard Check ResImgLower            */
	struct PTB_PRCWC_ImageResImgUpper            imageResImgUpper;             /*!< Image     Wizard Check ResImgUpper            */
	struct PTB_PRCWC_ImageResBmpLower            imageResBmpLower;             /*!< Image     Wizard Check ResBmpLower            */
	struct PTB_PRCWC_ImageResBmpUpper            imageResBmpUpper;             /*!< Image     Wizard Check ResBmpUpper            */
	struct PTB_PRCWC_ImageUncompressed           imageUncompressed;            /*!< Image     Wizard Check Uncompressed           */
	struct PTB_PRCWC_ImageUseLossyCompression    imageUseLossyCompression;     /*!< Image     Wizard Check UseLossyCompression    */
	struct PTB_PRCWC_ImageUseOPI                 imageUseOPI;                  /*!< Image     Wizard Check UseOPI                 */
	struct PTB_PRCWC_ColorCMYSeparations         colorCMYSeparations;          /*!< Color     Wizard Check CMYSeparations         */
	struct PTB_PRCWC_ColorMoreThan               colorMoreThan;                /*!< Color     Wizard Check MoreThan               */
	struct PTB_PRCWC_ColorRGB                    colorRGB;                     /*!< Color     Wizard Check RGB                    */
	struct PTB_PRCWC_ColorDICS                   colorDICS;                    /*!< Color     Wizard Check DICS                   */
	struct PTB_PRCWC_ColorObjectUseSpotColors    colorObjectUseSpotColors;     /*!< Color     Wizard Check ObjectUseSpotColors    */
	struct PTB_PRCWC_ColorInconsistentNaming     colorInconsistentNaming;      /*!< Color     Wizard Check InconsistentNaming     */
	struct PTB_PRCWC_FontNotEmbedded             fontNotEmbedded;              /*!< Font      Wizard Check NotEmbedded            */
	struct PTB_PRCWC_FontEmbedded                fontEmbedded;                 /*!< Font      Wizard Check Embedded               */
	struct PTB_PRCWC_FontType1                   fontType1;                    /*!< Font      Wizard Check Type1                  */
	struct PTB_PRCWC_FontTrueType                fontTrueType;                 /*!< Font      Wizard Check TrueType               */
	struct PTB_PRCWC_FontType1CID                fontType1CID;                 /*!< Font      Wizard Check Type1CID               */
	struct PTB_PRCWC_FontTrueTypeCID             fontTrueTypeCID;              /*!< Font      Wizard Check TrueTypeCID            */
	struct PTB_PRCWC_FontType3                   fontType3;                    /*!< Font      Wizard Check Type3                  */
	struct PTB_PRCWC_FontOpenType                fontOpenType;                 /*!< Font      Wizard Check OpenType               */
	struct PTB_PRCWC_FontIsUsed                  fontIsUsed;                   /*!< Font      Wizard Check IsUsed                 */
	struct PTB_PRCWC_RenderingTransparency       renderingTransparency;        /*!< Rendering Wizard Check Transparency           */
	struct PTB_PRCWC_RenderingHalftone           renderingHalftone;            /*!< Rendering Wizard Check Halftone               */
	struct PTB_PRCWC_RenderingCurve              renderingCurve;               /*!< Rendering Wizard Check Curve                  */
	struct PTB_PRCWC_RenderingThickness          renderingThickness;           /*!< Rendering Wizard Check Thickness              */
	struct PTB_PRCWC_RenderingPostscript         renderingPostscript;          /*!< Rendering Wizard Check Postscript             */
	struct PTB_PRCWC_PDFXADocumentPDFX           pdfxaDocumentPDFX;            /*!< Standards Wizard Check DocumentPDFX           */
	struct PTB_PRCWC_PDFXADocumentPDFA           pdfxaDocumentPDFA;            /*!< Standards Wizard Check DocumentPDFA           */
	struct PTB_PRCWC_PDFXADocumentPDFE           pdfxaDocumentPDFE;            /*!< Standards Wizard Check DocumentPDFE           */
	struct PTB_PRCWC_PDFXADocumentPDFUA          pdfxaDocumentPDFUA;           /*!< Standards Wizard Check DocumentPDFUA          */
	struct PTB_PRCWC_PDFXADocumentPDFVT          pdfxaDocumentPDFVT;           /*!< Standards Wizard Check DocumentPDFVT          */
	struct PTB_PRCWC_PDFXADocumentRGB            pdfxaDocumentRGB;             /*!< Standards Wizard Check DocumentRGB            */
	struct PTB_PRCWC_PDFXAOutputCondition        pdfxaOutputCondition;         /*!< Standards Wizard Check OutputCondition        */
};

/******************************************************************************/
/* MetaProfile data */

/*! MetaProfile data
*/
typedef struct {
	PTB_size_t                        size;     /*!< must be sizeof(PTB_PRCMPrfData)     */
	const PTB_utf8_char_t*            name;     /*!< Name                                      */
	const PTB_utf8_char_t*            comment;  /*!< Comment                                   */
	const PTB_sys_char_t *            dictkey;  /*!< Dict key
	                                                   \note  Either dictkey or name and comment
	                                                          must be NULL                       */
} PTB_PRCMPrfData;

/******************************************************************************/
/* ProfGroup data */

/*! ProfGroup data */
typedef struct {
	PTB_size_t                        size;     /*!< must be sizeof(PTB_PRCPGrpData) */
	const PTB_utf8_char_t*            name;     /*!< Name                              */
	const PTB_utf8_char_t*            comment;  /*!< Comment                           */
	const PTB_sys_char_t *            dictkey;  /*!< Dict key
	                                                   \note  Either dictkey or name and
	                                                          comment must be NULL       */
} PTB_PRCPGrpData;

/******************************************************************************/
/* Child attributes */

/*! Rule-Condition child attributes */
typedef struct {
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCRuleCondAttr)   */
	PTB_PRCMetaData*                  metaDataList;  /*!< Metadata list                         */
	PTB_size_t                        metaDataSize;  /*!< Number of objects in metadata list    */
} PTB_PRCRuleCondAttr;

/*! RuleSet-Rule child attributes */
typedef struct {
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCRSetRuleAttr)   */
	PTB_PRCMetaData*                  metaDataList;  /*!< Metadata list                         */
	PTB_size_t                        metaDataSize;  /*!< Number of objects in metadata list    */
	enum PTB_PRCERuleCheckSeverity    checkSeverity; /*!< Rule's Check Severity                 */
} PTB_PRCRSetRuleAttr;

/*! FixupSet-Fixup child attributes */
typedef struct {
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCFSetFxupAttr)   */
	PTB_PRCMetaData*                  metaDataList;  /*!< Metadata list                         */
	PTB_size_t                        metaDataSize;  /*!< Number of objects in metadata list    */
} PTB_PRCFSetFxupAttr;

/*! Profile-RuleSet child attributes */
typedef struct {
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCProfRSetAttr)   */
	PTB_PRCMetaData*                  metaDataList;  /*!< Metadata list                         */
	PTB_size_t                        metaDataSize;  /*!< Number of objects in metadata list    */
} PTB_PRCProfRSetAttr;

/*! Profile-FixupSet child attributes */
typedef struct {
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCProfFSetAttr)   */
	PTB_PRCMetaData*                  metaDataList;  /*!< Metadata list                         */
	PTB_size_t                        metaDataSize;  /*!< Number of objects in metadata list    */
} PTB_PRCProfFSetAttr;

/*! ProfGroup-Profile child attributes */
typedef struct {
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCPGrpProfAttr)   */
	PTB_PRCMetaData*                  metaDataList;  /*!< Metadata list                         */
	PTB_size_t                        metaDataSize;  /*!< Number of objects in metadata list    */
} PTB_PRCPGrpProfAttr;


/* Reference attributes */

/*! Fixup-Rule reference attributes */
typedef struct
{
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCFxupRuleAttr)   */
	PTB_PRCMetaData*                  metaDataList;  /*!< Metadata list                         */
	PTB_size_t                        metaDataSize;  /*!< Number of objects in metadata list    */
} PTB_PRCFxupRuleAttr;

/*! Condition-Rule reference attributes */
typedef struct
{
	PTB_size_t                        size;          /*!< must be sizeof(PTB_PRCCondRuleAttr)   */
	PTB_PRCMetaData*                  metaDataList;  /*!< Metadata list                         */
	PTB_size_t                        metaDataSize;  /*!< Number of objects in metadata list    */
} PTB_PRCCondRuleAttr;

/******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif //defined(PTB_PRCTYPES_H)

/******************************************************************************/
/* EOF */
