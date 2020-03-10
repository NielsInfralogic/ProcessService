/*!
**
** \file ptbLog.h
**
** \brief callas pdfEngine SDK: Logging
** 
** The callas pdfEngine SDK logs many of its activities. This can be used to 
** diagnose errors and better understand what is going on "under the hood". 
** Users of the library can either let the library write its log messages into 
** a file or have it call them back with the log statements in strings. <BR>
** Upon setting up logging (using either PTB_LoggingStartFileLog() or 
** PTB_LoggingStartCBLog()) the library will return an ::PTB_LoggingID which 
** can be used later to stop logging to this target. <BR>
** Log messages have a severity. Users can setup logging so that only log 
** messages above a certain severity level are logged. They can also have the 
** library log to several targets (files or callbacks) simultanously using 
** different severity levels. <BR>
** Note that logging might impair performance. Also, the lower, the log level, 
** the more log messages usually are to be expected. 
** 
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_LOG_H)
/*! \def PTB_LOG_H header define */
#define PTB_LOG_H

/******************************************************************************/

#include "ptbTypes.h"

/******************************************************************************/

#include "ptbProlog.h"

/******************************************************************************/

/*! Message severity levels */
enum PTB_ELogLevel { PTB_ellLog   = 1 /*!< Log messages                */
                   , PTB_ellInfo      /*!< Informational message       */
                   , PTB_ellWarn      /*!< Warning messages            */
                   , PTB_ellError     /*!< Error messages              */
                   , PTB_ellFatal     /*!< Fatal error messages        */
                   };

/*! callas pdfEngine SDK log callback type
** 
** \sa PTB_LoggingStartCBLog()
*/
typedef void (*PTB_LogCB)( enum PTB_ELogLevel level     /*!< Message severity    */
                         , PTB_StringID       idStr     /*!< Message string data */
                         , void*              userData  /*!< Message user data   */
                         );

/*!
** \brief  Activate logging to file
** 
** \retval             PTB_eerrNone  No error
** \retval  PTB_eerrDestVolumeExist  Destination volume does not exist
** \retval  PTB_eerrDestFolderExist  Destination folder does not exist 
** \retval PTB_eerrDestFolderCreate  Destination folder could not be created 
** \retval        PTB_eerrDestWrite  No write access to destination
** \retval  PTB_eerrFileNameInvalid  Invalid file name
** 
** \note   Log messages will be appended to the file. 
** \note   Pass \a idLog to PTB_LoggingStopLog() to stop logging to the file. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LoggingStartFileLog( const PTB_Path_t*    logFile  /*!< Log file             */
                       , enum PTB_ELogLevel   level    /*!< Log level            */
                       , PTB_LoggingID*       idLog    /*!< Resulting logging ID */
                       );

/*!
** \brief  Activate logging to callback
** 
** \retval    PTB_eerrNone  No error
** 
** \note   Pass \a idLog to PTB_LoggingStopLog() to stop logging to the 
**         callback. 
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LoggingStartCBLog( PTB_LogCB           cbLog      /*!< Logging callback. Must not be NULL.     */
                     , void*               userData   /*!< User data submitted to logging callback */
                     , enum PTB_ELogLevel  level      /*!< Message severity                        */
                     , PTB_LoggingID*      idLog      /*!< Resulting logging ID                    */
                     );

/*!
** \brief  Stops a specific logger
** 
** \retval       PTB_eerrNone  No error
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LoggingStopLog( PTB_LoggingID idLog /*!< Logging ID as returned by PTB_LoggingStartFileLog() or PTB_LoggingStartCBLog() */
                  );

/*!
** \brief  Activate trace logging (PTB_Preflight only)
** 
** \retval       PTB_eerrNone  No error
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LoggingStartTraceLog( const PTB_Path_t* path                 /*!< Path for the tracing log files */
                        );
/*!
** \brief  Stops trace logging
** 
** \retval       PTB_eerrNone  No error
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_LoggingStopTraceLog();

/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_LOG_H */

/******************************************************************************/
/* EOF */
