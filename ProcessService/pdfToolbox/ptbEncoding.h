/*!
**
** \file ptbEncoding.h
**
** \brief callas pdfEngine SDK: Encoding converter
**
** This declares functions for converting between system, UTF-8, and UTF-16 encoding. 
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_ENCODING_H)
/*! \def PTB_ENCODING_H header define */
#define PTB_ENCODING_H

/******************************************************************************/

#include "ptbTypes.h"

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** Encoding conversion functions
*/

/*! Endianess */
enum PTB_ConvertEUTF16Endian { PTB_ConvertEHostEndian    /*!< Returns the bytes in the host's native endian.   */
                             , PTB_ConvertELittleEndian  /*!< Always returns the bytes in little-endian order. */
                             , PTB_ConvertEBigEndian     /*!< Always returns the bytes in big-endian order.    */
                             };

/*!
** \brief  Convert UTF-8 to UTF-16
** 
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrStrBufferTooSmall  Unknown error
**                                    (If so, then \a strBufSize will contain
**                                     the needed buffer size)
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_ConvertUTF8ToUTF16( const PTB_utf8_char_t *      strSource
                      , PTB_utf16_char_t*            bufTarget
                      , PTB_uint32_t    *            bufSize     /*!< in: supplied buffer length, out: required buffer length */
                      , enum PTB_ConvertEUTF16Endian utf16Endian
                      );

/*!
** \brief  Convert UTF-16 to UTF-8
** 
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrStrBufferTooSmall  Unknown error
**                                    (If so, then \a strBufSize will contain
**                                     the needed buffer size)
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_ConvertUTF16ToUTF8( const PTB_utf16_char_t *      strSource
                      , PTB_utf8_char_t *             bufTarget
                      , PTB_uint32_t *                bufSize     /*!< in: supplied buffer length, out: required buffer length */
                      , enum PTB_ConvertEUTF16Endian  utf16Endian
                      );

/*!
** \brief  Convert system encoding to UTF-8
** 
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrStrBufferTooSmall  Unknown error
**                                    (If so, then \a strBufSize will contain
**                                     the needed buffer size)
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_ConvertSysToUTF8  ( const PTB_sys_char_t * strSource
                      , PTB_utf8_char_t *      bufTarget
                      , PTB_uint32_t *         bufSize       /*!< in: supplied buffer length, out: required buffer length */
                      );

/*!
** \brief  Convert UTF-8 to system encoding
** 
** \retval              PTB_eerrNone  No error
** \retval PTB_eerrStrBufferTooSmall  Unknown error
**                                    (If so, then \a strBufSize will contain
**                                     the needed buffer size)
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_ConvertUTF8ToSys  ( const PTB_utf8_char_t * strSource
                      ,       PTB_sys_char_t *  bufTarget
                      ,       PTB_uint32_t *    bufSize       /*!< in: supplied buffer length, out: required buffer length*/
                      );

/******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_ENCODING_H*/

/******************************************************************************/
/* EOF */
