#include "bd.h"
#include <stdio.h>

extern ulong aes_table[256];

extern void BD_G(ulong* src, ulong* target, ulong *key);
extern void BD_update(Dragon2_Ctx* ctx, ulonglong Oz[2]);

void BD_initkey(Dragon2_Ctx* ctx, ulong key[4])
{
	ulonglong Oz[2];
	ulong     EK[16];
    int i = 0, j=0;
	
	/* Generate the extended key from a short master key */
    for (i = 0; i < 4; i++) {
        for (j = 0; j < 4; j++) {
		    EK[i*4+j] = key[(i+j) & 3];
		}
	}
	
	/* of course, this gets optimized away in non-reference code */
    for (i = 0; i < 8; i++) {
		ctx->W[i]   = EK[i*2];
		ctx->W[i] <<= 32;
		ctx->W[i]  |= EK[i*2+1];
	}
	ctx->W[1] ^= 0x80; /* key length is 128 bits; note it is added to the first
	                    * 128-bit word, which is the second 64-bit word */
	                    
	ctx->M[0]  = 0;
	ctx->M[1]  = 0;
	ctx->index = 0;
	
	BD_show_state(ctx);
	
    for (i = 0; i < 4; i++) {
		BD_update(ctx, Oz);
		ctx->M[0] = Oz[0];
		ctx->M[1] = Oz[1];
		BD_show_state(ctx);
	}
}

void BD_initiv(Dragon2_Ctx* ctx, ulong iv[4])
{
	ulonglong Oz[2];
	ulonglong EIV[8];
    int i=0;
	
	EIV[0] = ((ulonglong)iv[0] << 32) | iv[1];
	EIV[1] = ((ulonglong)iv[2] << 32) | iv[3];
	EIV[2] = ((ulonglong)iv[1] << 32) | iv[2];
	EIV[3] = (((ulonglong)iv[3] << 32) | iv[0]) + 1;
	
	if (EIV[3] == 0) EIV[2]++;
	
	EIV[4] = ((ulonglong)iv[2] << 32) | iv[3];
	EIV[5] = ((ulonglong)iv[0] << 32) | iv[1];
	
	if (EIV[5] == 0) EIV[4]--;
	EIV[5]--;
	
	EIV[6] = ((ulonglong)iv[3] << 32) | iv[0];
	EIV[7] = ((ulonglong)iv[1] << 32) | iv[2];
	
	if (EIV[7] == 0) EIV[6]--;
	EIV[7]--;
	EIV[6] += 0x8000000000000000ULL;
	
	DEBUG("Extended IV: %016llX %016llX\n"
	      "             %016llX %016llX\n"
		  "             %016llX %016llX\n"
		  "             %016llX %016llX\n", 
		  EIV[0], EIV[1], EIV[2], EIV[3], EIV[4], EIV[5], EIV[6], EIV[7]);

	/* of course, this gets optimized away in non-reference code */
    for (i = 0; i < 8; i++) {
		ctx->W[i] ^= EIV[i];
	}
	BD_show_state(ctx);
	
    for (i = 0; i < 4; i++) {
		BD_update(ctx, Oz);
		ctx->M[0] = Oz[0];
		ctx->M[1] = Oz[1];
		BD_show_state(ctx);
	}
}

void BD_keystream(Dragon2_Ctx* ctx, ulonglong* ks, const ulong num_clks)
{
	ulonglong Oz[2];
	ulonglong t;
    int i=0;
    for (i = 0; i < num_clks; i++) {
		BD_update(ctx, Oz);
		ks[i*2]   = Oz[0];
		ks[i*2+1] = Oz[1];
		
		t = ctx->M[1];
		ctx->M[1] += Oz[1];
		ctx->M[0] += Oz[0] + ((ctx->M[1] < t) ? 1 : 0);
	}
};

#define WI(x) (ctx->W[(ctx->index + x) & 0x7])

/*--------------------DIAGNOSTIC FUNCTION------------------------------------*/
void BD_show_state(Dragon2_Ctx* ctx) 
{
#ifdef DEBUG_ON
	printf("\nState dump\n-------------\nS:\n    ");
	for (int i = 0; i < 8; i++) {
		printf("%016llX ", WI(i));
		if ((i %4) == 3) printf("\n    ");		
	}
	printf("\nM:  %016llX|%016llX\n\n", ctx->M[0], ctx->M[1]);
#endif
}

/*--------------------INTERNAL FUNCTIONS-------------------------------------*/
#define MIX(a, b, c, d) \
    b ^= a; \
    d ^= c; \
    c += b; \
    a += d;

void F1(const ulonglong in[2], ulonglong out[2]) {
	static ulong key[]= { 0, 0, 0, 0 };
	
	ulong *i = (ulong*)in;
	ulong *o = (ulong*)out;
	
	DEBUG("F1-in :  %016llX | %016llX\n", in[0], in[1]);
	
	BD_G(i, o, key);	
	MIX(o[0], o[1], o[2], o[3]);
		
	DEBUG("F1-o  : %16llX% | %16llX\n", out[0], out[1]);
}

void F2(const ulonglong in[2], ulonglong out[2], const ulonglong key[2]) {
	
	ulong *i = (ulong*)in;
	ulong *o = (ulong*)out;
	ulong *k = (ulong*)key;
	ulong t[4];
	
	DEBUG("F2-in :  %016llX|%016llX, %016llX|%016llX\n", in[0], in[1], key[0], key[1]);
		
	out[0] = in[0];
	out[1] = in[1];
	
	MIX(o[0], o[1], o[2], o[3]);
	MIX(o[0], o[1], o[2], o[3]);
	BD_G(o, t, k);
	
	o[0] ^= t[0];
	o[1] ^= t[1];
	o[2] ^= t[2];
	o[3] ^= t[3];
	
	DEBUG("F2-ot:   %016llX|%016llX\n", out[0], out[1]);
} 

void BD_update(Dragon2_Ctx* ctx, ulonglong Oz[2]) {
	ulonglong If[2], Iz[2], Of[2];
	
	If[0] = WI(0) ^ WI(2) ^ WI(6);
	If[1] = WI(1) ^ WI(3) ^ WI(7);
	F1(If, Of);
	
	Iz[0] = WI(4) ^ ctx->M[0];
	Iz[1] = WI(5) ^ ctx->M[1];
	F2(Iz, Oz, Of);
	
	ctx->index += 2;
	WI(6) = Of[0];
	WI(7) = Of[1];
}

/*--------------------G FUNCTION-----------------------------------------*/
#define T(x)     aes_table[(x) & 0xFF]

ulong TLOOKUP(ulong w0, ulong w1, ulong w2, ulong w3)
{
	ulong r = 
	    T(w0) ^
		ROL32(T(w1 >> 8), 8) ^
		ROL32(T(w2 >> 16), 16) ^
		ROL32(T(w3 >> 24), 24);
	return r;
}

void BD_G(ulong* src, ulong* target, ulong* key)
{
	DEBUG("G[AES]: (%08X%08X %08X%08X, %08X%08X %08X%08X)\n",
	    src[0], src[1], src[2], src[3], key[0], key[1], key[2], key[3]);
		
	target[0] = TLOOKUP(src[0], src[1], src[2], src[3]) ^ key[0];
	target[1] = TLOOKUP(src[1], src[2], src[3], src[0]) ^ key[1];
	target[2] = TLOOKUP(src[2], src[3], src[0], src[1]) ^ key[2];
	target[3] = TLOOKUP(src[3], src[0], src[1], src[2]) ^ key[3];
	
	DEBUG("\t-->%08X%08X %08X%08X\n", 
	    target[0], target[1], target[2], target[3]);
}

