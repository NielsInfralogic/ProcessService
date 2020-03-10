/*!
**
** \file ptbLib.h
**
** \brief callas pdfEngine SDK: initialization
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_LIB_H)
/*! \def PTB_LIB_H header define */
#define PTB_LIB_H

/******************************************************************************/

#include "ptbTypes.h"

/******************************************************************************/

#include "ptbProlog.h"

/******************************************************************************/
/*!
** \brief   Initializes the library.
** \note    Loads the dictionary file. Must only be called once.  
**
**          Other library functions might crash, if the library is not initialized.
**          Initialization will fail, if the dictionary file could not be loaded.
**
** \retval PTB_eerrNone                 No error
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LibInit( const PTB_sys_char_t * keycode       /*!< Keycode */
           , const PTB_sys_char_t * keycodeAdOn   /*!< Keycode for DeviceLink Ad on (may be NULL) */
           , const PTB_Path_t     * pathtolib     /*!< complete path to the library  */
           );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LibInit2( const PTB_sys_char_t * keycode       /*!< Keycode */
            , const PTB_sys_char_t * keycodeAdOn   /*!< Keycode for DeviceLink Ad on (may be NULL) */
            , const PTB_Path_t     * pathtolib     /*!< complete path to the library  */
            , const PTB_sys_char_t * lang          /*!< lang code (can be NULL to use english dictionary) */
            );

/*!
** \brief   Releases library and frees the resources.
** \note    Must only be called once. 
**
** \retval PTB_eerrNone               No error
** \retval PTB_eerrLibNotInitialized  Lib not initialized
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_LibRelease();

/*!
** \brief   Get library API version. 
** 
**          The user should always compare the pdfEngineVersion and apiVersion. 
**          Both values, returned by the library, must coincide with the number in 
**          in the header file 'ptbTypes.h'.
**          For compatibility, the apiIteration number must be greater or equal than 
**          the value in the header 'ptbTypes.h'.
** \param   pdfEngineVersion is 7.
**          
** \param   apiVersion   dPTBSDK_APIMainVersion
** \param   apiIteration dPTBSDK_APIVersionIteration
** \param   buildNumber  is a value, we need for support of the library. This can be ignored
**                       but is needed in all support requests.
**
** \retval PTB_eerrNone     No error
** \retval PTB_eerrUnknown  Unknown error
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LibAPIGetVersion( PTB_uint16_t * pdfEngineVersion    /*!< retrieves the pdfEngine version of the pdfEngine library */
                    , PTB_uint16_t * apiVersion          /*!< retrieves the API version of the pdfEngine library, see dPTBSDK_APIMainVersion */
                    , PTB_uint16_t * apiIteration        /*!< retrieves the API iteration version of the pdfEngine library, see dPTBSDK_APIVersionIteration */
                    , PTB_uint16_t * buildNumber         /*!< retrieves the build number of the pdfEngine library */
                    );

PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LibAPIGetVersion2( PTB_uint16_t * pdfEngineVersion    /*!< retrieves the pdfEngine version of the pdfEngine library */
                     , PTB_uint16_t * apiVersion          /*!< retrieves the API version of the pdfEngine library, see dPTBSDK_APIMainVersion */
                     , PTB_uint16_t * apiIteration        /*!< retrieves the API iteration version of the pdfEngine library, see dPTBSDK_APIVersionIteration */
                     , PTB_uint16_t * buildNumber         /*!< retrieves the build number of the pdfEngine library */
					 , PTB_bool_t   * x64                 /*!< the pdfEngine library is x64 */
					 , PTB_bool_t   * threadSafe          /*!< the pdfEngine library is thread-safe */
                     );

/*!
** \brief   Register custom icc profiles.
** 
** \retval PTB_eerrNone     No error
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LibRegisterCustomICCProfile( const PTB_utf8_char_t* name    /*!< the name           */
                               , const PTB_Path_t*      iccFile /*!< ICC Profile        */
                               );


/*!
** \brief  Resource types for PTB_LibRegisterCustomResource() \note May be extended in upcoming versions of the library
*/
enum PTB_EResourceType { PTB_ertICCProfile    /* ICC profile */
                       , PTB_ertFSPolicy      /* Font substitution policy file */
					   , PTB_ertCCPolicy      /* Color conversion policy file */
					   };

/*!
** \brief   Register custom resource file
** 
** \retval PTB_eerrNone                No error
** \retval PTB_eerrFileSourceFileExist File does not exist
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LibRegisterCustomResource( enum PTB_EResourceType type    /*!< which type to set         */
							 , const PTB_utf8_char_t* name    /*!< the name                  */
							 , const PTB_Path_t*      resFile /*!< Path to the resource file */
                             );

/*!
** \brief  Path types for PTB_LibSetPath() \note May be extended in upcoming versions of the library
** Default cache folder:
**   Mac: ~/Library/Preferences/callas software/callas pdfEngine SDK
**   Win: %APPDATA%/callas software/callas pdfEngine SDK
**   Unix: $HOME\.callas software/callas pdfEngine SDK
*/
enum PTB_EPathType { PTB_eptCache     /* base folder for the cache folder. To be used in conjunction with PTB_LibSetPath */
                   , PTB_eptTemp      /* base folder for the temp folder. To be used in conjunction with PTB_LibSetPath */
                   , PTB_eptNormICC   /* path for a Normalizer ICC profiles folder. To be used in conjunction with PTB_LibAddPath */
                   , PTB_eptNormFonts /* path for a Normalizer Fonts Folder (Type1 only). To be used in conjunction with PTB_LibAddPath */
                   , PTB_eptNormHostFonts /* path for a Normalizer "Host Fonts" Folder. To be used in conjunction with PTB_LibAddPath */
                   };

/*!
** \brief   Set internal path for specific types, must be called before PTB_LibInit
** 
** \retval PTB_eerrNone               No error
** \retval PTB_eerrSourceFolderExist  Folder does not exist
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LibSetPath( enum PTB_EPathType type  /*!< which path to set                                            */
              , const PTB_Path_t*  path  /*!< new path that is used in subsequent calls inside the library */
              );

/*!
** \brief   Add given path to internal folder list for specific types
** 
** \retval PTB_eerrNone               No error
** \retval PTB_eerrLibNotInitialized  Lib not initialized
** \retval PTB_eerrSourceFolderExist  Folder does not exist
** \retval PTB_eerrParamFailure       path type not valid for addition
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LibAddPath( enum PTB_EPathType type  /*!< which kind of path to add to */
              , const PTB_Path_t*  path  /*!< additional path that is used in subsequent calls inside the library */
              );

/*!
** \brief   Sets a profile matching from path
** 
** \retval PTB_eerrNone                No error
** \retval PTB_eerrFileSourceFileExist File does not exist
** \retval PTB_eerrParamFailure        A parameter is unknown or incorrect
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_LibSetWorkingSpaceProfileFromPath( const PTB_Path_t * path       /*!< the ICC profile */
                                     );

/*!
** \brief   Sets a profile matching the description string
** 
** \retval PTB_eerrNone                No error
** \retval PTB_eerrParamFailure        A parameter is unknown or incorrect
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_LibSetWorkingSpaceProfileFromDesc( const PTB_utf8_char_t * desc  /*!< the description string */
                                     );

/*!
** \brief  Empties the font cache - all font information will be removed.
**
** \note   ATTENTION: This function will be remove the folder in 'path'!
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
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LibEmptyFontCache( const PTB_Path_t*  path /*!< Path for the font cache, if NULL, FontCache in the default (user) preferences will be removed */
                     );

/*!
** \brief  Language callback
** 
** \return If the user's implementation returns false, processing of the 
**         dictionary file will stop and PTB_LanguageEnum() will return
**        ::PTB_eerrCancel
**
** \note   \a idLang can be used during subsequent calls to PTB_LanguageSet() 
**         The user should use the language ID received through \a idLang to 
**         refer to the language. 
*/
typedef PTB_bool_t (*PTB_LanguageCB)( PTB_StringID           idLang    /*!< Language ID of language */
                                    , void*                  userData  /*!< User data               */
                                    );

/*!
** \brief  Enumerates all available languages
** 
**         When calling this function, \a cbLang will be called for all 
**         languages.
** 
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrLibNotInitialized  Lib not initialized
** \retval           PTB_eerrUnknown  Unknown error
** \retval            PTB_eerrCancel
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LanguageEnum( PTB_LanguageCB   cbLang    /*!< language callback, gets called once for every language, that is available  */
                , void*            userData  /*!< User data submitted to language callback function                          */
                );
				  
/*!
** \brief  Sets current language
** 
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrLibNotInitialized  Lib not initialized
** \retval           PTB_eerrUnknown  Unknown error
** \retval     PTB_eerrLangFileExist  Language file does not exist
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LanguageSet(const PTB_sys_char_t * lang  /*!< language code to be used (en, de, fr, ...) */
               );

/*!
** \brief  Loads a user's language file 
** 
**         Loads a user's translated language XML file. 
** 
** \note   There is no check whether all necessary key-value pairs are present 
**         in the file. Only duplicate keys will result in an error. It is the 
**         user's responsibility to create correct translation files. 
**         It is not possible to read the same file twice 
**         The language in between, because this would produce duplicate keys. 
**         You can put multiple languages into one file.
**         Each language XYZ corresponding to the line in the file: <BR>
**                   \<dict version="3.0" lang="XYZ"\> <BR>
**         will create its own language id.
**         This function will be set the current language. If more than one
**         language in the XML file, the last language is the current one.
**         If you want use an other language you must call \a PTB_LanguageSet
**
**         Note: Please use only own strings in the XML files.
**
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrLibNotInitialized  Lib not initialized
** \retval PTB_eerrSourceVolumeExist  Volume does not exist
** \retval PTB_eerrSourceFolderExist  Folder does not exist
** \retval        PTB_eerrSourceRead  No read access to file
** \retval   PTB_eerrFileNameInvalid  Invalid file name
** \retval   PTB_eerrInvalidLangFile  Invalid file format
** \retval           PTB_eerrUnknown  Unknown error
** \retval            PTB_eerrCancel  If the PTB_LanguageCB returned false.
** \retval     PTB_eerrLangFileExist  Language file does not exist
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LanguageLoadXMLFile( const PTB_Path_t * dictfile    /*!< XML dictionary file to be loaded                                                                             */
                       , PTB_LanguageCB     cbLang      /*!< language ID callback, gets called once for every language found in the language dictionary file (may be NULL) */
                       , void*              userData    /*!< User data submitted to language callback function                                                             */
                       );



enum PTB_EFontnameFilterType { 
	  PTB_efdtSubstitution = 1    // fontName valid for substitution
	, PTB_efdtImposition   = 2    // fontName valid for imposition
};


typedef PTB_bool_t (*PTB_FontnameCB)( PTB_StringID           idFontname    /*!< StringID of the Fontname */
                                    , PTB_uint32_t           flags         /*!< a combination of values from PTB_EFontnameFilterType */
                                    , void*                  userData      /*!< User data               */
                                    );
/*!
** \brief  Fontname callback
** 
** \return If the user's implementation returns false, processing will stop and PTB_FontnameEnum() will return
**        ::PTB_eerrCancel
**
** \note   \a the received fontname can be used as a valid name for imposition of for font substitution.
*/

/*!
** \brief  Enumerates all available fontnames
** 
**         When calling this function, \a cbFontname will be called for all 
**         fontnames contained in the specified fontname domain.
** 
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrLibNotInitialized  Lib not initialized
** \retval           PTB_eerrUnknown  Unknown error
** \retval            PTB_eerrCancel
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_FontnameEnum( PTB_uint32_t     filter                /*!< restrict enumerated fontnames to the given filter (see PTB_EFontnameFilterType) */
				, PTB_FontnameCB   cbFontname            /*!< fontname callback, gets called once for every fontname, that is available  */
				, void*            userData              /*!< User data submitted to fontname callback function                          */
				);



/*!
** \brief   Avalaible license types for the license server
*/
enum PTB_ELicenseServerType { PTB_eelstNone                         = 0x0000 /*!< No license found */
                            , PTB_eelstRuncount                     = 0x0001 /*!< Use Runcount     */
                            , PTB_eelstCredits                      = 0x0002 /*!< Use Credits      */
                            };

/*!
** \brief  Setups the license server
** \retval PTB_eerrNone                 No error
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_LicenseServerSetup( const PTB_utf8_char_t**    urls     /*!< list of urls including optional port , e.g. "10.0.0.228", "10.0.0.228:1400",...  */
                      , PTB_uint32_t            urlsCount   /*!< indicates the size of the urls array */
                      , PTB_uint32_t            timeout     /*!< default timeout in seconds */
                      , PTB_uint32_t            types       /*!< default license types (any combination of PTB_ELicenseServerType values) */
                      );


/*!
** \brief   The license is in use after this call.

** \note    This function is referenced counted and can be called multiple times in the same thread. In this case only the first invocation uses a license.
**
** \retval PTB_eerrNone                  If license is available.
** \retval PTB_eerrLicenseNoServer       No license server found
** \retval PTB_eerrLicenseNoCartridge    License server found but no matching cartridge installed
** \retval PTB_eerrLicenseNoLicense      License server with matching cartridge found, but no free license
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_LicenseServerGetLicense( PTB_uint32_t            timeout         /*!< timeout in seconds */
                           , PTB_uint32_t            licenseTypes    /*!< license types (any combination of PTB_ELicenseServerType values) */
                           , PTB_ELicenseServerType* usedLicenseType /*!< [OUT] If not NULL, will be filled with actual used license type */
                            );

/*!
** \brief   The license is freed after this call.

** \note    Decrements the reference counter for the current thread and releases the license if reference counter is zero.
**
** \retval PTB_eerrNone                  No error
*/
PTB_FUNC_PROTO
enum PTB_EError
PTB_LicenseServerFreeLicense();


/******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_LIB_H*/

/******************************************************************************/
/* EOF */
