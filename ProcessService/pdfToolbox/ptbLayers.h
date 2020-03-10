/*!
**
** \file ptbLayers.h
**
** \brief callas pdfEngine SDK: Layers actions
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#if !defined(PTB_LAYERS_H)
/*! \def PTB_LAYERS_H header define */
#define PTB_LAYERS_H

/******************************************************************************/

#include "ptbTypes.h"
#include "ptbProgress.h"

/******************************************************************************/

#include "ptbProlog.h"

/*******************************************************************************
** Layers
*/

/*!
** \brief Enumarate layers
**        Enumerate the chosen objects on separate layers.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_EnumerateLayers( const PTB_Path_t*        src                 /*!< Document */
                   , PTB_bool_t               spotnames           /*!< Creates a layer for each spot color in the PDF */
                   , PTB_bool_t               fontnames           /*!< Creates a layer for each font in the PDF */
                   , PTB_bool_t               iccnames            /*!< Creates a layer for each ICC profile in the PDF */
                   , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
                   , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                   , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
                   );


/*!
** \brief  Import as layer
**         Imports the chosen document as a layer in the processed PDF.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ImportAsLayer( const PTB_Path_t*        src                 /*!< Document */
                 , const PTB_Path_t*        importFile          /*!< Full path to a PDF to be imported */
                 , const PTB_utf8_char_t*   layername           /*!< Name of the new layer */
                 , const PTB_Path_t*        dest                /*!< Destination path (including file name) */
                 , PTB_ProgressCB           cbProgress          /*!< Progress CB function to be called during processing, might be NULL */
                 , void*                    cbProgressUserData  /*!< User data submitted to progress callback function */
                 );


/*!
** \brief  Layers type
*/
enum PTB_ELayerType { PTB_eOCG      /*!<Optional Content Groups */
                    , PTB_eOCCD     /*!<Optional Content Configuration Dictionary */
                    };

/*!
** \brief  Callback for PTB_SplitLayers. Called for every created document.
*/
typedef void(*PTB_SplitLayersCB)( PTB_StringID         idPath    /*!< Created file */
                                , PTB_StringID         idName    /*!< Name of OCG or OCCD */
                                , enum PTB_ELayerType  type      /*!< Type of layer (OCG or OCCD) */
                                , PTB_int32_t          page      /*!< Page (-1: all pages) */
                                , void*                userData  /*!< User data */
                                );
/*!
** \brief  Split layers
**         Splits layers/layer views into single PDFs
**         with just the content of the layers/layer views.
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_SplitLayers( const PTB_Path_t*        src                   /*!< Document */
               , PTB_bool_t               singlePages           /*!< Create one new file per page */
               , const PTB_Path_t*        destFolder            /*!< Destination folder path (including folder name) */
               , PTB_SplitLayersCB        cbSplitLayers         /*!< Callback function to be called for every created PDF, might be NULL */
               , void*                    cbSplitLayersUserData /*!< User data submitted to callback function */
               , PTB_ProgressCB           cbProgress            /*!< Progress Callback function to be called during processing, might be NULL */
               , void*                    cbProgressUserData    /*!< User data submitted to progress callback function */
               );

/*!
** \brief Apply layer views
**        Apply layer views that configured in a config file to the PDF document 
**
*/
PTB_FUNC_PROTO 
enum PTB_EError 
PTB_ApplyLayerViews( const PTB_Path_t*        src                   /*!< Document */
                   , const PTB_Path_t*        occdConfigFile        /*!< Config file that described the layer views (OCCD) */
                   , const PTB_Path_t*        ocgConfigFile         /*!< Config file that described the layer metadata (OCG metadata) */
                   , const PTB_Path_t*        dest                  /*!< Destination path (including file name) */
                   );


/*******************************************************************************/

#include "ptbEpilog.h"

/******************************************************************************/

#endif /*PTB_LAYERS_H*/

/******************************************************************************/
/* EOF */
