/*
** $Id: lundump.c $
** load precompiled Vmk chunks
** See Copyright Notice in vmk.h
*/

#define lundump_c
#define VMK_CORE

#include "lprefix.h"


#include <limits.h>
#include <string.h>

#include "vmk.h"

#include "ldebug.h"
#include "ldo.h"
#include "lfunc.h"
#include "lmem.h"
#include "lobject.h"
#include "lstring.h"
#include "lundump.h"
#include "lzio.h"


#if !defined(vmki_verifycode)
#define vmki_verifycode(L,f)  /* empty */
#endif


typedef struct {
  vmk_State *L;
  ZIO *Z;
  const char *name;
} LoadState;


static l_noret error (LoadState *S, const char *why) {
  vmkO_pushfstring(S->L, "%s: bad binary format (%s)", S->name, why);
  vmkD_throw(S->L, VMK_ERRSYNTAX);
}


/*
** All high-level loads go through loadVector; you can change it to
** adapt to the endianness of the input
*/
#define loadVector(S,b,n)	loadBlock(S,b,(n)*sizeof((b)[0]))

static void loadBlock (LoadState *S, void *b, size_t size) {
  if (vmkZ_read(S->Z, b, size) != 0)
    error(S, "truncated chunk");
}


#define loadVar(S,x)		loadVector(S,&x,1)


static lu_byte loadByte (LoadState *S) {
  int b = zgetc(S->Z);
  if (b == EOZ)
    error(S, "truncated chunk");
  return cast_byte(b);
}


static size_t loadUnsigned (LoadState *S, size_t limit) {
  size_t x = 0;
  int b;
  limit >>= 7;
  do {
    b = loadByte(S);
    if (x >= limit)
      error(S, "integer overflow");
    x = (x << 7) | (b & 0x7f);
  } while ((b & 0x80) == 0);
  return x;
}


static size_t loadSize (LoadState *S) {
  return loadUnsigned(S, MAX_SIZET);
}


static int loadInt (LoadState *S) {
  return cast_int(loadUnsigned(S, INT_MAX));
}


static vmk_Number loadNumber (LoadState *S) {
  vmk_Number x;
  loadVar(S, x);
  return x;
}


static vmk_Integer loadInteger (LoadState *S) {
  vmk_Integer x;
  loadVar(S, x);
  return x;
}


/*
** Load a nullable string into prototype 'p'.
*/
static TString *loadStringN (LoadState *S, Proto *p) {
  vmk_State *L = S->L;
  TString *ts;
  size_t size = loadSize(S);
  if (size == 0)  /* no string? */
    return NULL;
  else if (--size <= VMKI_MAXSHORTLEN) {  /* short string? */
    char buff[VMKI_MAXSHORTLEN];
    loadVector(S, buff, size);  /* load string into buffer */
    ts = vmkS_newlstr(L, buff, size);  /* create string */
  }
  else {  /* long string */
    ts = vmkS_createlngstrobj(L, size);  /* create string */
    setsvalue2s(L, L->top.p, ts);  /* anchor it ('loadVector' can GC) */
    vmkD_inctop(L);
    loadVector(S, getlngstr(ts), size);  /* load directly in final place */
    L->top.p--;  /* pop string */
  }
  vmkC_objbarrier(L, p, ts);
  return ts;
}


/*
** Load a non-nullable string into prototype 'p'.
*/
static TString *loadString (LoadState *S, Proto *p) {
  TString *st = loadStringN(S, p);
  if (st == NULL)
    error(S, "bad format for constant string");
  return st;
}


static void loadCode (LoadState *S, Proto *f) {
  int n = loadInt(S);
  f->code = vmkM_newvectorchecked(S->L, n, Instruction);
  f->sizecode = n;
  loadVector(S, f->code, n);
}


static void loadFunction(LoadState *S, Proto *f, TString *psource);


static void loadConstants (LoadState *S, Proto *f) {
  int i;
  int n = loadInt(S);
  f->k = vmkM_newvectorchecked(S->L, n, TValue);
  f->sizek = n;
  for (i = 0; i < n; i++)
    setnilvalue(&f->k[i]);
  for (i = 0; i < n; i++) {
    TValue *o = &f->k[i];
    int t = loadByte(S);
    switch (t) {
      case VMK_VNIL:
        setnilvalue(o);
        break;
      case VMK_VFALSE:
        setbfvalue(o);
        break;
      case VMK_VTRUE:
        setbtvalue(o);
        break;
      case VMK_VNUMFLT:
        setfltvalue(o, loadNumber(S));
        break;
      case VMK_VNUMINT:
        setivalue(o, loadInteger(S));
        break;
      case VMK_VSHRSTR:
      case VMK_VLNGSTR:
        setsvalue2n(S->L, o, loadString(S, f));
        break;
      default: vmk_assert(0);
    }
  }
}


static void loadProtos (LoadState *S, Proto *f) {
  int i;
  int n = loadInt(S);
  f->p = vmkM_newvectorchecked(S->L, n, Proto *);
  f->sizep = n;
  for (i = 0; i < n; i++)
    f->p[i] = NULL;
  for (i = 0; i < n; i++) {
    f->p[i] = vmkF_newproto(S->L);
    vmkC_objbarrier(S->L, f, f->p[i]);
    loadFunction(S, f->p[i], f->source);
  }
}


/*
** Load the upvalues for a function. The names must be filled first,
** because the filling of the other fields can raise read errors and
** the creation of the error message can call an emergency collection;
** in that case all prototypes must be consistent for the GC.
*/
static void loadUpvalues (LoadState *S, Proto *f) {
  int i, n;
  n = loadInt(S);
  f->upvalues = vmkM_newvectorchecked(S->L, n, Upvaldesc);
  f->sizeupvalues = n;
  for (i = 0; i < n; i++)  /* make array valid for GC */
    f->upvalues[i].name = NULL;
  for (i = 0; i < n; i++) {  /* following calls can raise errors */
    f->upvalues[i].instack = loadByte(S);
    f->upvalues[i].idx = loadByte(S);
    f->upvalues[i].kind = loadByte(S);
  }
}


static void loadDebug (LoadState *S, Proto *f) {
  int i, n;
  n = loadInt(S);
  f->lineinfo = vmkM_newvectorchecked(S->L, n, ls_byte);
  f->sizelineinfo = n;
  loadVector(S, f->lineinfo, n);
  n = loadInt(S);
  f->abslineinfo = vmkM_newvectorchecked(S->L, n, AbsLineInfo);
  f->sizeabslineinfo = n;
  for (i = 0; i < n; i++) {
    f->abslineinfo[i].pc = loadInt(S);
    f->abslineinfo[i].line = loadInt(S);
  }
  n = loadInt(S);
  f->locvars = vmkM_newvectorchecked(S->L, n, LocVar);
  f->sizelocvars = n;
  for (i = 0; i < n; i++)
    f->locvars[i].varname = NULL;
  for (i = 0; i < n; i++) {
    f->locvars[i].varname = loadStringN(S, f);
    f->locvars[i].startpc = loadInt(S);
    f->locvars[i].endpc = loadInt(S);
  }
  n = loadInt(S);
  if (n != 0)  /* does it have debug information? */
    n = f->sizeupvalues;  /* must be this many */
  for (i = 0; i < n; i++)
    f->upvalues[i].name = loadStringN(S, f);
}


static void loadFunction (LoadState *S, Proto *f, TString *psource) {
  f->source = loadStringN(S, f);
  if (f->source == NULL)  /* no source in dump? */
    f->source = psource;  /* reuse parent's source */
  f->linedefined = loadInt(S);
  f->lastlinedefined = loadInt(S);
  f->numparams = loadByte(S);
  f->is_vararg = loadByte(S);
  f->maxstacksize = loadByte(S);
  loadCode(S, f);
  loadConstants(S, f);
  loadUpvalues(S, f);
  loadProtos(S, f);
  loadDebug(S, f);
}


static void checkliteral (LoadState *S, const char *s, const char *msg) {
  char buff[sizeof(VMK_SIGNATURE) + sizeof(VMKC_DATA)]; /* larger than both */
  size_t len = strlen(s);
  loadVector(S, buff, len);
  if (memcmp(s, buff, len) != 0)
    error(S, msg);
}


static void fchecksize (LoadState *S, size_t size, const char *tname) {
  if (loadByte(S) != size)
    error(S, vmkO_pushfstring(S->L, "%s size mismatch", tname));
}


#define checksize(S,t)	fchecksize(S,sizeof(t),#t)

static void checkHeader (LoadState *S) {
  /* skip 1st char (already read and checked) */
  checkliteral(S, &VMK_SIGNATURE[1], "not a binary chunk");
  if (loadByte(S) != VMKC_VERSION)
    error(S, "version mismatch");
  if (loadByte(S) != VMKC_FORMAT)
    error(S, "format mismatch");
  checkliteral(S, VMKC_DATA, "corrupted chunk");
  checksize(S, Instruction);
  checksize(S, vmk_Integer);
  checksize(S, vmk_Number);
  if (loadInteger(S) != VMKC_INT)
    error(S, "integer format mismatch");
  if (loadNumber(S) != VMKC_NUM)
    error(S, "float format mismatch");
}


/*
** Load precompiled chunk.
*/
LClosure *vmkU_undump(vmk_State *L, ZIO *Z, const char *name) {
  LoadState S;
  LClosure *cl;
  if (*name == '@' || *name == '=')
    S.name = name + 1;
  else if (*name == VMK_SIGNATURE[0])
    S.name = "binary string";
  else
    S.name = name;
  S.L = L;
  S.Z = Z;
  checkHeader(&S);
  cl = vmkF_newLclosure(L, loadByte(&S));
  setclLvalue2s(L, L->top.p, cl);
  vmkD_inctop(L);
  cl->p = vmkF_newproto(L);
  vmkC_objbarrier(L, cl, cl->p);
  loadFunction(&S, cl->p, NULL);
  vmk_assert(cl->nupvalues == cl->p->sizeupvalues);
  vmki_verifycode(L, cl->p);
  return cl;
}

