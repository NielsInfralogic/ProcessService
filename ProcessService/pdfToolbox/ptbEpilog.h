/*!
**
** \file ptbEpilog.h
**
** \brief callas pdfEngine SDK: Header epilog
**
** This is used for struct alignment and function prototype definitions. 
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

#undef PTB_FUNC_PROTO

/******************************************************************************/

#if defined(PTB_EXPORT_PRAGMA)
#   pragma export reset
#endif

/******************************************************************************/

/*******************************************************************************
** Alignment
*/
#if defined(_WIN32) || defined(_M_X64)
#  pragma pack( pop)
#elif defined(PRAGMA_STRUCT_ALIGN)
#  if defined(__MWERKS__) || defined(__GNUC__)
#    pragma options align=reset
#  endif /* defined(__MWERKS__) || defined(__GNUC__) */
#endif /* defined(PRAGMA_STRUCT_ALIGN) */

#if defined(PTB_EXPORT_PRAGMA)
#	pragma export reset
#endif

/******************************************************************************/

/*******************************************************************************
** Avoid mangled names
*/

#if defined(__cplusplus)
} /*extern "C"*/
#endif /*defined(__cplusplus)*/

/******************************************************************************/

/******************************************************************************/
/* EOF */
