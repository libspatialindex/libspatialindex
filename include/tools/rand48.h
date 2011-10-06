#if HAVE_FEATURES_H
#include <features.h>
#ifndef __THROW
/* copy-pasted from sys/cdefs.h */
/* GCC can always grok prototypes.  For C++ programs we add throw()
to help it optimize the function calls.  But this works only with
gcc 2.8.x and egcs.  For gcc 3.2 and up we even mark C functions
as non-throwing using a function attribute since programs can use
the -fexceptions options for C code as well.  */
# if !defined __cplusplus && __GNUC_PREREQ (3, 3)
#  define __THROW      __attribute__ ((__nothrow__))
#  define __NTH(fct)   __attribute__ ((__nothrow__)) fct
# else
#  if defined __cplusplus && __GNUC_PREREQ (2,8)
#   define __THROW     throw ()
#   define __NTH(fct)  fct throw ()
#  else
#   define __THROW
#   define __NTH(fct)  fct
#  endif
# endif

#endif
#else
#   define __THROW
#   define __NTH(fct)  fct
#endif

#ifndef srand48
extern void srand48(long seed) __THROW;
#endif

#ifndef seed48
extern unsigned short *seed48(unsigned short xseed[3]) __THROW;
#endif

#ifndef nrand48
extern long nrand48(unsigned short xseed[3]) __THROW;
#endif

#ifndef mrand48
extern long mrand48(void) __THROW;
#endif

#ifndef lrand48
extern long lrand48(void) __THROW;
#endif

#ifndef lcong48
extern void lcong48(unsigned short p[7]) __THROW;
#endif

#ifndef jrand48
extern long jrand48(unsigned short xseed[3]) __THROW;
#endif

#ifndef erand48
extern double erand48(unsigned short xseed[3]) __THROW;
#endif

#ifndef drand48
extern double drand48(void) __THROW;
#endif
