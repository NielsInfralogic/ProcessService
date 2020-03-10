/*!
**
** \file ptbError.h
**
** \brief callas pdfEngine SDK: Error handling
** 
** Functions for retrieving textual error descriptions. 
** 
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_ERROR_H)
/*! \def PTB_ERROR_H header define */
#define PTB_ERROR_H

/******************************************************************************/

#include "ptbTypes.h"

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** Error
*/

/*!
** \brief   Get error message for given error code 
** 
**          Retrieves a textual error description for a given error code 
**          returned by most of the callas pdfEngine SDK API functions. 
** 
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_GetErrorMessage( enum PTB_EError  eerr     /*!< Error code, as returned from any of the library functions */
                   , PTB_StringID*    idStr    /*!< String data                                               */
                   );

PTB_FUNC_PROTO
enum PTB_EError 
PTB_GetErrorMessage2( enum PTB_EError  eerr        /*!< Error code, as returned from any of the library functions */
                    , PTB_StringID*    idStr       /*!< String data                                               */
                    , PTB_StringID*    idLastErr   /*!< Last error text                                           */
                   );

/*!
** \brief   Get the last internal error text for the given result 
** 
**          This can be helpful if you get 'PTB_eerrUnknown' or similar error codes
**          
*/
PTB_FUNC_PROTO
enum PTB_EError 
PTB_GetMachineErrorText( PTB_ResultID     idResult /*!< Result ID as returned by PTB_Preflight(), PTB_CheckPDFA() or PTB_ConvertPDFA() */
                       , PTB_StringID*    idStr    /*!< String data                                                                    */
                       );

/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_ERROR_H*/

/******************************************************************************/
/* EOF */
