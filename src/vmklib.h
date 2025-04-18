/*
** $Id: vmklib.h $
** Vmk standard libraries
** See Copyright Notice in vmk.h
*/


#ifndef vmklib_h
#define vmklib_h

#include "vmk.h"


/* version suffix for environment variable names */
#define VMK_VERSUFFIX          "_" VMK_VERSION_MAJOR "_" VMK_VERSION_MINOR


VMKMOD_API int (vmkopen_base) (vmk_State *L);

#define VMK_COLIBNAME	"coroutine"
VMKMOD_API int (vmkopen_coroutine) (vmk_State *L);

#define VMK_TABLIBNAME	"table"
VMKMOD_API int (vmkopen_table) (vmk_State *L);

#define VMK_IOLIBNAME	"io"
VMKMOD_API int (vmkopen_io) (vmk_State *L);

#define VMK_OSLIBNAME	"os"
VMKMOD_API int (vmkopen_os) (vmk_State *L);

#define VMK_STRLIBNAME	"string"
VMKMOD_API int (vmkopen_string) (vmk_State *L); /* Juga tersedia sebagai 'str' melalui alias */

#define VMK_UTF8LIBNAME	"utf8"
VMKMOD_API int (vmkopen_utf8) (vmk_State *L);

#define VMK_MATHLIBNAME	"math"
VMKMOD_API int (vmkopen_math) (vmk_State *L);

#define VMK_DBLIBNAME	"debug"
VMKMOD_API int (vmkopen_debug) (vmk_State *L);

#define VMK_LOADLIBNAME	"package"
VMKMOD_API int (vmkopen_package) (vmk_State *L);


/* open all previous libraries */
VMKLIB_API void (vmkL_openlibs) (vmk_State *L);


#endif
