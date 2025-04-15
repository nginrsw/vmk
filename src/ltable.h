/*
** $Id: ltable.h $
** Vmk tables (hash)
** See Copyright Notice in vmk.h
*/

#ifndef ltable_h
#define ltable_h

#include "lobject.h"


#define gnode(t,i)	(&(t)->node[i])
#define gval(n)		(&(n)->i_val)
#define gnext(n)	((n)->u.next)


/*
** Clear all bits of fast-access metamethods, which means that the table
** may have any of these metamethods. (First access that fails after the
** clearing will set the bit again.)
*/
#define invalidateTMcache(t)	((t)->flags &= ~maskflags)


/* true when 't' is using 'dummynode' as its hash part */
#define isdummy(t)		((t)->lastfree == NULL)


/* allocated size for hash nodes */
#define allocsizenode(t)	(isdummy(t) ? 0 : sizenode(t))


/* returns the Node, given the value of a table entry */
#define nodefromval(v)	cast(Node *, (v))


VMKI_FUNC const TValue *vmkH_getint (Table *t, vmk_Integer key);
VMKI_FUNC void vmkH_setint (vmk_State *L, Table *t, vmk_Integer key,
                                                    TValue *value);
VMKI_FUNC const TValue *vmkH_getshortstr (Table *t, TString *key);
VMKI_FUNC const TValue *vmkH_getstr (Table *t, TString *key);
VMKI_FUNC const TValue *vmkH_get (Table *t, const TValue *key);
VMKI_FUNC void vmkH_set (vmk_State *L, Table *t, const TValue *key,
                                                 TValue *value);
VMKI_FUNC void vmkH_finishset (vmk_State *L, Table *t, const TValue *key,
                                       const TValue *slot, TValue *value);
VMKI_FUNC Table *vmkH_new (vmk_State *L);
VMKI_FUNC void vmkH_resize (vmk_State *L, Table *t, unsigned int nasize,
                                                    unsigned int nhsize);
VMKI_FUNC void vmkH_resizearray (vmk_State *L, Table *t, unsigned int nasize);
VMKI_FUNC void vmkH_free (vmk_State *L, Table *t);
VMKI_FUNC int vmkH_next (vmk_State *L, Table *t, StkId key);
VMKI_FUNC vmk_Unsigned vmkH_getn (Table *t);
VMKI_FUNC unsigned int vmkH_realasize (const Table *t);


#if defined(VMK_DEBUG)
VMKI_FUNC Node *vmkH_mainposition (const Table *t, const TValue *key);
#endif


#endif
