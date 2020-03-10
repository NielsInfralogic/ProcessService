/*!
**
** \file ptbProlog.h
**
** \brief callas pdfEngine SDK: Header prolog
**
** This is used for struct alignment and function prototype definitions. 
**
** \author &copy; 2009-2011 callas software GmbH, Berlin, Germany - www.callassoftware.com
**
*/

/******************************************************************************/

/*******************************************************************************
** Avoid mangled names
*/

#if defined(__cplusplus)
extern "C" {
#endif /*defined(__cplusplus)*/

/******************************************************************************/

#if defined(__MWERKS__) && defined(PTB_LIBRARY) && !defined(PTB_EXPORT_PRAGMA)
#	define PTB_EXPORT_PRAGMA
#endif

#if defined(PTB_EXPORT_PRAGMA)
#	pragma export on
#endif

/*******************************************************************************
** Alignment
*/
#if defined(_WIN32) || defined(_M_X64)
#  pragma pack( push, 8)
#elif defined(PRAGMA_STRUCT_ALIGN)
#  if defined(__MWERKS__)
#    pragma options align=power_gcc
#  elif defined(__GNUC__)
#    pragma options align=power
#  else /* __GNUC__ */
#    error
#  endif
#endif /* defined(PRAGMA_STRUCT_ALIGN) */

/*******************************************************************************
** Function prototypes
*/

#if defined(__APPLE__) || defined(macintosh) || defined(unix)
#	if defined( __GNUG__ ) && (__GNUG__ >= 4) && defined( PTB_LIBRARY_DLL)
#   	define   PTB_FUNC_PROTO __attribute__ ((visibility("default")))
#	else
#		define   PTB_FUNC_PROTO
#	endif // defined( __GNUG__ ) && (__GNUG__ >= 4) && defined( PTB_LIBRARY_DLL)
#elif defined(_WIN32) || defined(_M_X64)
#  if defined( _WINDLL) && defined( PTB_LIBRARY_DLL)
#    define    PTB_FUNC_PROTO    __declspec(dllexport)
#    define    PTB_STDCALL_CONV  _stdcall
#    define    PTB_STDCALL_EXPORT
#  elif defined( PTB_LIBRARY_IMPORT)
#    define    PTB_FUNC_PROTO    __declspec(dllimport)
#  else // PTB_LIBRARY_IMPORT
#    define    PTB_FUNC_PROTO    extern
#  endif // _WINDLL
#else
#  define   PTB_FUNC_PROTO
#endif // defined(macintosh)

/******************************************************************************/

/******************************************************************************/
/* EOF */
