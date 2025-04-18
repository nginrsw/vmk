/*
** $Id: linit.c $
** Initialization of libraries for vmk.c and other clients
** See Copyright Notice in vmk.h
*/


#define linit_c
#define VMK_LIB

/*
** If you embed Vmk in your program and need to open the standard
** libraries, call vmkL_openlibs in your program. If you need a
** different set of libraries, copy this file to your project and edit
** it to suit your needs.
**
** You can also *preload* libraries, so that a later 'require' can
** open the library, which is already linked to the application.
** For that, do the following code:
**
**  vmkL_getsubtable(L, VMK_REGISTRYINDEX, VMK_PRELOAD_TABLE);
**  vmk_pushcfunction(L, vmkopen_modname);
**  vmk_setfield(L, -2, modname);
**  vmk_pop(L, 1);  // remove PRELOAD table
*/

#include "lprefix.h"

#include <stddef.h>
#include <string.h>  /* Ditambahkan untuk strcmp */

#include "vmk.h"

#include "vmklib.h"
#include "lauxlib.h"


/*
** these libs are loaded by vmk.c and are readily available to any Vmk
** program
*/
static const vmkL_Reg loadedlibs[] = {
  {VMK_GNAME, vmkopen_base},
  {VMK_LOADLIBNAME, vmkopen_package},
  {VMK_COLIBNAME, vmkopen_coroutine},
  {VMK_TABLIBNAME, vmkopen_table},
  {VMK_IOLIBNAME, vmkopen_io},
  {VMK_OSLIBNAME, vmkopen_os},
  {VMK_STRLIBNAME, vmkopen_string},
  {VMK_MATHLIBNAME, vmkopen_math},
  {VMK_UTF8LIBNAME, vmkopen_utf8},
  {VMK_DBLIBNAME, vmkopen_debug},
  {NULL, NULL}
};


VMKLIB_API void vmkL_openlibs (vmk_State *L) {
  const vmkL_Reg *lib;
  /* "require" functions from 'loadedlibs' and set results to global table */
  for (lib = loadedlibs; lib->func; lib++) {
    vmkL_requiref(L, lib->name, lib->func, 1);
	if (strcmp(lib->name, VMK_STRLIBNAME) == 0) {
       vmk_getglobal(L, "str");
       if (vmk_isnil(L, -1)) {  /* Jika 'str' belum ada */
         vmk_pop(L, 1);
         vmk_pushvalue(L, -1);
         vmk_setglobal(L, "str");
       } else {
         vmk_pop(L, 1);  /* Jangan timpa 'str' yang sudah ada */
       }
    }
    vmk_pop(L, 1);  /* remove lib */
  }
}

