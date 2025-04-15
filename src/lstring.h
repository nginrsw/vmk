/*
** $Id: lstring.h $
** String table (keep all strings handled by Vmk)
** See Copyright Notice in vmk.h
*/

#ifndef lstring_h
#define lstring_h

#include "lgc.h"
#include "lobject.h"
#include "lstate.h"


/*
** Memory-allocation error message must be preallocated (it cannot
** be created after memory is exhausted)
*/
#define MEMERRMSG       "not enough memory"


/*
** Size of a TString: Size of the header plus space for the string
** itself (including final '\0').
*/
#define sizelstring(l)  (offsetof(TString, contents) + ((l) + 1) * sizeof(char))

#define vmkS_newliteral(L, s)	(vmkS_newlstr(L, "" s, \
                                 (sizeof(s)/sizeof(char))-1))


/*
** test whether a string is a reserved word
*/
#define isreserved(s)	((s)->tt == VMK_VSHRSTR && (s)->extra > 0)


/*
** equality for short strings, which are always internalized
*/
#define eqshrstr(a,b)	check_exp((a)->tt == VMK_VSHRSTR, (a) == (b))


VMKI_FUNC unsigned int vmkS_hash (const char *str, size_t l, unsigned int seed);
VMKI_FUNC unsigned int vmkS_hashlongstr (TString *ts);
VMKI_FUNC int vmkS_eqlngstr (TString *a, TString *b);
VMKI_FUNC void vmkS_resize (vmk_State *L, int newsize);
VMKI_FUNC void vmkS_clearcache (global_State *g);
VMKI_FUNC void vmkS_init (vmk_State *L);
VMKI_FUNC void vmkS_remove (vmk_State *L, TString *ts);
VMKI_FUNC Udata *vmkS_newudata (vmk_State *L, size_t s, int nuvalue);
VMKI_FUNC TString *vmkS_newlstr (vmk_State *L, const char *str, size_t l);
VMKI_FUNC TString *vmkS_new (vmk_State *L, const char *str);
VMKI_FUNC TString *vmkS_createlngstrobj (vmk_State *L, size_t l);


#endif
