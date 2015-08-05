#ifndef BD_H
#define BD_H

#include "defs.h"

#ifdef __cplusplus
#ifndef DONT_USE_EXTERN_C
extern "C" {
#endif
#endif

/*------------------Internal functions---------------------------------------*/
/* Core function - customizable */
void BD_G(ulong* src, ulong* target, ulong* key);


/*------------------Public API---------------------------------------------- */
typedef struct {
	ulonglong  W[8];
	ulonglong  M[2];
	ulong      index;
} Dragon2_Ctx;

/* Init cipher with key */
void BD_initkey(Dragon2_Ctx* ctx, ulong key[4]);

/* Init cipher with IV */
void BD_initiv(Dragon2_Ctx* ctx, ulong iv[4]);

/* Produce keystream
 * Note: ks should be allocated for 2*num_clks 
 * Note: must be preceeded with BD_initkey and BD_initiv 
 */
void BD_keystream(Dragon2_Ctx* ctx, ulonglong* ks, const ulong num_clks);

/* Diagnostic - show the state */
void BD_show_state(Dragon2_Ctx* ctx);

#ifdef __cplusplus
#ifndef DONT_USE_EXTERN_C
}
#endif
#endif

#endif
