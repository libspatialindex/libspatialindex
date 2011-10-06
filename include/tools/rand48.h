#pragma once

#ifndef srand48
extern void srand48(long seed);
#endif

#ifndef seed48
extern unsigned short *seed48(unsigned short xseed[3]);
#endif

#ifndef nrand48
extern long nrand48(unsigned short xseed[3]);
#endif

#ifndef mrand48
extern long mrand48(void);
#endif

#ifndef lrand48
extern long lrand48(void);
#endif

#ifndef lcong48
extern void lcong48(unsigned short p[7]);
#endif

#ifndef jrand48
extern long jrand48(unsigned short xseed[3]);
#endif

#ifndef erand48
extern double erand48(unsigned short xseed[3]);
#endif

#ifdef drand48
extern double drand48(void);
#endif