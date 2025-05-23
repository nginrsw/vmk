/*
** $Id: lvm.h $
** Vmk virtual machine
** See Copyright Notice in vmk.h
*/

#ifndef lvm_h
#define lvm_h


#include "ldo.h"
#include "lobject.h"
#include "ltm.h"


#if !defined(VMK_NOCVTN2S)
#define cvt2str(o)	ttisnumber(o)
#else
#define cvt2str(o)	0	/* no conversion from numbers to strings */
#endif


#if !defined(VMK_NOCVTS2N)
#define cvt2num(o)	ttisstring(o)
#else
#define cvt2num(o)	0	/* no conversion from strings to numbers */
#endif


/*
** You can define VMK_FLOORN2I if you want to convert floats to integers
** by flooring them (instead of raising an error if they are not
** integral values)
*/
#if !defined(VMK_FLOORN2I)
#define VMK_FLOORN2I		F2Ieq
#endif


/*
** Rounding modes for float->integer coercion
 */
typedef enum {
  F2Ieq,     /* no rounding; accepts only integral values */
  F2Ifloor,  /* takes the floor of the number */
  F2Iceil    /* takes the ceil of the number */
} F2Imod;


/* convert an object to a float (including string coercion) */
#define tonumber(o,n) \
	(ttisfloat(o) ? (*(n) = fltvalue(o), 1) : vmkV_tonumber_(o,n))


/* convert an object to a float (without string coercion) */
#define tonumberns(o,n) \
	(ttisfloat(o) ? ((n) = fltvalue(o), 1) : \
	(ttisinteger(o) ? ((n) = cast_num(ivalue(o)), 1) : 0))


/* convert an object to an integer (including string coercion) */
#define tointeger(o,i) \
  (l_likely(ttisinteger(o)) ? (*(i) = ivalue(o), 1) \
                          : vmkV_tointeger(o,i,VMK_FLOORN2I))


/* convert an object to an integer (without string coercion) */
#define tointegerns(o,i) \
  (l_likely(ttisinteger(o)) ? (*(i) = ivalue(o), 1) \
                          : vmkV_tointegerns(o,i,VMK_FLOORN2I))


#define intop(op,v1,v2) l_castU2S(l_castS2U(v1) op l_castS2U(v2))

#define vmkV_rawequalobj(t1,t2)		vmkV_equalobj(NULL,t1,t2)


/*
** fast track for 'gettable': if 't' is a table and 't[k]' is present,
** return 1 with 'slot' pointing to 't[k]' (position of final result).
** Otherwise, return 0 (meaning it will have to check metamethod)
** with 'slot' pointing to an empty 't[k]' (if 't' is a table) or NULL
** (otherwise). 'f' is the raw get function to use.
*/
#define vmkV_fastget(L,t,k,slot,f) \
  (!ttistable(t)  \
   ? (slot = NULL, 0)  /* not a table; 'slot' is NULL and result is 0 */  \
   : (slot = f(hvalue(t), k),  /* else, do raw access */  \
      !isempty(slot)))  /* result not empty? */


/*
** Special case of 'vmkV_fastget' for integers, inlining the fast case
** of 'vmkH_getint'.
*/
#define vmkV_fastgeti(L,t,k,slot) \
  (!ttistable(t)  \
   ? (slot = NULL, 0)  /* not a table; 'slot' is NULL and result is 0 */  \
   : (slot = (l_castS2U(k) - 1u < hvalue(t)->alimit) \
              ? &hvalue(t)->array[k - 1] : vmkH_getint(hvalue(t), k), \
      !isempty(slot)))  /* result not empty? */


/*
** Finish a fast set operation (when fast get succeeds). In that case,
** 'slot' points to the place to put the value.
*/
#define vmkV_finishfastset(L,t,slot,v) \
    { setobj2t(L, cast(TValue *,slot), v); \
      vmkC_barrierback(L, gcvalue(t), v); }


/*
** Shift right is the same as shift left with a negative 'y'
*/
#define vmkV_shiftr(x,y)	vmkV_shiftl(x,intop(-, 0, y))



VMKI_FUNC int vmkV_equalobj (vmk_State *L, const TValue *t1, const TValue *t2);
VMKI_FUNC int vmkV_lessthan (vmk_State *L, const TValue *l, const TValue *r);
VMKI_FUNC int vmkV_lessequal (vmk_State *L, const TValue *l, const TValue *r);
VMKI_FUNC int vmkV_tonumber_ (const TValue *obj, vmk_Number *n);
VMKI_FUNC int vmkV_tointeger (const TValue *obj, vmk_Integer *p, F2Imod mode);
VMKI_FUNC int vmkV_tointegerns (const TValue *obj, vmk_Integer *p,
                                F2Imod mode);
VMKI_FUNC int vmkV_flttointeger (vmk_Number n, vmk_Integer *p, F2Imod mode);
VMKI_FUNC void vmkV_finishget (vmk_State *L, const TValue *t, TValue *key,
                               StkId val, const TValue *slot);
VMKI_FUNC void vmkV_finishset (vmk_State *L, const TValue *t, TValue *key,
                               TValue *val, const TValue *slot);
VMKI_FUNC void vmkV_finishOp (vmk_State *L);
VMKI_FUNC void vmkV_execute (vmk_State *L, CallInfo *ci);
VMKI_FUNC void vmkV_concat (vmk_State *L, int total);
VMKI_FUNC vmk_Integer vmkV_idiv (vmk_State *L, vmk_Integer x, vmk_Integer y);
VMKI_FUNC vmk_Integer vmkV_mod (vmk_State *L, vmk_Integer x, vmk_Integer y);
VMKI_FUNC vmk_Number vmkV_modf (vmk_State *L, vmk_Number x, vmk_Number y);
VMKI_FUNC vmk_Integer vmkV_shiftl (vmk_Integer x, vmk_Integer y);
VMKI_FUNC void vmkV_objlen (vmk_State *L, StkId ra, const TValue *rb);

#endif
