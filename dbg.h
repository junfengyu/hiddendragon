#ifndef __dbg_h__
#define __dbg_h__
#endif

#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d\n" M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "None":strerror(errno))
#define log_error(M, ...) fprintf(stderr, "[ERROR] %s:%d  " M "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#define log_warn(M, ...) fprintf(stderr, "[WARIN] %s:%d  " M "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#define log_info(M, ...) fprintf(stderr, "[INFO] %s:%d  " M "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#define check(A,M, ...) if(!(A)) {log_error(M, ##__VA_ARGS__);errno=0;goto error;}
#define sentinel(M, ...) {log_error(M, ##__VA_ARGS__);errno=0;goto error; }
#define check_mem(A) check(A,"Out of memory")
#define check_debug(A,M, ...) if(!(A)) {debug(M, ##__VA_ARGS__);errno=0;goto error;}
