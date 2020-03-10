/*!
**
** \file ptbPRC.h
**
** \brief callas pdfEngine SDK: PRC API
** 
** This declares the PRC C API (see \ref PRC_Overview)
** 
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/*!
** \page PRC_Overview PRC Overview
** 
** \section PRC_Overview_sec Overview
** 
** This is the API to manipulate callas pdfEngine SDK's Profile Groups,
** Profiles, Rules, Rule Sets, Fixups Sets, Fixups and Conditions. (In Acrobat 
** Professional's Preflight tool they are found in the "Edit Profile" dialog.) 
** For historical reasons ProfGroups, Profiles, Rules, Rule Sets, Fixups,  
** Fixups Sets and Conditions are collectively referred to as PRC Objects. 
** 
** PRC Objects are hold in PRC Engines. Objects in PRC Engines can refer to 
** each other. In this API, PRC Engines and PRC Objects are represented to by 
** IDs. (::PTB_PRCEngineID, ::PTB_PRCCondID, ::PTB_PRCRuleID, 
** ::PTB_PRCRSetID, ::PTB_PRCFxupID, ::PTB_PRCFSetID, ::PTB_PRCProfID,
** ::PTB_PRCPGrpID)
** 
** There is a many-to-many relationship between Profiles and FixupSets, between 
** Profiles and RuleSets, between RuleSets and Rules, between FixupSets and 
** Fixups, and between Rules and Conditions.
** So Preflight Profiles refer to zero or more FixupSets and to zero 
** or more RuleSets. Each of the latter refers to zero or more Rules, each of 
** which refers to zero or more Conditions. Both a FixupSet or a RuleSet can be 
** referred to by zero or more Profiles, a Rule can be referred to by zero or 
** more RuleSets, a Fixup can be referred to by zero or more FixupSets,
** a Condition can be referred to by zero or more Rules. 
** 
** Preflight Profiles can be executed. This means that callas pdfEngine SDK (or 
** Acrobat's Preflight tool) analyzes all its referenced child objects and 
** checks a PDF document accordingly. 
** 
** \section PRC_WizardChecks_sec Wizard Checks
** 
** It is, however, possible to create Profiles which do not refer to any child 
** objects and still contain information as to what should be checked in a PDF 
** document. One can add so called Wizard Checks to a profile. These parallel 
** the options in Preflight's "Edit Profile" dialog (except for the "Custom 
** Checks" and "Fixups" sections). The callas pdfEngine SDK (or Acrobat's 
** Preflight tool) will, before the execution of such a profile, analyze these 
** Wizard Checks and modify (a copy of) the Profile by adding RuleSets (and, in 
** case of Preflight, Fixups) accordingly. 
** 
** Wizard Checks can either be inactive or trigger an informational, warning, or 
** error message. This is referred to as the Wizard Check's state. A Profile's 
** Wizard Checks states can be checked by calling 
** PTB_PRCGetProfileWizardCheckState(). 
** Some Wizard Checks have additional data which can be checked by calling 
** PTB_PRCGetProfileWizardCheckStateData(). 
** 
** A Profile's Wizard Checks can be set using PTB_PRCSetProfileWizardChecks(). 
** This must be passed a PTB_PRCWizardChecks object. Since this is a huge 
** struct, it can be initialized/cleared using the C API function 
** PTB_PRCClrProfileWizardCheckData(). 
** 
** \section PRC_Packages_sec Exporting and importing Profiles
** 
** In order to transfer PRC Objects between different PRC Engines, they need to 
** be exported into XML data called Packages. These Packages can then be 
** imported into other PRC Engines (PTB_PRCEngineImportPackage(), 
** PTB_PRCEngineImportPackageFromFile()). They can be written to and read 
** from memory or files. Exporting and importing Packages works across all 
** supported platforms. 
** 
** Packages can be created to contain whole PRC Engines 
** (PTB_PRCEngineExportEngine(), PTB_PRCEngineExportEngineToFile()) or only 
** certain Profiles (PTB_PRCEngineExportProfileToFile(), 
** PTB_PRCEngineExportProfile()). In the latter case they will contain all 
** Fixups, RuleSets, Rules and Conditions referred to by the exported Profile. 
** Since Packages also contain other data associated with Preflight Profiles 
** (like Output Intents or Color Profiles), they can become quite large. 
** 
** It is also possible to import the Preflight Profile repositories of Acrobat 
** 6, Acrobat 7, or Acrobat 8 into a PRC Engine created by this API (see 
** PTB_PRCEngineImportAcroRepos()). In order to avoid conflicts, is not 
** possible to write to those repositories. 
** 
** \section PRC_Examination_sec Examining PRC Objects
** 
** The content of a PRC Engine can be examined by calling 
** PTB_PRCEnumConditions() etc.) These functions call the user-supplied 
** callback functions with the IDs of all objects of that type stored in the 
** PRC Engine. The IDs can then be used to retrieve the data for each object 
** (PTB_PRCGetConditionData() etc.). 
** 
** \section PRC_ObjectMan_sec Manipulating PRC Objects
** 
** It is further possible to add new PRC Objects to a PRC Engine 
** (PTB_PRCEngineAddCondition() etc.), remove them 
** (PTB_PRCEngineRemoveCondition() etc.), duplicate them
** (PTB_PRCEngineDuplicateCondition() etc.), or replace their data 
** (PTB_PRCEngineReplaceCondition(), etc.). 
** 
** \section PRC_ObjectRel_sec Object Relationships
** 
** Relationships between PRC Objects can have attributes. The exact types of 
** these attributes depend on the type of relationship. All relationships can 
** have a list of meta data key-value pairs (see \ref PRCMetaData_Intro_sec) 
** associated with. Some relationships, like the one between RuleSets and Rules, 
** have other data as well. 
** 
** Relationships can only exist between PRC Objects that are stored within the 
** same PRC Engine. 
** 
** There are functions to examine the children of an object 
** (PTB_PRCEngineRuleEnumConditions() etc.), to examine 
** (PTB_PRCEngineGetRuleConditionAttributes() etc.) and to change
** (PTB_PRCEngineReplaceRuleConditionAttributes() etc.) relationship 
** attributes . 
** 
** There are also functions to create (PTB_PRCEngineRuleAddChildCondition() 
** etc.) and remove (PTB_PRCEngineRuleRemoveChildCondition() etc.) 
** associations between PRC Objects. 
** 
** \section PRCMetaData_Intro_sec PRC Meta Data
** 
** PRC meta data is a list of key-value pairs, where both the key and the value 
** are strings. Keys need to be unique. Meta data is part of a Profile's data 
** and part of all parent-child relationships. 
** 
** When a function needs to return meta data, it will return an 
** ::PTB_PRCMetaDataID. Users can use this ID to retrieve the number of 
** key-value pairs in the meta data list (PTB_PRCGetMetaDataLength()), and 
** to retrieve individual keys (PTB_PRCGetMetaDataKey()) and values 
** (PTB_PRCGetMetaDataValue()). Meta data IDs need to be released using 
** PTB_PRCMetaDataRelease(). 
** 
** For a Profile's meta data, there are three keys which have a predefined 
** meaning: ::PTB_PRC_PROFILE_METAKEY_AUTHOR, ::PTB_PRC_PROFILE_METAKEY_EMAIL, 
** and ::PTB_PRC_PROFILE_METAKEY_RATING. These refer to the corresponding GUI 
** features in Acrobat's Preflight plugin. 
** 
*/

/******************************************************************************/

#if !defined(PTB_PRC_H)
/*! \def PTB_PRC_H header define */
#define PTB_PRC_H

/******************************************************************************/

#include "ptbTypes.h"
#include "ptbPRCTypes.h"
#include "ptbProgress.h"

/******************************************************************************/

#include "ptbProlog.h"

/******************************************************************************/
/*
********************************************************************************
** Basic PRC stuff
********************************************************************************
*/

/******************************************************************************/
/* Validating IDs */

/*! 
** \brief  Test IDs 
** 
**         Tests whether a meta data ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidMetaDataID(PTB_PRCMetaDataID id  /*!< Meta data ID to check */
                        );

/*! 
** \brief  Test IDs 
** 
**         Tests whether a PRC Engine ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidEngineID(PTB_PRCEngineID  /*!< ID of PRC Engine to check */
                      );

/*! 
** \brief  Test IDs 
** 
**         Tests whether a Condition ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidCondID(PTB_PRCCondID  /*!< ID of Condition to check */
                    );

/*! 
** \brief  Test IDs 
** 
**         Tests whether a Rule ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidRuleID(PTB_PRCRuleID  /*!< ID of Rule to check */
                    );

/*! 
** \brief  Test IDs 
** 
**         Tests whether a RuleSet ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidRSetID(PTB_PRCRSetID  /*!< ID of RuleSet to check */
                    );

/*! 
** \brief  Test IDs 
** 
**         Tests whether a Fixup ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidFxupID(PTB_PRCFxupID  /*!< ID of Fixup to check */
                    );


/*! 
** \brief  Test IDs 
** 
**         Tests whether a FixupSet ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidFSetID(PTB_PRCFSetID  /*!< ID of FixupSet to check */
                    );

/*! 
** \brief  Test IDs 
** 
**         Tests whether a Action ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidActnID(PTB_PRCActnID  /*!< ID of Action to check */
                    );

/*! 
** \brief  Test IDs 
** 
**         Tests whether a Profile ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidProfID(PTB_PRCProfID  /*!< ID of Profile to check */
                    );

/*! 
** \brief  Test IDs 
** 
**         Tests whether a MetaProfile ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidMPrfID(PTB_PRCMPrfID  /*!< ID of MetaProfile to check */
                    );

/*! 
** \brief  Test IDs 
** 
**         Tests whether a Profile Group ID is valid.
** 
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_PRCIsValidPGrpID(PTB_PRCPGrpID  /*!< ID of ProfGroup to check */
                    );

/*
********************************************************************************
** Dict keys
********************************************************************************
*/

/*! 
** \brief  Get object's name from dict key
** 
**         Returns the object's name
** 
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidDictkey  Invalid dict key
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetObjectNameFromDictKey( const PTB_sys_char_t* dictkey  /*!< Dict key to look up */
                               , PTB_StringID*         idStr    /*!< Resulting string ID */
                               );

/*! 
** \brief  Get object's comment from dict key
** 
**         Returns the object's comment
** 
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidDictkey  Invalid dict key
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetObjectCommentFromDictKey( const PTB_sys_char_t* dictkey  /*!< Dict key to look up */
                                  , PTB_StringID*         idStr    /*!< Resulting string ID */
                                  );

/*! 
** \brief  Get dict key from object's name
** 
**         Returns the dict key
** 
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidDictkey  Name is from dict
** 
** \note   The resulting string will be in system encoding.
*/
/*@{*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetDictKeyFromConditionName  ( const PTB_utf8_char_t* name   /*!< Name to look up    */
                                    , PTB_StringID*          idStr  /*!< Resulting dict key */
                                    );
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetDictKeyFromRuleName       ( const PTB_utf8_char_t* name   /*!< Name to look up    */
                                    , PTB_StringID*          idStr  /*!< Resulting dict key */
                                    );
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetDictKeyFromRuleSetName    ( const PTB_utf8_char_t* name   /*!< Name to look up    */
                                    , PTB_StringID*          idStr  /*!< Resulting dict key */
                                    );
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetDictKeyFromFixupName      ( const PTB_utf8_char_t* name   /*!< Name to look up    */
                                    , PTB_StringID*          idStr  /*!< Resulting dict key */
                                    );
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetDictKeyFromFixupSetName   ( const PTB_utf8_char_t* name   /*!< Name to look up    */
                                    , PTB_StringID*          idStr  /*!< Resulting dict key */
                                    );
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetDictKeyFromActionName     ( const PTB_utf8_char_t* name   /*!< Name to look up    */
                                    , PTB_StringID*          idStr  /*!< Resulting dict key */
                                    );
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetDictKeyFromProfileName    ( const PTB_utf8_char_t* name   /*!< Name to look up    */
                                    , PTB_StringID*          idStr  /*!< Resulting dict key */
                                    );
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetDictKeyFromMetaProfileName( const PTB_utf8_char_t* name   /*!< Name to look up    */
                                    , PTB_StringID*          idStr  /*!< Resulting dict key */
                                    );
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetDictKeyFromProfGroupName  ( const PTB_utf8_char_t* name   /*!< Name to look up    */
                                    , PTB_StringID*          idStr  /*!< Resulting dict key */
                                    );
/*}@*/

/*
********************************************************************************
** Meta data
********************************************************************************
*/

/*! 
** \brief  Get meta data size
** 
**         Returns the number of entries in the meta data map with the ID id
** 
** \retval                 PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidMetaDataID  Invalid meta data ID
** 
** \relatesalso PTB_PRCMetaData
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetMetaDataLength( PTB_PRCMetaDataID id    /*!< Meta data ID    */
                        , PTB_size_t*       size  /*!< Meta data size  */
                        );

/*! 
** \brief  Get meta data key
** 
**         Returns the key with the number n in the meta data map with the ID id
** 
** \retval                  PTB_eerrNone  No error
** \retval  PTB_eerrPRCInvalidMetaDataID  Invalid meta data ID
** \retval PTB_eerrPRCInvalidMetaDataIdx  Invalid meta data index
** \note   The String ID returned by this function needs to get released by calling PTB_StringRelease()
** 
** \relatesalso PTB_PRCMetaData
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetMetaDataKey( PTB_PRCMetaDataID idMeta  /*!< Meta data ID                */
                     , PTB_size_t        index   /*!< Meta data item index        */
                     , PTB_StringID*     idStr   /*!< String ID for meta data key */
                     );

/*! 
** \brief  Get meta data key
** 
**         Returns the value with the number n in the meta data map with the ID id
** 
** \retval                  PTB_eerrNone  No error
** \retval  PTB_eerrPRCInvalidMetaDataID  Invalid meta data ID
** \retval PTB_eerrPRCInvalidMetaDataIdx  Invalid meta data index
** \note   The String ID returned by this function needs to get released by calling PTB_StringRelease()
** 
** \relatesalso PTB_PRCMetaData
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetMetaDataValue( PTB_PRCMetaDataID idMeta  /*!< Meta data ID                  */
                       , PTB_size_t        index   /*!< Meta data item index          */
                       , PTB_StringID*     idStr   /*!< String ID for meta data value */
                       );

/*! 
** \brief  Release meta data
** 
**         Releases the meta data with the ID id
** 
** \retval            PTB_eerrNone  No error
** \retval PTB_eerrInvalidStringID  Invalid meta data ID
** 
** \relatesalso PTB_PRCMetaData
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCMetaDataRelease( PTB_PRCMetaDataID id  /*!< String ID */
                      );

/*
********************************************************************************
** PRC Engines and Packages
********************************************************************************
*/

/******************************************************************************/
/* PRC Engine creation, disposal, saving*/

/*! 
** \brief  Create empty PRC Engine 
** 
**         Creates an empty PRC Engine
** 
** \note   Use PRCDisposePRCEngine() to release this Engine.
** 
** \retval PTB_eerrNone  No error
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineCreate( PTB_PRCEngineID* idEng /*!< Resulting Engine ID */
                   );

/*! 
** \brief  Release Engine 
** 
**         Releases an instance of the PRC Engine
** 
** \note   Use only for PRC Engines created using PRCCreatePRCEngine().
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  Engine ID invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineDispose( PTB_PRCEngineID idEng  /*!< ID of Engine to release */
                    );

/******************************************************************************/
/* Importing and exporting */

/*! 
** \brief  Imports a profile from an XML package in memory
** 
**         Imports all objects from a buffer containing the XML package
**         representation of a profile.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineImportPackage( PTB_PRCEngineID         idEng      /*!< ID of PRC Engine to import package into             */
                          , const PTB_utf8_char_t*  xml        /*!< Buffer containing the XML data to read package from */
                          , PTB_PRC_ProgressCB      cb         /*!< Progress callback                                   */
                          , void*                   userData   /*!< User data                                           */
                          );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineImportPackage2( PTB_PRCEngineID         idEng      /*!< ID of PRC Engine to import package into             */
                           , const PTB_utf8_char_t*  xml        /*!< Buffer containing the XML data to read package from */
                           , PTB_bool_t              ignoreHash /*!< Ignore hash values in packages                      */
						   , PTB_PRC_ProgressCB      cb         /*!< Progress callback                                   */
                           , void*                   userData   /*!< User data                                           */
                           );

/*! 
** \brief  Imports a profile from an XML package in a file
** 
**         Imports all objects from a file containing the XML package
**         representation of a profile.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval     PTB_eerrFileSourceRead  Cannot read file
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineImportPackageFromFile( PTB_PRCEngineID         idEng      /*!< ID of PRC Engine to import package into  */
                                  , const PTB_Path_t*       file       /*!< File name                                */
                                  , PTB_PRC_ProgressCB      cb         /*!< Progress callback                        */
                                  , void*                   userData   /*!< User data                                */
                                  );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineImportPackageFromFile2( PTB_PRCEngineID         idEng      /*!< ID of PRC Engine to import package into  */
                                   , const PTB_Path_t*       file       /*!< File name                                */
                                   , PTB_bool_t              ignoreHash /*!< Ignore hash values in packages           */
                                   , PTB_PRC_ProgressCB      cb         /*!< Progress callback                        */
                                   , void*                   userData   /*!< User data                                */
                                   );

// #ifndef unix
/*! 
** \brief  Import Acrobat's repository
** 
** \retval                PTB_eerrNone  No error
** \retval  PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval     PTB_eerrPRCNoRepository  Requested repository does not exist
** \retval PTB_eerrPRCDefectRepository  Repository defect, only partially loaded
** 
** \note   The parameter \a acrover is ignored if this is the preflight plugin,
**         called via HFT. The plugin will always only refer to its own 
**         repository. 
** \note   Saving to a PRC engine repository that belongs to Preflight/Acrobat 
**         Professional is not supported by the callas pdfEngine SDK.
** \note   This requires that the Acrobat version named in the parameter 
**         \a acrover is installed and the Preflight tool of it was already 
**         used at least once. 
**         Note that an empty repository is a valid repository. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineImportAcroRepos( PTB_PRCEngineID              idEng      /*!< ID of PRC Engine to import repository into */
                            , enum PTB_EAcrobatVersion     acrover    /*!< Acrobat version to import repository from  */
                            , PTB_PRC_ProgressCB           cb         /*!< progress callback                          */
                            , void*                        userData   /*!< user data                                  */
                            );

/*! 
** \brief  Import an repository from the Acrobat 8 preferences of a user
** 
** \retval                PTB_eerrNone  No error
** \retval  PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval     PTB_eerrPRCNoRepository  Requested repository does not exist
** \retval PTB_eerrPRCDefectRepository  Repository defect, only partially loaded
** 
** \note   This requires that the Acrobat 8 is installed and the Preflight tool 
**         of it was already used at least once by the user who's repository 
**         is to be imported. 
**         Note that an empty repository is a valid repository. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineImportAcroReposFromUser( PTB_PRCEngineID         idEng            /*!< ID of PRC Engine to import repository into                */
                                    , const PTB_Path_t*       userPrefFolder   /*!< user's application data folder ("<user>\Application data" */
                                    , PTB_PRC_ProgressCB      cb               /*!< progress callback                                         */
                                    , void*                   userData         /*!< user data                                                 */
                                    );

/*! 
** \brief  Import an repository from the Acrobat preferences of a user
** 
** \retval                PTB_eerrNone  No error
** \retval  PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval     PTB_eerrPRCNoRepository  Requested repository does not exist
** \retval PTB_eerrPRCDefectRepository  Repository defect, only partially loaded
** 
** \note   This requires that the Acrobat version named in the parameter 
**         \a acrover is installed and the Preflight tool of it was already 
**         used at least once. 
**         Note that an empty repository is a valid repository. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineImportAcroReposFromUser2( PTB_PRCEngineID          idEng            /*!< ID of PRC Engine to import repository into                */
                                     , const PTB_Path_t*        userPrefFolder   /*!< user's application data folder ("<user>\Application data" */
                                     , enum PTB_EAcrobatVersion acrover          /*!< Acrobat version to import repository from                 */
                                     , PTB_PRC_ProgressCB       cb               /*!< progress callback                                         */
                                     , void*                    userData         /*!< user data                                                 */
                                     );

/******************************************************************************
** Acrobat DC specifc method related to mutiple Libraries since Acrobat DC 2017
*******************************************************************************
*/

typedef void(*PTB_PRCEnumAcroReproCB)( PTB_StringID    idName    /*!< Name of found Library */
                                     , void*           userData  /*!< User data             */
                                     );

PTB_FUNC_PROTO
PTB_EError
PTB_PRCEngineEnumAcroRepos( const PTB_Path_t*            userPrefFolder  /*!< NULL for default or user's application data folder ("<user>\Application data" or "Application Support") */
                          , enum PTB_EAcrobatVersion     acrover         /*!< Acrobat version to import repository from  */
                          , PTB_PRCEnumAcroReproCB       cb              /*!< name callback                              */
                          , void*                        userData        /*!< user data                                  */
                          );

/*!
** \brief  Import Acrobat DC repository by name
**
** \retval                PTB_eerrNone  No error
** \retval  PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval     PTB_eerrPRCNoRepository  Requested repository does not exist
** \retval PTB_eerrPRCDefectRepository  Repository defect, only partially loaded
**
** \note   The parameter \a acrover is ignored if this is the preflight plugin,
**         called via HFT. The plugin will always only refer to its own
**         repository.
** \note   Saving to a PRC engine repository that belongs to Preflight/Acrobat
**         Professional is not supported by the pdfEngine SDK.
** \note   This requires that the Acrobat version named in the parameter
**         \a acrover is installed and the Preflight tool of it was already
**         used at least once.
**         Note that an empty repository is a valid repository.
*/
PTB_FUNC_PROTO
PTB_EError
PTB_PRCEngineImportAcroRepoWithName( PTB_PRCEngineID              idEng           /*!< ID of PRC Engine to import repository into */
                                   , const PTB_Path_t*            userPrefFolder  /*!< NULL for default or user's application data folder ("<user>\Application data" or "Application Support") */
                                   , enum PTB_EAcrobatVersion     acrover         /*!< Acrobat version to import repository from  */
                                   , const PTB_utf8_char_t*       name            /*!< Library name to import repository from     */
                                   , PTB_PRC_ProgressCB           cb              /*!< progress callback                          */
                                   , void*                        userData        /*!< user data                                  */
                                   );

// #endif

/*! 
** \brief  Exports a profile into an XML package in memory
** 
**         Exports a profile and all objects referenced by it as an XML 
**         package into a buffer.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineExportProfile( PTB_PRCEngineID idEng    /*!< ID of PRC Engine to export package from */
                          , PTB_PRCProfID   idProf   /*!< ID of Profile to export                 */
                          , PTB_StringID*   idStr    /*!< String data                             */
                          );

/*! 
** \brief  Exports a profile into an XML package in a file
** 
**         Exports a profile and all objects referenced by it as an XML 
**         package to a file.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval      PTB_eerrFileDestWrite  Cannot write file
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineExportProfileToFile( PTB_PRCEngineID   idEng    /*!< ID of PRC Engine to export package from */
                                , PTB_PRCProfID     idProf   /*!< ID of Profile to export                 */
                                , const PTB_Path_t* file     /*!< File name                               */
                                );

/*! 
** \brief  Explode a profile into an XML package in a file with external binary data
** 
**         Exports a profile and all objects referenced by it as an XML 
**         package to a file and will include relative paths to the binary data
**         into an existing folder.
** 
** \retval                PTB_eerrNone  No error
** \retval  PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval    PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval PTB_eerrFileDestFolderExist  Dest folder does not exist
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineExplodeProfileToFolder( PTB_PRCEngineID   idEng    /*!< ID of PRC Engine to export package from */
                                   , PTB_PRCProfID     idProf   /*!< ID of Profile to export                 */
                                   , const PTB_Path_t* folder   /*!< Folder                                  */
                                   );

/*! 
** \brief  Exports a PRC Engine into an XML package in memory
** 
**         Exports all objects from a PRC Engine it as an XML package into a 
**         buffer.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineExportEngine( PTB_PRCEngineID idEng   /*!< ID of PRC Engine to export package from */
                         , PTB_StringID*   idStr   /*!< String data                             */
                         );

/*! 
** \brief  Exports a PRC Engine into an XML package in a file
** 
**         Exports all objects from a PRC Engine as an XML package to a file.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval      PTB_eerrFileDestWrite  Cannot write file
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineExportEngineToFile( PTB_PRCEngineID   idEng  /*!< ID of PRC Engine to export package from */
                               , const PTB_Path_t* file   /*!< File name                               */
                               );

/*! 
** \brief  Explode a PRC Engine into an XML package in a file with external binary data
** 
**         Exports all objects from a PRC Engine as an XML package to a file
**         and will include relative paths to the binary data into an existing folder.
** 
** \retval                PTB_eerrNone     No error
** \retval  PTB_eerrPRCInvalidEngineID     ID of PRC Engine invalid
** \retval PTB_eerrFileDestFolderExist     Dest folder does not exist
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineExplodeEngineToFolder( PTB_PRCEngineID   idEng  /*!< ID of PRC Engine to export package from */
                                  , const PTB_Path_t* folder /*!< Folder                                  */
                                  );

/*!
** note: for maximum flexibility the repairHash functionality is available indirectly: 
**
**    1st - import the profile with ignoreHash=true
**    2nd - export the profile
*/

/*
********************************************************************************
** Accessing PRC objects in PRC Engines
********************************************************************************
*/

/******************************************************************************/
/* Get execution objects (Profile or MetaProfile */

/*!
** \brief  Get execution Profile
**
**         Gets the Profile from a PRC Engine that can be used for execution
**
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_PRCGetExecProfile( PTB_PRCEngineID         idEng    /*!< ID of PRC Engine to get Profile data from    */
                     , PTB_PRCProfID *         idProf   /*!< ID of Profile that can be used for execution */
                     );

/*!
** \brief  Get execution MetaProfile
**
**         Gets the MetaProfile from a PRC Engine that can be used for execution
**
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_PRCGetExecMetaProfile( PTB_PRCEngineID         idEng    /*!< ID of PRC Engine to get Profile data from    */
                         , PTB_PRCMPrfID *         idMPrf   /*!< ID of MetaProfile that can be used for execution */
                         );

/******************************************************************************/
/* Enumerating objects */

/*! 
** \brief  Enumerate Conditions
** 
**         Enumerates all Conditions in a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEnumConditions( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine for which Conditions are to be enumerated */
                     , PTB_PRCEnumCondCallback cb        /*!< Enumeration callback                                       */
                     , void*                   userData  /*!< User data                                                  */
                       );

/*! 
** \brief  Enumerate Rules
** 
**         Enumerates all Rules in a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError
PTB_PRCEnumRules( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine for which Rules are to be enumerated */
                , PTB_PRCEnumRuleCallback cb        /*!< Enumeration callback                                  */
                , void*                   userData  /*!< User data                                             */
                );

/*! 
** \brief  Enumerate RuleSets
** 
**         Enumerates all RuleSets in a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError
PTB_PRCEnumRuleSets( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine for which RuleSets are to be enumerated */
                   , PTB_PRCEnumRSetCallback cb        /*!< Enumeration callback                                     */
                   , void*                   userData  /*!< User data                                                */
                   );

/*! 
** \brief  Enumerate Fixups
** 
**         Enumerates all Fixups in a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError
PTB_PRCEnumFixups( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine for which Fixups are to be enumerated   */
                 , PTB_PRCEnumFxupCallback cb        /*!< Enumeration callback                                     */
                 , void*                   userData  /*!< User data                                                */
                 );

/*! 
** \brief  Enumerate FixupSets
** 
**         Enumerates all FixupSets in a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError
PTB_PRCEnumFixupSets( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine for which FixupSets are to be enumerated */
                    , PTB_PRCEnumFSetCallback cb        /*!< Enumeration callback                                      */
                    , void*                   userData  /*!< User data                                                 */
                    );

/*! 
** \brief  Enumerate Actions
** 
**         Enumerates all Actions in a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEnumActions( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine for which Actions are to be enumerated */
                  , PTB_PRCEnumActnCallback cb        /*!< Enumeration callback                                     */
                  , void*                   userData  /*!< User data                                                */
                  );

/*! 
** \brief  Enumerate Profiles
** 
**         Enumerates all Profiles in a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEnumProfiles( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine for which Profiles are to be enumerated */
                   , PTB_PRCEnumProfCallback cb        /*!< Enumeration callback                                     */
                   , void*                   userData  /*!< User data                                                */
                   );

/*! 
** \brief  Enumerate MetaProfiles
** 
**         Enumerates all MetaProfiles in a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEnumMetaProfiles( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine for which MetaProfiles are to be enumerated */
                       , PTB_PRCEnumMPrfCallback cb        /*!< Enumeration callback                                         */
                       , void*                   userData  /*!< User data                                                    */
                       );

/*! 
** \brief  Enumerate ProfGroups
** 
**         Enumerates all ProfGroups in a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEnumProfGroups( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine for which ProfGroups are to be enumerated */
                     , PTB_PRCEnumPGrpCallback cb        /*!< Enumeration callback                                       */
                     , void*                   userData  /*!< User data                                                  */
                     );

/******************************************************************************/
/* Accessing object data */

/*! 
** \brief  Get Condition data
** 
**         Get a Condition's data
** \note   Possible values for \a edt are: 
**         PTB_PRCEdtName, PTB_PRCEdtComment
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**         PTB_PRCEdtDictKey
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          - The resulting string will be in system encoding
**         PTB_PRCEdtCondGroup, PTB_PRCEdtCondProperty, PTB_PRCEdtCondOperator
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**         PTB_PRCEdtCondDataType
**          - intData must point to an int for the function to write the 
**            result to
**          - idStr must be NULL
**         PTB_PRCEdtCondText
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          PTB_PRCEdtCondNumText, PTB_PRCEdtCondNumTolerance
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          PTB_PRCEdtCondRectWidth, PTB_PRCEdtCondRectHeight, PTB_PRCEdtCondRectTolerance
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          PTB_PRCEdtCondRectIgnorOr
**          - intData must point to an int
**          - idStr must be NULL
**          PTB_PRCEdtCondListSize
**          - intData must point to an int for the function to write the 
**            result to
**          - idStr must be NULL
**          PTB_PRCEdtCondListItem
**          - intData must point to an int containing index of the string to 
**            be retrieved; its value can be (0 <= index < list size)
**          - idStr must point to a string ID for the function to write the 
**            result to
**          PTB_PRCEdtCondJobOptionText
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          PTB_PRCEdtCondJobOptionData
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidCondID  ID of Condition invalid
** \retval PTB_eerrPRCInvalidDataType  Data type invalid for this object type
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetConditionData( PTB_PRCEngineID       idEng       /*!< ID of PRC Engine to get Condition data from */
                       , PTB_PRCCondID         idCond      /*!< ID of Condition to get data from            */
                       , enum PTB_PRCEDataType edt         /*!< Data type to retrieve                       */
                       , PTB_int32_t*          intData     /*!< Pointer to integer data                     */
                       , PTB_StringID*         idStr       /*!< String data                                 */
                       );

/*!
** \brief  Get Rule data
** 
**         Get a Rule's data
** \note   Possible values for \a edt are: 
**         PTB_PRCEdtName, PTB_PRCEdtComment
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**         PTB_PRCEdtDictKey
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          - The resulting string will be in system encoding
**         PTB_PRCEdtRuleAnalysisCont
**          - intData must point to an int for the function to write the 
**            result to
**          - idStr must be NULL
**         PTB_PRCEdtRuleConditionLogic
**          - intData must point to an int for the function to write the 
**            result to
**          - idStr must be NULL
**         PTB_PRCEdtRuleReportNoMatch
**          - intData must point to an int for the function to write the 
**            result to
**          - idStr must be NULL
**         PTB_PRCEdtRuleNoMatchString
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
** \retval PTB_eerrPRCInvalidDataType  Data type invalid for this object type
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetRuleData( PTB_PRCEngineID       idEng    /*!< ID of PRC Engine to get Rule data from */
                  , PTB_PRCRuleID         idRule   /*!< ID of Rule to get data from            */
                  , enum PTB_PRCEDataType edt      /*!< Data type to retrieve                  */
                  , PTB_int32_t*          intData  /*!< Pointer to integer data                */
                  , PTB_StringID*         idStr    /*!< String data                            */
                  );

/*!
** \brief  Get RuleSet data
** 
**         Get a RuleSet's data
** \note   Possible values for \a edt are: 
**         PTB_PRCEdtName, PTB_PRCEdtComment
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**         PTB_PRCEdtDictKey
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          - The resulting string will be in system encoding
**         PTB_PRCEdtRSetDefaultFlag
**          - intData must point to an int for the function to write the 
**            result to
**          - idStr must be NULL
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
** \retval PTB_eerrPRCInvalidDataType  Data type invalid for this object type
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetRuleSetData( PTB_PRCEngineID       idEng    /*!< ID of PRC Engine to get RuleSet data from */
                     , PTB_PRCRSetID         idRSet   /*!< ID of RuleSet to get data from            */
                     , enum PTB_PRCEDataType edt      /*!< Data type to retrieve                     */
                     , PTB_int32_t*          intData  /*!< Pointer to integer data                   */
                     , PTB_StringID*         idStr    /*!< String data                               */
                     );

/*!
** \brief  Get Fixup data
** 
**         Get a Fixup's data
** \note   Possible values for \a edt are: 
**         PTB_PRCEdtName, PTB_PRCEdtComment
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**         PTB_PRCEdtDictKey
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          - The resulting string will be in system encoding
**         PTB_PRCEdtFxupConfigString
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFxupID  ID of Fixup invalid
** \retval PTB_eerrPRCInvalidDataType  Data type invalid for this object type
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetFixupData( PTB_PRCEngineID       idEng    /*!< ID of PRC Engine to get Fixup data from    */
                   , PTB_PRCFxupID         idFxup   /*!< ID of Fixup to get data from               */
                   , enum PTB_PRCEDataType edt      /*!< Data type to retrieve                      */
                   , PTB_int32_t*          intData  /*!< Pointer to integer data (currently unused) */
                   , PTB_StringID*         idStr    /*!< String data                                */
                   );

/*!
** \brief  Get FixupSet data
** 
**         Get a FixupSet's data
** \note   Possible values for \a edt are: 
**         PTB_PRCEdtName, PTB_PRCEdtComment
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**         PTB_PRCEdtDictKey
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          - The resulting string will be in system encoding
**         PTB_PRCEdtFSetDefaultFlag
**          - intData must point to an int for the function to write the 
**            result to
**          - idStr must be NULL
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
** \retval PTB_eerrPRCInvalidDataType  Data type invalid for this object type
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetFixupSetData( PTB_PRCEngineID       idEng    /*!< ID of PRC Engine to get FixupSet data from */
                      , PTB_PRCFSetID         idRSet   /*!< ID of FixupSet to get data from            */
                      , enum PTB_PRCEDataType edt      /*!< Data type to retrieve                      */
                      , PTB_int32_t*          intData  /*!< Pointer to integer data                    */
                      , PTB_StringID*         idStr    /*!< String data                                */
                      );

/*!
** \brief  Get Action data
** 
**         Get a Action's data
** \note   Possible values for \a edt are: 
**         PTB_PRCEdtName, PTB_PRCEdtComment
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**         PTB_PRCEdtDictKey
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          - The resulting string will be in system encoding
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidActnID  ID of Action invalid
** \retval PTB_eerrPRCInvalidDataType  Data type invalid for this object type
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetActionData( PTB_PRCEngineID  idEng    /*!< ID of PRC Engine to get Action data from   */
                    , PTB_PRCActnID    idActn   /*!< ID of Action to get data from              */
                    , PTB_PRCEDataType edt      /*!< Data type to retrieve                      */
                    , PTB_int32_t*     intData  /*!< Pointer to integer data                    */
                    , PTB_StringID*    idStr    /*!< String data                                */
                    );

/*! 
** \brief  Get Profile data
** 
**         Get a Profile's data
** \note   Possible values for \a edt are: 
**         PTB_PRCEdtName, PTB_PRCEdtComment
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**         PTB_PRCEdtDictKey
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          - The resulting string will be in system encoding
**         PTB_PRCEdtProfPDFXAConv
**          - intData must point to an int for the function to write the 
**            PDF/X and PDF/A state flags to
**          - idStr must point to a string ID for the function to write the 
**            output intent to
**         PTB_PRCEdtProfMetaData
**          - intData must point to a PTB_PRCMetaDataID for the function to 
**            return the Profile's meta data
**          - idStr must be NULL
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval PTB_eerrPRCInvalidDataType  Data type invalid for this object type
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetProfileData( PTB_PRCEngineID         idEng    /*!< ID of PRC Engine to get Profile data from */
                     , PTB_PRCProfID           idProf   /*!< ID of Profile to get data from            */
                     , enum PTB_PRCEDataType   edt      /*!< Data type to retrieve                     */
                     , PTB_int32_t*            intData  /*!< Pointer to integer data                   */
                     , PTB_StringID*           idStr    /*!< String data                               */
                     );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetProfileEditFlag( PTB_PRCEngineID    idEng     /*!< ID of PRC Engine to get Profile data from */
                         , PTB_PRCProfID      idProf    /*!< ID of Profile to get data from            */
                         , PTB_bool_t *       editFlag  /*!< Pointer to integer data                   */
                         );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCSetProfileEditFlag( PTB_PRCEngineID          idEng     /*!< ID of PRC Engine to get Profile data from */
                         , PTB_PRCProfID            idProf    /*!< ID of Profile to get data from            */
                         , const PTB_utf8_char_t *  pwdOld    /*!< Current password, maybe NULL              */
                         , PTB_bool_t               editFlag  /*!< Pointer to integer data                   */
                         );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCSetProfilePassword( PTB_PRCEngineID          idEng   /*!< ID of PRC Engine to get Profile data from                 */
                         , PTB_PRCProfID            idProf  /*!< ID of Profile to get data from                            */
                         , const PTB_utf8_char_t *  pwdOld  /*!< Current password, maybe NULL                              */
                         , const PTB_utf8_char_t *  pwdNew  /*!< New password, NULL in order to remove password protection */
                         );

/*!
** \brief  Get MetaProfile data
** 
**         Get a MetaProfile's data
** \note   Possible values for \a edt are: 
**         PTB_PRCEdtName, PTB_PRCEdtComment
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**         PTB_PRCEdtDictKey
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          - The resulting string will be in system encoding
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidMPrfID  ID of MetaProfile invalid
** \retval PTB_eerrPRCInvalidDataType  Data type invalid for this object type
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetMetaProfileData( PTB_PRCEngineID  idEng    /*!< ID of PRC Engine to get MetaProfile data from */
                         , PTB_PRCMPrfID    idMPrf   /*!< ID of MetaProfile to get data from            */
                         , PTB_PRCEDataType edt      /*!< Data type to retrieve                         */
                         , PTB_int32_t*     intData  /*!< Pointer to integer data                       */
                         , PTB_StringID*    idStr    /*!< String data                                   */
                         );

/*!
** \brief  Get ProfGroup data
** 
**         Get a ProfGroup's data
** \note   Possible values for \a edt are: 
**         PTB_PRCEdtName, PTB_PRCEdtComment
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**         PTB_PRCEdtDictKey
**          - intData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
**          - The resulting string will be in system encoding
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidPGrpID  ID of ProfGroup invalid
** \retval PTB_eerrPRCInvalidDataType  Data type invalid for this object type
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetProfGroupData( PTB_PRCEngineID       idEng    /*!< ID of PRC Engine to get ProfGroup data from */
                       , PTB_PRCPGrpID         idPGrp   /*!< ID of ProfGroup to get data from            */
                       , enum PTB_PRCEDataType edt      /*!< Data type to retrieve                       */
                       , PTB_int32_t*          intData  /*!< Pointer to integer data (currently unused)  */
                       , PTB_StringID*         idStr    /*!< String data                                 */
                       );

/*
********************************************************************************
** Adding, removing, and changing objects in PRC Engines
********************************************************************************
*/

/******************************************************************************/
/* Adding objects */

/*! 
** \brief  Add a Condition
** 
**         Adds a Condition to a PRC Engine.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineAddCondition( PTB_PRCEngineID        idEng   /*!< ID of PRC Engine              */
                         , const PTB_PRCCondData* data    /*!< Condition's data              */
                         , PTB_PRCCondID*         idCond  /*!< ID of newly created Condition */
                         );

/*! 
** \brief  Add a Rule
** 
**         Adds a Rule to a PRC Engine.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineAddRule( PTB_PRCEngineID        idEng   /*!< ID of PRC Engine         */
                    , const PTB_PRCRuleData* data    /*!< Rule's data              */
                    , PTB_PRCRuleID *        idRule  /*!< ID of newly created Rule */
                    );

/*! 
** \brief  Add a RuleSet
** 
**         Adds a RuleSet to a PRC Engine.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineAddRuleSet( PTB_PRCEngineID        idEng   /*!< ID of PRC Engine            */
                       , const PTB_PRCRSetData* data    /*!< RuleSet's data              */
                       , PTB_PRCRSetID *        idRSet  /*!< ID of newly created RuleSet */
                       );

/*! 
** \brief  Add a Fixup
** 
**         Adds a Fixup to a PRC Engine.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineAddFixup( PTB_PRCEngineID         idEng   /*!< ID of PRC Engine          */
                     , const PTB_PRCFxupData*  data    /*!< Fixup's data              */
                     , PTB_PRCFxupID *         idFxup  /*!< ID of newly created Fixup */
                     );

/*! 
** \brief  Add a FixupSet
** 
**         Adds a FixupSet to a PRC Engine.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineAddFixupSet( PTB_PRCEngineID        idEng   /*!< ID of PRC Engine             */
                        , const PTB_PRCFSetData* data    /*!< FixupSet's data              */
                        , PTB_PRCFSetID *        idFSet  /*!< ID of newly created FixupSet */
                        );

/*! 
** \brief  Add a Profile
** 
**         Adds a Profile to a PRC Engine.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineAddProfile( PTB_PRCEngineID        idEng   /*!< ID of PRC Engine            */
                       , const PTB_PRCProfData* data    /*!< Profile's data              */
                       , PTB_PRCProfID *        idProf  /*!< ID of newly created Profile */
                       );

/*! 
** \brief  Add a ProfGroup
** 
**         Adds a ProfGroup to a PRC Engine.
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineAddProfGroup( PTB_PRCEngineID        idEng   /*!< ID of PRC Engine              */
                         , const PTB_PRCPGrpData* data    /*!< ProfGroup's data              */
                         , PTB_PRCPGrpID*         idPGrp  /*!< ID of newly created ProfGroup */
                         );

/******************************************************************************/
/* Removing objects */

/*! 
** \brief  Remove a Condition
** 
**         Removes a Condition from a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidCondID  ID of Condition invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRemoveCondition( PTB_PRCEngineID idEng  /*!< ID of PRC Engine          */
                            , PTB_PRCCondID   id     /*!< ID of Condition to remove */
                            );

/*! 
** \brief  Remove a Rule
** 
**         Removes a Rule from a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRemoveRule( PTB_PRCEngineID idEng  /*!< ID of PRC Engine     */
                       , PTB_PRCRuleID id       /*!< ID of Rule to remove */
                       );

/*! 
** \brief  Remove a RuleSet
** 
**         Removes a RuleSet from a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRemoveRuleSet( PTB_PRCEngineID idEng  /*!< ID of PRC Engine        */
                          , PTB_PRCRSetID id       /*!< ID of RuleSet to remove */
                          );

/*! 
** \brief  Remove a Fixup
** 
**         Removes a Fixup from a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFxupID  ID of Fixup invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRemoveFixup( PTB_PRCEngineID idEng  /*!< ID of PRC Engine      */
                        , PTB_PRCFxupID id       /*!< ID of Fixup to remove */
                        );

/*! 
** \brief  Remove a FixupSet
** 
**         Removes a FixupSet from a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRemoveFixupSet( PTB_PRCEngineID idEng  /*!< ID of PRC Engine         */
                           , PTB_PRCFSetID id       /*!< ID of FixupSet to remove */
                           );

/*! 
** \brief  Remove a Profile
** 
**         Removes a Profile from a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRemoveProfile( PTB_PRCEngineID idEng  /*!< ID of PRC Engine        */
                          , PTB_PRCProfID id       /*!< ID of Profile to remove */
                          );

/*! 
** \brief  Remove a ProfGroup
** 
**         Removes a ProfGroup from a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidPGrpID  ID of ProfGroup invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRemoveProfGroup( PTB_PRCEngineID idEng  /*!< ID of PRC Engine          */
                            , PTB_PRCPGrpID id       /*!< ID of ProfGroup to remove */
                            );

/******************************************************************************/
/* Replacing objects */

/*! 
** \brief  Replace a Condition
** 
**         Replaces a Condition's data in a PRC Engine with other data
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidCondID  ID of Condition invalid
** 
** \note  The new ID will be different if the object needed to be split. This
**        is necessary if it is referenced from several parent objects not all 
**        of which should be altered. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineReplaceCondition( PTB_PRCEngineID        idEng  /*!< ID of PRC Engine                          */
                             , PTB_PRCCondID          id     /*!< ID of Condition for which to replace data */
                             , const PTB_PRCCondData* data   /*!< New data for Condition                    */
                             );

/*! 
** \brief  Replace a Rule
** 
**         Replaces a Rule's data in a PRC Engine with other data
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
** 
** \note  The new ID will be different if the object needed to be split. This
**        is necessary if it is referenced from several parent objects not all 
**        of which should be altered. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineReplaceRule( PTB_PRCEngineID        idEng  /*!< ID of PRC Engine                     */
                        , PTB_PRCRuleID          id     /*!< ID of Rule for which to replace data */
                        , const PTB_PRCRuleData* data   /*!< New data for Rule                    */
                        );

/*! 
** \brief  Replace a RuleSet
** 
**         Replaces a RuleSet's data in a PRC Engine with other data
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
** 
** \note  The new ID will be different if the object needed to be split. This
**        is necessary if it is referenced from several parent objects not all 
**        of which should be altered. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineReplaceRuleSet( PTB_PRCEngineID        idEng  /*!< ID of PRC Engine                        */
                           , PTB_PRCRSetID          id     /*!< ID of RuleSet for which to replace data */
                           , const PTB_PRCRSetData* data   /*!< New data for RuleSet                    */
                           );

/*! 
** \brief  Replace a Fixup
** 
**         Replaces a Fixup's data in a PRC Engine with other data
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFxupID  ID of Fixup invalid
** 
** \note  The new ID will be different if the object needed to be split. This
**        is necessary if it is referenced from several parent objects not all 
**        of which should be altered. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineReplaceFixup( PTB_PRCEngineID        idEng  /*!< ID of PRC Engine                     */
                         , PTB_PRCFxupID          id     /*!< ID of Fixup for which to replace data */
                         , const PTB_PRCFxupData* data   /*!< New data for Fixup                    */
                         );

/*! 
** \brief  Replace a FixupSet
** 
**         Replaces a FixupSet's data in a PRC Engine with other data
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
** 
** \note  The new ID will be different if the object needed to be split. This
**        is necessary if it is referenced from several parent objects not all 
**        of which should be altered. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineReplaceFixupSet( PTB_PRCEngineID        idEng  /*!< ID of PRC Engine                         */
                            , PTB_PRCFSetID          id     /*!< ID of FixupSet for which to replace data */
                            , const PTB_PRCFSetData* data   /*!< New data for FixupSet                    */
                            );

/*! 
** \brief  Replace a Profile
** 
**         Replaces a Profile's data in a PRC Engine with other data
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** 
** \note  The new ID will be different if the object needed to be split. This
**        is necessary if it is referenced from several parent objects not all 
**        of which should be altered. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineReplaceProfile( PTB_PRCEngineID        idEng  /*!< ID of PRC Engine                        */
                           , PTB_PRCProfID          id     /*!< ID of Profile for which to replace data */
                           , const PTB_PRCProfData* data   /*!< New data for Profile                    */
                           );

/*! 
** \brief  Replace a ProfGroup
** 
**         Replaces a ProfGroup's data in a PRC Engine with other data
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidPGrpID  ID of ProfGroup invalid
** 
** \note  The new ID will be different if the object needed to be split. This
**        is necessary if it is referenced from several parent objects not all 
**        of which should be altered. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineReplaceProfGroup( PTB_PRCEngineID        idEng  /*!< ID of PRC Engine                          */
                             , PTB_PRCPGrpID          id     /*!< ID of ProfGroup for which to replace data */
                             , const PTB_PRCPGrpData* data   /*!< New data for ProfGroup                    */
                             );

/******************************************************************************/
/* Duplicating objects */

/*! 
** \brief  Duplicate a Condition
** 
**         Duplicates a Condition within a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidCondID  ID of Condition invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineDuplicateCondition( PTB_PRCEngineID idEng  /*!< ID of PRC Engine                     */
                               , PTB_PRCCondID*  id     /*!< In: ID of Condition to be duplicated 
                                                            Out: ID of duplicate                 */
                               );

/*! 
** \brief  Duplicate a Rule
** 
**         Duplicates a Rule within a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineDuplicateRule( PTB_PRCEngineID idEng  /*!< ID of PRC Engine                     */
                          , PTB_PRCRuleID*  id     /*!< In: ID of Rule to be duplicated      
                                                       Out: ID of duplicate                 */
                          );

/*! 
** \brief  Duplicate a RuleSet
** 
**         Duplicates a RuleSet within a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineDuplicateRuleSet( PTB_PRCEngineID idEng  /*!< ID of PRC Engine                     */
                             , PTB_PRCRSetID*  id     /*!< In: ID of RuleSet to be duplicated 
                                                          Out: ID of duplicate                 */
                             );

/*! 
** \brief  Duplicate a Fixup
** 
**         Duplicates a Fixup within a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFxupID  ID of Fixup invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineDuplicateFixup( PTB_PRCEngineID idEng  /*!< ID of PRC Engine                     */
                           , PTB_PRCFxupID*  id     /*!< In: ID of Fixup to be duplicated      
                                                        Out: ID of duplicate                 */
                           );

/*! 
** \brief  Duplicate a FixupSet
** 
**         Duplicates a FixupSet within a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineDuplicateFixupSet( PTB_PRCEngineID idEng  /*!< ID of PRC Engine                     */
                              , PTB_PRCFSetID*  id     /*!< In: ID of FixupSet to be duplicated 
                                                           Out: ID of duplicate                 */
                              );

/*! 
** \brief  Duplicate a Profile
** 
**         Duplicates a Profile within a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineDuplicateProfile( PTB_PRCEngineID idEng  /*!< ID of PRC Engine                     */
                             , PTB_PRCProfID*  id     /*!< In: ID of Profile to be duplicated   
                                                          Out: ID of duplicate                 */
                             );

/*! 
** \brief  Duplicate a ProfGroup
** 
**         Duplicates a ProfGroup within a PRC Engine
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidPGrpID  ID of ProfGroup invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineDuplicateProfGroup( PTB_PRCEngineID idEng  /*!< ID of PRC Engine                     */
                               , PTB_PRCPGrpID*  id     /*!< In: ID of ProfGroup to be duplicated 
                                                            Out: ID of duplicate                 */
                               );

/*
********************************************************************************
** Child relationships
********************************************************************************
*/

/******************************************************************************/
/* Enumerating children */

/*! 
** \brief  Enumerate a Rule's Conditions 
** 
**         Enumerates all Conditions which are a child of a Rule
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRuleEnumConditions( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine                             */
                               , PTB_PRCRuleID           id        /*!< ID of Rule for which to enumerate Conditions */
                               , PTB_PRCEnumCondCallback cb        /*!< Enumeration callback                         */
                               , void*                   userData  /*!< User data                                    */
                               );

/*! 
** \brief  Enumerate a RuleSet's Rules
** 
**         Enumerates all Rules which are a child of a RuleSet
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRuleSetEnumRules( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine                             */
                             , PTB_PRCRSetID           id        /*!< ID of RuleSet for which to enumerate Rules   */
                             , PTB_PRCEnumRuleCallback cb        /*!< Enumeration callback                         */
                             , void*                   userData  /*!< User data                                    */
                             );

/*! 
** \brief  Enumerate a FixupSet's Fixups
** 
**         Enumerates all Fixups which are a child of a FixupSet
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of FixupSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineFixupSetEnumFixups( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine                             */
                               , PTB_PRCFSetID           id        /*!< ID of FixupSet for which to enumerate Fixups */
                               , PTB_PRCEnumFxupCallback cb        /*!< Enumeration callback                         */
                               , void*                   userData  /*!< User data                                    */
                               );

/*! 
** \brief  Enumerate a Profile's RuleSets
** 
**         Enumerates all RuleSets which are a child of a Profile
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of RuleSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfileEnumRuleSets( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine                              */
                                , PTB_PRCProfID           id        /*!< ID of Profile for which to enumerate RuleSets */
                                , PTB_PRCEnumRSetCallback cb        /*!< Enumeration callback                          */
                                , void*                   userData  /*!< User data                                     */
                                );

/*! 
** \brief  Enumerate a Profile's FixupSets
** 
**         Enumerates all FixupSets which are a child of a Profile
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfileEnumFixupSets( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine                               */
                                 , PTB_PRCProfID           id        /*!< ID of Profile for which to enumerate FixupSets */
                                 , PTB_PRCEnumFSetCallback cb        /*!< Enumeration callback                           */
                                 , void*                   userData  /*!< User data                                      */
                                 );

/*! 
** \brief  Enumerate a ProfGroup's Profiles
** 
**         Enumerates all Profiles which are a child of a ProfGroup
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidPGrpID  ID of Profile invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfGroupEnumProfiles( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine                                */
                                  , PTB_PRCPGrpID           id        /*!< ID of ProfGroup for which to enumerate Profiles */
                                  , PTB_PRCEnumProfCallback cb        /*!< Enumeration callback                            */
                                  , void*                   userData  /*!< User data                                       */
                                  );

/*! 
** \brief  Enumerate a ProfGroup's MetaProfiles
** 
**         Enumerates all MetaProfiles which are a child of a ProfGroup
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidMPrfID  ID of MetaProfile invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfGroupEnumMetaProfiles( PTB_PRCEngineID         idEng     /*!< ID of PRC Engine                                */
                                      , PTB_PRCPGrpID           id        /*!< ID of ProfGroup for which to enumerate Profiles */
                                      , PTB_PRCEnumMPrfCallback cb        /*!< Enumeration callback                            */
                                      , void*                   userData  /*!< User data                                       */
                                      );

/*! 
** \brief  Enumerate a sequence steps of MetaProfiles
** 
**         Enumerates all sequence steps from a MetaProfile
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidMPrfID  ID of MetaProfile invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineMetaProfilesEnumSequenceSteps( PTB_PRCEngineID                  idEng     /*!< ID of PRC Engine                                */
                                          , PTB_PRCMPrfID                    id        /*!< ID of MetaProfile for which to enumerate steps  */
                                          , PTB_PRCEnumSequenceStepsCallback cb        /*!< Enumeration callback                            */
                                          , void*                            userData  /*!< User data                                       */
                                          );

PTB_FUNC_PROTO
enum PTB_EError
PTB_PRCEngineMetaProfilesEnumSequenceSteps2( PTB_PRCEngineID                   idEng     /*!< ID of PRC Engine                                */
                                           , PTB_PRCMPrfID                     id        /*!< ID of MetaProfile for which to enumerate steps  */
                                           , PTB_PRCEnumSequenceStepsCallback2 cb        /*!< Enumeration callback                            */
                                           , void*                             userData  /*!< User data                                       */
                                           );

/******************************************************************************/
/* Child relationship attributes */

/*! 
** \brief  Get Rule-Condition child attributes
** 
**         Retrieves the attributes of a Rule-Condition child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
** \retval   PTB_eerrPRCInvalidCondID  ID of Condition invalid
** \note   The meta data must be released by passing its ID to PTB_PRCMetaDataRelease()
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineGetRuleConditionAttributes( PTB_PRCEngineID            idEng   /*!< ID of PRC Engine */
                                       , PTB_PRCRuleID              idRule  /*!< ID of Rule       */
                                       , PTB_PRCCondID              idCond  /*!< ID of Conditions */
                                       , PTB_PRCMetaDataID*         idMeta  /*!< Meta data        */
                                       );

/*! 
** \brief  Replace Rule-Condition child attributes
** 
**         Replaces the attributes of a Rule-Condition child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
** \retval   PTB_eerrPRCInvalidCondID  ID of Condition invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineReplaceRuleConditionAttributes( PTB_PRCEngineID            idEng   /*!< ID of PRC Engine */
                                           , PTB_PRCRuleID              idRule  /*!< ID of Rule       */
                                           , PTB_PRCCondID              idCond  /*!< ID of Conditions */
                                           , const PTB_PRCRuleCondAttr* attr    /*!< Child attributes */
                                           );

/*! 
** \brief  Get RuleSet-Rule child attributes
** 
**         Retrieves the attributes of a RuleSet-Rule child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
** \note   The meta data must be released by passing its ID to PTB_PRCMetaDataRelease()
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineGetRuleSetRuleAttributes( PTB_PRCEngineID            idEng    /*!< ID of PRC Engine      */
                                     , PTB_PRCRSetID              idRSet   /*!< ID of RuleSet         */
                                     , PTB_PRCRuleID              idRule   /*!< ID of Rule            */
                                     , PTB_int32_t*               intData  /*!< Rule's check severity */
                                     , PTB_PRCMetaDataID*         idMeta   /*!< Meta data             */
                                     );

/*! 
** \brief  Replace RuleSet-Rule child attributes
** 
**         Replaces the attributes of a RuleSet-Rule child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_PRCEngineReplaceRuleSetRuleAttributes( PTB_PRCEngineID            idEng   /*!< ID of PRC Engine */
                                         , PTB_PRCRSetID              idRSet  /*!< ID of RuleSet    */
                                         , PTB_PRCRuleID              idRule  /*!< ID of Rule       */
                                         , const PTB_PRCRSetRuleAttr* attr    /*!< Child attributes */
                                         );

/*! 
** \brief  Get FixupSet-Fixup child attributes
** 
**         Retrieves the attributes of a FixupSet-Fixup child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of FixupSet invalid
** \retval   PTB_eerrPRCInvalidFxupID  ID of Fixup invalid
** \note   The meta data must be released by passing its ID to PTB_PRCMetaDataRelease()
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineGetFixupSetFixupAttributes( PTB_PRCEngineID            idEng    /*!< ID of PRC Engine      */
                                       , PTB_PRCFSetID              idFSet   /*!< ID of FixupSet         */
                                       , PTB_PRCFxupID              idFxup   /*!< ID of Fixup            */
                                       , PTB_PRCMetaDataID*         idMeta   /*!< Meta data             */
                                       );

/*! 
** \brief  Replace FixupSet-Fixup child attributes
** 
**         Replaces the attributes of a FixupSet-Fixup child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
** \retval   PTB_eerrPRCInvalidFxupID  ID of Fixup invalid
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_PRCEngineReplaceFixupSetFixupAttributes( PTB_PRCEngineID            idEng   /*!< ID of PRC Engine */
                                           , PTB_PRCFSetID              idFSet  /*!< ID of FixupSet    */
                                           , PTB_PRCFxupID              idFxup  /*!< ID of Fixup       */
                                           , const PTB_PRCFSetFxupAttr* attr    /*!< Child attributes */
                                           );

/*! 
** \brief  Get Profile-RuleSet child attributes
** 
**         Retrieves the attributes of a Profile-RuleSet child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
** \note   The meta data must be released by passing its ID to PTB_PRCMetaDataRelease()
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineGetProfileRuleSetAttributes( PTB_PRCEngineID    idEng   /*!< ID of PRC Engine */
                                        , PTB_PRCProfID      idProf  /*!< ID of Profile    */
                                        , PTB_PRCRSetID      idRSet  /*!< ID of RuleSet    */
                                        , PTB_PRCMetaDataID* idMeta  /*!< Meta data        */
                                        );

/*! 
** \brief  Replace Profile-RuleSet child attributes
** 
**         Replaces the attributes of a Profile-RuleSet child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_PRCEngineReplaceProfileRuleSetAttributes( PTB_PRCEngineID            idEng   /*!< ID of PRC Engine */
                                            , PTB_PRCProfID              idProf  /*!< ID of Profile    */
                                            , PTB_PRCRSetID              idRSet  /*!< ID of RuleSet    */
                                            , const PTB_PRCProfRSetAttr* attr    /*!< Child attributes */
                                            );

/*! 
** \brief  Get Profile-FixupSet child attributes
** 
**         Retrieves the attributes of a Profile-FixupSet child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
** \note   The meta data must be released by passing its ID to PTB_PRCMetaDataRelease()
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineGetProfileFixupSetAttributes( PTB_PRCEngineID    idEng   /*!< ID of PRC Engine */
                                         , PTB_PRCProfID      idProf  /*!< ID of Profile    */
                                         , PTB_PRCFSetID      idFSet  /*!< ID of FixupSet    */
                                         , PTB_PRCMetaDataID* idMeta  /*!< Meta data        */
                                         );

/*! 
** \brief  Replace Profile-FixupSet child attributes
** 
**         Replaces the attributes of a Fixup-Condition child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_PRCEngineReplaceProfileFixupSetAttributes( PTB_PRCEngineID            idEng   /*!< ID of PRC Engine */
                                             , PTB_PRCProfID              idProf  /*!< ID of Profile    */
                                             , PTB_PRCFSetID              idRSet  /*!< ID of FixupSet    */
                                             , const PTB_PRCProfFSetAttr* attr    /*!< Child attributes */
                                             );

/*! 
** \brief  Get ProfGroup-Profile child attributes
** 
**         Retrieves the attributes of a ProfGroup-Profile child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidPGrpID  ID of ProfGroup invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \note   The meta data must be released by passing its ID to PTB_PRCMetaDataRelease()
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineGetProfGroupProfileAttributes( PTB_PRCEngineID    idEng   /*!< ID of PRC Engine */
                                          , PTB_PRCPGrpID      idPGrp  /*!< ID of ProfGroup  */
                                          , PTB_PRCProfID      idProf  /*!< ID of Profile    */
                                          , PTB_PRCMetaDataID* idMeta  /*!< Meta data        */
                                          );

/*! 
** \brief  Replace ProfGroup-Profile child attributes
** 
**         Replaces the attributes of a Rule-Condition child relationship
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidPGrpID  ID of ProfGroup invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_PRCEngineReplaceProfGroupProfileAttributes( PTB_PRCEngineID            idEng   /*!< ID of PRC Engine */
                                              , PTB_PRCPGrpID              idPGrp  /*!< ID of ProfGroup  */
                                              , PTB_PRCProfID              idProf  /*!< ID of Profile    */
                                              , const PTB_PRCPGrpProfAttr* attr    /*!< Child attributes */
                                              );

/******************************************************************************/
/* Creating child relationships */

/*! 
** \brief  Add a Condition object to a Rule
** 
**         Associates an existing Condition with a Rule as a new child object
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
** \retval   PTB_eerrPRCInvalidCondID  ID of Condition invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRuleAddChildCondition( PTB_PRCEngineID  idEng        /*!< ID of PRC Engine                    */
                                  , PTB_PRCRuleID    idRule       /*!< ID of Rule to add Condition to      */
                                  , PTB_PRCCondID    idCond       /*!< ID of Condition to be added to Rule */
                                  , PTB_PRCMetaData* metaDataList /*!< Metadata list                       */
                                  , PTB_size_t       metaDataSize /*!< Number of objects in metadata list  */
                                  );

/*! 
** \brief  Add a Rule object to a RuleSet
** 
**         Associates an existing Rule with a RuleSet as a new child object
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRuleSetAddChildRule( PTB_PRCEngineID                idEng         /*!< ID of PRC Engine                   */
                                , PTB_PRCRSetID                  idRSet        /*!< ID of RuleSet to add Rule to       */
                                , PTB_PRCRuleID                  idRule        /*!< ID of Rule to be added to RuleSet  */
                                , PTB_PRCMetaData*               metaDataList  /*!< Metadata list                      */
                                , PTB_size_t                     metaDataSize  /*!< Number of objects in metadata list */
                                , enum PTB_PRCERuleCheckSeverity cs            /*!< Rule's check severity              */
                                );

/*! 
** \brief  Add a Fixup object to a FixupSet
** 
**         Associates an existing Fixup with a FixupSet as a new child object
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
** \retval   PTB_eerrPRCInvalidFxupID  ID of Fixup invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineFixupSetAddChildFixup( PTB_PRCEngineID           idEng         /*!< ID of PRC Engine                    */
                                  , PTB_PRCFSetID             idFSet        /*!< ID of FixupSet to add Fixup to      */
                                  , PTB_PRCFxupID             idFxup        /*!< ID of Fixup to be added to FixupSet */
                                  , PTB_PRCMetaData*          metaDataList  /*!< Metadata list                       */
                                  , PTB_size_t                metaDataSize  /*!< Number of objects in metadata list  */
                                  );

/*! 
** \brief  Add a RuleSet object to a Profile
** 
**         Associates an existing RuleSet with a Profile as a new child object
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfileAddChildRuleSet( PTB_PRCEngineID  idEng         /*!< ID of PRC Engine                     */
                                   , PTB_PRCProfID    idProf        /*!< ID of Profile to add RuleSet to      */
                                   , PTB_PRCRSetID    idRSet        /*!< ID of RuleSet to be added to Profile */
                                   , PTB_PRCMetaData* metaDataList  /*!< Metadata list                        */
                                   , PTB_size_t       metaDataSize  /*!< Number of objects in metadata list   */
                                   );

/*! 
** \brief  Add a FixupSet object to a Profile
** 
**         Associates an existing FixupSet with a Profile as a new child object
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfileAddChildFixupSet( PTB_PRCEngineID  idEng         /*!< ID of PRC Engine                     */
                                    , PTB_PRCProfID    idProf        /*!< ID of Profile to add FixupSet to      */
                                    , PTB_PRCFSetID    idFSet        /*!< ID of FixupSet to be added to Profile */
                                    , PTB_PRCMetaData* metaDataList  /*!< Metadata list                        */
                                    , PTB_size_t       metaDataSize  /*!< Number of objects in metadata list   */
                                    );

/*! 
** \brief  Add a Profile object to a ProfGroup
** 
**         Associates an existing Profile with a ProfGroup as a new child object
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidPGrpID  ID of ProfGroup invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfGroupAddChildProfile( PTB_PRCEngineID  idEng         /*!< ID of PRC Engine                   */
                                     , PTB_PRCPGrpID    idPGrp        /*!< ID of ProfGroup to add Profile to      */
                                     , PTB_PRCProfID    idProf        /*!< ID of Profile to be added to ProfGroup */
                                     , PTB_PRCMetaData* metaDataList  /*!< Metadata list                      */
                                     , PTB_size_t       metaDataSize  /*!< Number of objects in metadata list */
                                     );

/******************************************************************************/
/* Deleting child-relationships */

/*! 
** \brief  Remove a Condition object from a Rule
** 
**         Removes a Condition from a Rule's child object list
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
** \retval   PTB_eerrPRCInvalidCondID  ID of Condition invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRuleRemoveChildCondition( PTB_PRCEngineID idEng   /*!< ID of PRC Engine                        */
                                     , PTB_PRCRuleID   idRule  /*!< ID of Rule to remove Condition from     */
                                     , PTB_PRCCondID   idCond  /*!< ID of Condition to be removed from Rule */
                                     );

/*! 
** \brief  Remove a Rule object from a RuleSet
** 
**         Removes a Rule from a RuleSet's child object list
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
** \retval   PTB_eerrPRCInvalidRuleID  ID of Rule invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineRuleSetRemoveChildRule( PTB_PRCEngineID idEng   /*!< ID of PRC Engine                      */
                                   , PTB_PRCRSetID   idRSet  /*!< ID of RuleSet to remove Rule from     */
                                   , PTB_PRCRuleID   idRule  /*!< ID of Rule to be removed from RuleSet */
                                   );

/*! 
** \brief  Remove a Fixup object from a FixupSet
** 
**         Removes a Fixup from a FixupSet's child object list
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
** \retval   PTB_eerrPRCInvalidFxupID  ID of Fixup invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineFixupSetRemoveChildFixup( PTB_PRCEngineID idEng   /*!< ID of PRC Engine                        */
                                     , PTB_PRCFSetID   idFSet  /*!< ID of FixupSet to remove Fixup from     */
                                     , PTB_PRCFxupID   idFxup  /*!< ID of Fixup to be removed from FixupSet */
                                     );

/*! 
** \brief  Remove a RuleSet object from a Profile
** 
**         Removes a RuleSet from a Profile's child object list
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval   PTB_eerrPRCInvalidRSetID  ID of RuleSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfileRemoveChildRuleSet( PTB_PRCEngineID idEng   /*!< ID of PRC Engine                         */
                                      , PTB_PRCProfID   idProf  /*!< ID of Profile to remove RuleSet from     */
                                      , PTB_PRCRSetID   idRSet  /*!< ID of RuleSet to be removed from Profile */
                                      );

/*! 
** \brief  Remove a FixupSet object from a Profile
** 
**         Removes a FixupSet from a Profile's child object list
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
** \retval   PTB_eerrPRCInvalidFSetID  ID of FixupSet invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfileRemoveChildFixupSet( PTB_PRCEngineID idEng   /*!< ID of PRC Engine                          */
                                       , PTB_PRCProfID   idProf  /*!< ID of Profile to remove FixupSet from     */
                                       , PTB_PRCFSetID   idFSet  /*!< ID of FixupSet to be removed from Profile */
                                       );

/*! 
** \brief  Remove a Profile object from a ProfGroup
** 
**         Removes a Profile from a ProfGroup's child object list
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID  ID of PRC Engine invalid
** \retval   PTB_eerrPRCInvalidPGrpID  ID of ProfGroup invalid
** \retval   PTB_eerrPRCInvalidProfID  ID of Profile invalid
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfGroupRemoveChildProfile( PTB_PRCEngineID idEng   /*!< ID of PRC Engine                       */
                                        , PTB_PRCPGrpID   idPGrp  /*!< ID of ProfGroup to remove Profile from     */
                                        , PTB_PRCProfID   idProf  /*!< ID of Profile to be removed from ProfGroup */
                                        );

/******************************************************************************/
/* Creating reference relationships */

PTB_FUNC_PROTO
enum PTB_EError
PTB_PRCEngineFixupAddReferenceRule( PTB_PRCEngineID  idEng         /*!< ID of PRC Engine                   */
                                  , PTB_PRCFxupID    idFxup        /*!< ID of Fixup                        */
								  , PTB_PRCRuleID    idRule		   /*!< ID of Rule                         */
								  , PTB_PRCMetaData* metaDataList  /*!< Metadata list                      */
								  , PTB_size_t       metaDataSize  /*!< Number of objects in metadata list */
								  );

PTB_FUNC_PROTO
enum PTB_EError
PTB_PRCEngineConditionAddReferenceRule( PTB_PRCEngineID  idEng         /*!< ID of PRC Engine                   */
                                      , PTB_PRCCondID    idCond        /*!< ID of Condition                    */
							    	  , PTB_PRCRuleID    idRule		   /*!< ID of Rule                         */
								      , PTB_PRCMetaData* metaDataList  /*!< Metadata list                      */
								      , PTB_size_t       metaDataSize  /*!< Number of objects in metadata list */
								      );

/******************************************************************************/
/* Dynamic params */

/*! 
** \brief  Enumerate a Profile's dynamic params
** 
**         Enumerates all dynamic params which are a used in a profile
**         Modifiactions to the PRC engine are not allowed inside this callback
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID      ID of PRC Engine invalid
** \retval PTB_eerrPRCInvalidProfID        ID of Profile invalid
** \retval PTB_eerrPRCEngineOpNotSupported An attempt to modify the engine failed
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineProfileEnumDynParams( PTB_PRCEngineID             idEng     /*!< ID of PRC Engine                                      */
                                 , PTB_PRCProfID               idProf    /*!< ID of Profile for which to enumerate dynamic params   */
                                 , PTB_PRCEnumDynParamCallback cb        /*!< Enumeration callback                                  */
                                 , void*                       userData  /*!< User data                                             */
                                 );

/*! 
** \brief  Enumerate a MetaProfile's dynamic params
** 
**         Enumerates all dynamic params which are a used in a meta profile
**         Modifiactions to the PRC engine are not allowed inside this callback
** 
** \retval               PTB_eerrNone  No error
** \retval PTB_eerrPRCInvalidEngineID      ID of PRC Engine invalid
** \retval PTB_eerrPRCInvalidProfID        ID of Profile invalid
** \retval PTB_eerrPRCEngineOpNotSupported An attempt to modify the engine failed
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCEngineMetaProfileEnumDynParams( PTB_PRCEngineID             idEng     /*!< ID of PRC Engine                                           */
                                     , PTB_PRCMPrfID               idMPrf    /*!< ID of Meta Profile for which to enumerate dynamic params   */
                                     , PTB_PRCEnumDynParamCallback cb        /*!< Enumeration callback                                       */
                                     , void*                       userData  /*!< User data                                                  */
                                     );

/*! 
** \brief  Get dynamic param data
** 
**         Get a dynamic param's data
**
** \note   Possible values for \a edt are: 
**         PTB_PRCEdtDynParamKey, PTB_PRCEdtDynParamDefValue
**          - idStr must point to a string ID for the function to write the 
**            result to
** 
** \retval PTB_eerrNone                  No error
** \retval PTB_eerrPRCInvalidDynParamID  Invalid dynamic parameter ID
** \retval PTB_eerrPRCInvalidDataType    Data type invalid for this object type
**
** \note   The String ID returned by this function needs to get released by calling PTB_StringRelease()
** 
** \relatesalso PTB_PRCMetaData
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetDynParamData( PTB_PRCDynParamID idParam  /*!< Dyn Param ID                */
                      , enum PTB_PRCEDataType edt  /*!< Data type to retrieve       */
                      , PTB_StringID*     idStr    /*!< String ID for dyn param key */
                      );


/*! 
** \brief  Set actual value for dynamic param as string
** 
**         Overwrites the current default value for a dynmaic parameter
** 
** \retval                 PTB_eerrNone  No error
** \retval     PTB_eerrPRCInvalidProfID  Invalid Profile ID
** \retval PTB_eerrPRCInvalidDynParamID  Invalid dynamic parameter ID
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCSetDynParamValueString( PTB_PRCDynParamID       idParam  /*!< Dyn Param ID     */
                             , const PTB_utf8_char_t * strValue /*!< New string value  */
                             );

/******************************************************************************/
/* Wizard Checks */

/*! 
** \brief  Get Wizard Check state
** 
**         Returns the Wizard Check state for a specific check
** 
** \retval                 PTB_eerrNone  No error
** \retval     PTB_eerrPRCInvalidProfID  Invalid Profile ID
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetProfileWizardCheckState( PTB_PRCEngineID                idEng   /*!< ID of PRC Engine                            */
                                 , PTB_PRCProfID                  idProf  /*!< ID of Profile to get Wizard Check state for */
                                 , enum PTB_EWizardCheckStateData ewcsd   /*!< Check to get state for                      */
                                 , enum PTB_EWizardCheckState*    wcs     /*!< Pointer to Wizard Check state               */
                                 );

/*! 
** \brief  Get Wizard Check state data
** 
**         Returns the Wizard Check state's additional data for a specific check
** 
** \note   Possible values for \a ewcsd are: 
**         PTB_ewcsdPageSizeIsNotOrientation, 
**         PTB_ewcsdPageOnePageEmptyIgnoreOutsideBox, 
**         PTB_ewcsdColrObjectUseSpotColorsInList, 
**         PTB_ewcsdFontIsUsedInList, 
**         PTB_ewcsdPDFXAOutputConditionInList, 
**         PTB_ewcsdPDFXADocumentPDFX1a2001, 
**         PTB_ewcsdPDFXADocumentPDFX1a2003, 
**         PTB_ewcsdPDFXADocumentPDFX32002, 
**         PTB_ewcsdPDFXADocumentPDFX32003, 
**         PTB_ewcsdPDFXADocumentPDFX4, 
**         PTB_ewcsdPDFXADocumentPDFX4p, 
**         PTB_ewcsdPDFXADocumentPDFX5g, 
**         PTB_ewcsdPDFXADocumentPDFX5pg, 
**         PTB_ewcsdPDFXADocumentPDFA1a2005, 
**         PTB_ewcsdPDFXADocumentPDFA1b2005,
**         PTB_ewcsdPDFXADocumentPDFA2a,
**         PTB_ewcsdPDFXADocumentPDFA2b,
**         PTB_ewcsdPDFXADocumentPDFA2u,
**         PTB_ewcsdPDFXADocumentPDFA3a,
**         PTB_ewcsdPDFXADocumentPDFA3b,
**         PTB_ewcsdPDFXADocumentPDFA3u
**          - intData must point to an int for the function to write the 
**            result (bool) to
**          - floatData must be NULL
**          - idStr must be NULL
**         PTB_ewcsdPageNumPagesNumber, PTB_ewcsdColrMoreThanPlates, 
**         PTB_ewcsdColrObjectUseSpotColorsListSize, 
**         PTB_ewcsdFontIsUsedListSize, 
**         PTB_ewcsdPDFXAOutputConditionListSize
**          - intData must point to an int for the function to write the 
**            result (unsigned int) to
**          - floatData must be NULL
**          - idStr must be NULL
**         PTB_ewcsdDocumentRequiresAtLeastVersion
**          - intData must point to an int for the function to write the 
**            result (PTB_EWCAcrobatVersion) to
**          - floatData must be NULL
**          - idStr must be NULL
**         PTB_ewcsdFontEmbeddedType
**          - intData must point to an int for the function to write the 
**            result (PTB_EFontsEmbedded) to
**          - floatData must be NULL
**          - idStr must be NULL
**         PTB_ewcsdPageSizeIsNotWidth, 
**         PTB_ewcsdPageSizeIsNotHeight, 
**         PTB_ewcsdPageSizeIsNotTolerance, 
**         PTB_ewcsdImagResImgLowerPPI, 
**         PTB_ewcsdImagResImgUpperPPI, 
**         PTB_ewcsdImagResBmpLowerPPI, 
**         PTB_ewcsdImagResBmpUpperPPI, 
**         PTB_ewcsdRendThicknessPoints
**          - intData must be NULL
**          - floatData must point to a double for the function to write the 
**            result to
**          - idStr must be NULL
**         PTB_ewcsdPageSizeIsNotUnit, 
**         PTB_ewcsdPageNumPagesOperator, 
**         PTB_ewcsdColrObjectUseSpotColorsListItem, 
**         PTB_ewcsdFontIsUsedListItem, 
**         PTB_ewcsdPDFXAOutputConditionListItem
**          - intData must be NULL
**          - floatData must be NULL
**          - idStr must point to a string ID for the function to write the 
**            result to
** 
** \retval              PTB_eerrNone  No error
** \retval  PTB_eerrPRCInvalidProfID  Invalid Profile ID
** \retval PTB_eerrPRCNoWizChackData  Invalid Profile ID
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCGetProfileWizardCheckStateData( PTB_PRCEngineID                idEng      /*!< ID of PRC Engine                            */
                                     , PTB_PRCProfID                  idProf     /*!< ID of Profile to get Wizard Check state for */
                                     , enum PTB_EWizardCheckStateData ewcsd      /*!< Pointer to Wizard Check state               */
                                     , PTB_int32_t*                   intData    /*!< Pointer to integer data                     */
                                     , PTB_float_t*                   floatData  /*!< Pointer to floating point data              */
                                     , PTB_StringID*                  idStr      /*!< String data                                 */
                                     );

/*! 
** \brief  Set Profile's Wizard Checks
** 
**         Sets the Wizard Check for a specific Profile
** 
** \retval                 PTB_eerrNone  No error
** \retval     PTB_eerrPRCInvalidProfID  Invalid Profile ID
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCSetProfileWizardChecks( PTB_PRCEngineID                   idEng   /*!< ID of PRC Engine                            */
                             , PTB_PRCProfID                     idProf  /*!< ID of Profile to get Wizard Check state for */
                             , const struct PTB_PRCWizardChecks* wcs     /*!< Wizard Checks to set                        */
                             );

/*! 
** \brief  Clears Wizard Check data
** 
**         Clears a Wizard Check data object
** 
** \retval                 PTB_eerrNone  No error
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_PRCClrProfileWizardCheckData( struct PTB_PRCWizardChecks* wcs     /*!< Wizard Check data to clear */
                                );

/******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_PRC_H*/

/******************************************************************************/
/* EOF */
