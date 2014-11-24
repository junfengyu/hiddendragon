/* config.h.  Generated automatically by configure.  */
/* config.h.in.  Generated automatically from configure.in by autoheader.  */

/* Define if you have strcasecmp, as a function or macro.  */
#define HAVE_STRCASECMP 1

/* Define if you have snprintf, as a function or macro.  */
#define HAVE_SNPRINTF 1

/* Define if you have a working `mmap' system call.  */
#define HAVE_MMAP 1

/* Define to `unsigned' if <sys/types.h> doesn't define.  */
/* #undef size_t */

/* Define if you have the ANSI C header files.  */
#define STDC_HEADERS 1

/* Define to `unsigned long long' if <sys/types.h> doesn't define.  */
/* #undef u_int64_t */

/* Define to `unsigned int' if <sys/types.h> doesn't define.  */
/* #undef u_int32_t */

/* Define to `unsigned short' if <sys/types.h> doesn't define.  */
/* #undef u_int16_t */

/* Define to `unsigned char' if <sys/types.h> doesn't define.  */
/* #undef u_int8_t */

/* Define if you have the getpagesize function.  */
#define HAVE_GETPAGESIZE 1

/* Define if you have the memcpy function.  */
#define HAVE_MEMCPY 1

/* Define if you have the strrchr function.  */
#define HAVE_STRRCHR 1

/* Define if you have the <fcntl.h> header file.  */
#define HAVE_FCNTL_H 1

/* Define if you have the <md5.h> header file.  */
/* #undef HAVE_MD5_H */

/* Define if you have the <err.h> header file.  */
#define HAVE_ERR_H 1

/* Define if you have the <unistd.h> header file.  */
#define HAVE_UNISTD_H 1

#if STDC_HEADERS || HAVE_STRING_H
# include <string.h>
#else
# ifndef HAVE_STRCHR
#  define strchr index
#  define strrchr rindex
# endif
char *strchr (), *strrchr ();
# ifndef HAVE_MEMCPY
#  define memcpy(d, s, n) bcopy ((s), (d), (n))
#  define memmove(d, s, n) bcopy ((s), (d), (n))
# endif
#endif

#ifndef HAVE_STRCASECMP
#define strcasecmp strcmp
#endif

#ifndef __P
#define __P(x)	x
#endif
