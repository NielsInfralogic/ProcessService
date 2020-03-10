/*!
** 
** \file ptbStrings.h
** 
** \brief callas pdfEngine SDK: String access
** 
** The callas pdfEngine SDK returns strings by returning IDs. These IDs 
** can then be used to retrieve the string's length, allocate a buffer of the 
** needed size, and have the API write the string to this buffer. Afterwards, 
** all string IDs must be released by passing them to PTB_StringRelease() API 
** function. <br>
** Note that in this API all strings containing user data are UTF-8 encoded. 
** 
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_STRINGS_H)
/*! \def PTB_STRINGS_H header define */
#define PTB_STRINGS_H

/******************************************************************************/

#include "ptbTypes.h"

/******************************************************************************/

#include "ptbProlog.h"

/******************************************************************************/

/*! 
** \brief  Test IDs 
**         Tests whether a String ID is valid.
**
** \retval   0  ID is invalid
** \retval !=0  ID is valid
*/
PTB_FUNC_PROTO 
PTB_bool_t 
PTB_IsValidStringID(PTB_StringID id  /*!< String ID to check */
                   );

/*! 
** \brief  Get length of string
**         Returns the length of the string with the ID id
**
** \retval PTB_eerrNone             No error
** \retval PTB_eerrInvalidStringID  Invalid string ID
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_StringLength( PTB_StringID id    /*!< String ID    */
                , PTB_size_t*  size  /*!< String size  */
                );

/*! 
** \brief  Get string
**         Returns the string with the ID id
**
** \retval PTB_eerrNone               No error
** \retval PTB_eerrInvalidStringID    Invalid string ID
** \retval PTB_eerrStrBufferTooSmall  Buffer too small (\a strBufSize contains needed buffer size)
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_StringGet( PTB_StringID     id    /*!< String ID                */
             , PTB_utf8_char_t* buf   /*!< Pointer to buffer string */
             , PTB_size_t*      size  /*!< String buffer size       */
             );

/*! 
** \brief  Release string
**         Releases the string with the ID id
**
** \retval PTB_eerrNone             No error
** \retval PTB_eerrInvalidStringID  Invalid string ID
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_StringRelease( PTB_StringID id  /*!< String ID                */
                 );

/*! 
** \brief  Get and release string
**         Returns the string with the ID id and releases it
**
** \retval PTB_eerrNone               No error
** \retval PTB_eerrInvalidStringID    Invalid string ID
** \retval PTB_eerrStrBufferTooSmall  Buffer too small (\a strBufSize contains needed buffer size)
** \note   If the result is PTB_eerrStrBufferTooSmall, the String won't be released
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_StringGetAndRelease( PTB_StringID     id    /*!< String ID                */
                       , PTB_utf8_char_t* buf   /*!< Pointer to string buffer */
                       , PTB_size_t*      size  /*!< String buffer size       */
                       );

/******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif // PTB_STRINGS_H

/******************************************************************************/
/* EOF */
