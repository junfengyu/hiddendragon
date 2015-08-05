#ifndef DEFS_H
#define DEFS_H


#ifdef __cplusplus
#ifndef DONT_USE_EXTERN_C
extern "C" {
#endif
#endif

typedef unsigned char       uchar;
typedef unsigned int        uint;
typedef unsigned long       ulong;
typedef unsigned long long  ulonglong;

#define M32(x)      ((x) & 0xFFFFFFFF)
#define ROL32(x, y) (M32((x) << y) | M32((x) >> (32-y)))

#ifdef DEBUG_ON
    #define DEBUG printf
#else
    #define DEBUG 
#endif


#ifdef __cplusplus
#ifndef DONT_USE_EXTERN_C
}
#endif
#endif

#endif
