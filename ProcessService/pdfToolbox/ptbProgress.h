/*!
** 
** \file ptbProgress.h
** 
** \brief callas pdfEngine SDK: Progress callbacks
** 
** This defines the type of the progress callback function used throughout the 
** callas pdfEngine SDK API
** 
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_PROGRESS_H)
/*! \def PTB_PROGRESS_H header define */
#define PTB_PROGRESS_H

/******************************************************************************/

#include "ptbTypes.h"

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** Progress callbacks
*/

/*!
** \brief  PTB_EPreflightPart provides the Preflight part usable in progress callback
*/
enum PTB_EPreflightPart { PTB_eppPrepare               /*!< Preflight prepare the given input PDF file and resources        */
                        , PTB_eppPreCheck              /*!< Precheck part of preflight is in process                        */
                        , PTB_eppPreCheckDone          /*!< Precheck part of preflight is done without problems.            */
                        , PTB_eppPreCheckHitProblems   /*!< Precheck part of preflight is done and has found some problems. */
                        , PTB_eppFixup                 /*!< Fixup part of preflight is in process                           */
                        , PTB_eppFixupDone             /*!< Fixup part of preflight is done                                 */
                        , PTB_eppPostCheck             /*!< Postcheck part of preflight is in process                       */
                        , PTB_eppPostCheckDone         /*!< Postcheck part of preflight is done                             */
                        , PTB_eppFixupSuccess          /*!< A Fixup was processed successfully                              */
                        , PTB_eppFixupFailure          /*!< A Fixup was not processed successfully                          */
                        , PTB_eppFixupNotRequired      /*!< No modification was required for an aspect of the pdf file      */
                        , PTB_eppExternalCheck         /*!< Preflight of external referenced pdf files is in process        */
                        };

/*! \brief  Prototype to create a Preflight progress callback function */
typedef PTB_bool_t (*PTB_Preflight_ProgressCB)( PTB_uint32_t            max       /*!< Progress value max                */
                                              , PTB_uint32_t            current   /*!< Current progress value            */
                                              , enum PTB_EPreflightPart part      /*!< Current Preflight part in process */
								              , PTB_StringID            idStr     /*!< String data                       */
                                              , void*                   userData  /*!< User data                         */
                                              );

/*! \brief  Prototype to create a PRC progress callback function */
typedef PTB_bool_t (*PTB_PRC_ProgressCB)( PTB_uint32_t max        /*!< Progress value max                */
                                        , PTB_uint32_t current    /*!< Current progress value            */
							            , PTB_StringID idStr      /*!< String data                       */
                                        , void*        userData   /*!< User data                         */
                                        );

/*! \brief  Prototype to create a progress callback function */
typedef PTB_bool_t (*PTB_ProgressCB)( PTB_uint32_t max         /*!< Progress value max                */
                                    , PTB_uint32_t current     /*!< Current progress value            */
                                    , void*        userData    /*!< User data                         */
                                    );


/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_PROGRESS_H*/

/******************************************************************************/
/* EOF */
