/*
 * sha512.c - implementation of SHA224, SHA256, SHA384 and SHA512
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include <string.h>
#include <stdlib.h>

//little endian machine
#define IS_LITTLE_ENDIAN 1
//asm swap should be on for analyses that can't reason about assembly
#define ASM_SWAP 0
//ports
#define PORT_Alloc(size) malloc(size)
#define PORT_New(type) (type*)PORT_Alloc(sizeof(type))
#define PORT_Free(ctx) free(ctx)
#define PORT_Strlen(str) strlen(str)
#define PORT_Memcpy(destination, source, num) memcpy(destination, source, num)

//from nspr prtypes.h
typedef unsigned char PRUint8;
typedef unsigned int PRUint32;
typedef int PRIntn;
typedef PRIntn PRBool;
#define PR_TRUE 1
#define PR_FALSE 0
#define PR_MIN(x,y)     ((x)<(y)?(x):(y))


//from sha256.h
 struct SHA256ContextStr {
    union {
	PRUint32 w[64];	    /* message schedule, input buffer, plus 48 words */
	PRUint8  b[256];
    } u;
    PRUint32 h[8];		/* 8 state variables */
    PRUint32 sizeHi,sizeLo;	/* 64-bit count of hashed bytes. */
};

//from blapit.h
typedef struct SHA256ContextStr     SHA256Context;
#define SHA256_LENGTH           32      /* bytes */
#define SHA256_BLOCK_LENGTH 	 64 	/* bytes */

/* ============= Common constants and defines ======================= */

#define W ctx->u.w
#define B ctx->u.b
#define H ctx->h

#define SHR(x,n) (x >> n)
#define SHL(x,n) (x << n)
#define Ch(x,y,z)  ((x & y) ^ (~x & z))
#define Maj(x,y,z) ((x & y) ^ (x & z) ^ (y & z))
#define SHA_MIN(a,b) (a < b ? a : b)

/* Padding used with all flavors of SHA */
static const PRUint8 pad[240] = { 
0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
   0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
   /* compiler will fill the rest in with zeros */
};

/* ============= SHA256 implementation ================================== */

/* SHA-256 constants, K256. */
static const PRUint32 K256[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

/* SHA-256 initial hash values */
static const PRUint32 H256[8] = {
    0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a, 
    0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
};


static inline void be32enc(void *pp, PRUint32 x)
{
    PRUint8 * p = (PRUint8 *)pp;

    p[3] = x & 0xff;
    p[2] = (x >> 8) & 0xff;
    p[1] = (x >> 16) & 0xff;
    p[0] = (x >> 24) & 0xff;
}

static void
be32enc_vect(unsigned char *dst, const PRUint32 *src, size_t len)
{
    size_t i;

    for (i = 0; i < len / 4; i++) {
        be32enc(dst + i * 4, src[i]);
    }
}

static void be32enc_vect_inplace(PRUint32 *src, size_t len){
    size_t i;
    PRUint32 tmp;
    PRUint32 * pt;

    for(i=0; i<len/4; i++){
	be32enc(&tmp, src[i]);
        src[i] = tmp;
    }

}
#define HOST_c2l(c,l)	(l =(((unsigned long)(*((c)++)))<<24),		\
			 l|=(((unsigned long)(*((c)++)))<<16),		\
			 l|=(((unsigned long)(*((c)++)))<< 8),		\
			 l|=(((unsigned long)(*((c)++)))    ),		\
			 l)


static __inline__ PRUint32 swap4b(PRUint32 value)
{
    __asm__("bswap %0" : "+r" (value));
    return (value);
}
#define SHA_HTONL(x) swap4b(x)
#define BYTESWAP4(x)  x = SHA_HTONL(x)


#if defined(_MSC_VER)
#pragma intrinsic (_lrotr, _lrotl) 
#define ROTR32(x,n) _lrotr(x,n)
#define ROTL32(x,n) _lrotl(x,n)
#else
#define ROTR32(x,n) ((x >> n) | (x << ((8 * sizeof x) - n)))
#define ROTL32(x,n) ((x << n) | (x >> ((8 * sizeof x) - n)))
#endif

/* Capitol Sigma and lower case sigma functions */
#define S0(x) (ROTR32(x, 2) ^ ROTR32(x,13) ^ ROTR32(x,22))
#define S1(x) (ROTR32(x, 6) ^ ROTR32(x,11) ^ ROTR32(x,25))
#define s0(x) (t1 = x, ROTR32(t1, 7) ^ ROTR32(t1,18) ^ SHR(t1, 3))
#define s1(x) (t2 = x, ROTR32(t2,17) ^ ROTR32(t2,19) ^ SHR(t2,10))

SHA256Context *
SHA256_NewContext(void)
{
    SHA256Context *ctx = PORT_New(SHA256Context);
    return ctx;
}

void 
SHA256_DestroyContext(SHA256Context *ctx, PRBool freeit)
{
    memset(ctx, 0, sizeof *ctx);
    if (freeit) {
        PORT_Free(ctx);
    }
}

void 
SHA256_Begin(SHA256Context *ctx)
{
    memset(ctx, 0, sizeof *ctx);
    memcpy(H, H256, sizeof H256);
}

static void
SHA256_Compress(SHA256Context *ctx)
{
  {
    register PRUint32 t1, t2;

#if defined(IS_LITTLE_ENDIAN)
#if !ASM_SWAP
    be32enc_vect_inplace(W, 64);
#else 
    BYTESWAP4(W[0]);
    BYTESWAP4(W[1]);
    BYTESWAP4(W[2]);
    BYTESWAP4(W[3]);
    BYTESWAP4(W[4]);
    BYTESWAP4(W[5]);
    BYTESWAP4(W[6]);
    BYTESWAP4(W[7]);
    BYTESWAP4(W[8]);
    BYTESWAP4(W[9]);
    BYTESWAP4(W[10]);
    BYTESWAP4(W[11]);
    BYTESWAP4(W[12]);
    BYTESWAP4(W[13]);
    BYTESWAP4(W[14]);
    BYTESWAP4(W[15]);
#endif
#endif

#define INITW(t) W[t] = (s1(W[t-2]) + W[t-7] + s0(W[t-15]) + W[t-16])

    /* prepare the "message schedule"   */
#ifdef NOUNROLL256
    {
	int t;
	for (t = 16; t < 64; ++t) {
	    INITW(t);
	}
    }
#else
    INITW(16);
    INITW(17);
    INITW(18);
    INITW(19);

    INITW(20);
    INITW(21);
    INITW(22);
    INITW(23);
    INITW(24);
    INITW(25);
    INITW(26);
    INITW(27);
    INITW(28);
    INITW(29);

    INITW(30);
    INITW(31);
    INITW(32);
    INITW(33);
    INITW(34);
    INITW(35);
    INITW(36);
    INITW(37);
    INITW(38);
    INITW(39);

    INITW(40);
    INITW(41);
    INITW(42);
    INITW(43);
    INITW(44);
    INITW(45);
    INITW(46);
    INITW(47);
    INITW(48);
    INITW(49);

    INITW(50);
    INITW(51);
    INITW(52);
    INITW(53);
    INITW(54);
    INITW(55);
    INITW(56);
    INITW(57);
    INITW(58);
    INITW(59);

    INITW(60);
    INITW(61);
    INITW(62);
    INITW(63);

#endif
#undef INITW
  }
  {
    PRUint32 a, b, c, d, e, f, g, h;

    a = H[0];
    b = H[1];
    c = H[2];
    d = H[3];
    e = H[4];
    f = H[5];
    g = H[6];
    h = H[7];

#define ROUND(n,a,b,c,d,e,f,g,h) \
    h += S1(e) + Ch(e,f,g) + K256[n] + W[n]; \
    d += h; \
    h += S0(a) + Maj(a,b,c); 

#ifdef NOUNROLL256
    {
	int t;
	for (t = 0; t < 64; t+= 8) {
	    ROUND(t+0,a,b,c,d,e,f,g,h)
	    ROUND(t+1,h,a,b,c,d,e,f,g)
	    ROUND(t+2,g,h,a,b,c,d,e,f)
	    ROUND(t+3,f,g,h,a,b,c,d,e)
	    ROUND(t+4,e,f,g,h,a,b,c,d)
	    ROUND(t+5,d,e,f,g,h,a,b,c)
	    ROUND(t+6,c,d,e,f,g,h,a,b)
	    ROUND(t+7,b,c,d,e,f,g,h,a)
	}
    }
#else
    ROUND( 0,a,b,c,d,e,f,g,h)
    ROUND( 1,h,a,b,c,d,e,f,g)
    ROUND( 2,g,h,a,b,c,d,e,f)
    ROUND( 3,f,g,h,a,b,c,d,e)
    ROUND( 4,e,f,g,h,a,b,c,d)
    ROUND( 5,d,e,f,g,h,a,b,c)
    ROUND( 6,c,d,e,f,g,h,a,b)
    ROUND( 7,b,c,d,e,f,g,h,a)

    ROUND( 8,a,b,c,d,e,f,g,h)
    ROUND( 9,h,a,b,c,d,e,f,g)
    ROUND(10,g,h,a,b,c,d,e,f)
    ROUND(11,f,g,h,a,b,c,d,e)
    ROUND(12,e,f,g,h,a,b,c,d)
    ROUND(13,d,e,f,g,h,a,b,c)
    ROUND(14,c,d,e,f,g,h,a,b)
    ROUND(15,b,c,d,e,f,g,h,a)

    ROUND(16,a,b,c,d,e,f,g,h)
    ROUND(17,h,a,b,c,d,e,f,g)
    ROUND(18,g,h,a,b,c,d,e,f)
    ROUND(19,f,g,h,a,b,c,d,e)
    ROUND(20,e,f,g,h,a,b,c,d)
    ROUND(21,d,e,f,g,h,a,b,c)
    ROUND(22,c,d,e,f,g,h,a,b)
    ROUND(23,b,c,d,e,f,g,h,a)

    ROUND(24,a,b,c,d,e,f,g,h)
    ROUND(25,h,a,b,c,d,e,f,g)
    ROUND(26,g,h,a,b,c,d,e,f)
    ROUND(27,f,g,h,a,b,c,d,e)
    ROUND(28,e,f,g,h,a,b,c,d)
    ROUND(29,d,e,f,g,h,a,b,c)
    ROUND(30,c,d,e,f,g,h,a,b)
    ROUND(31,b,c,d,e,f,g,h,a)

    ROUND(32,a,b,c,d,e,f,g,h)
    ROUND(33,h,a,b,c,d,e,f,g)
    ROUND(34,g,h,a,b,c,d,e,f)
    ROUND(35,f,g,h,a,b,c,d,e)
    ROUND(36,e,f,g,h,a,b,c,d)
    ROUND(37,d,e,f,g,h,a,b,c)
    ROUND(38,c,d,e,f,g,h,a,b)
    ROUND(39,b,c,d,e,f,g,h,a)

    ROUND(40,a,b,c,d,e,f,g,h)
    ROUND(41,h,a,b,c,d,e,f,g)
    ROUND(42,g,h,a,b,c,d,e,f)
    ROUND(43,f,g,h,a,b,c,d,e)
    ROUND(44,e,f,g,h,a,b,c,d)
    ROUND(45,d,e,f,g,h,a,b,c)
    ROUND(46,c,d,e,f,g,h,a,b)
    ROUND(47,b,c,d,e,f,g,h,a)

    ROUND(48,a,b,c,d,e,f,g,h)
    ROUND(49,h,a,b,c,d,e,f,g)
    ROUND(50,g,h,a,b,c,d,e,f)
    ROUND(51,f,g,h,a,b,c,d,e)
    ROUND(52,e,f,g,h,a,b,c,d)
    ROUND(53,d,e,f,g,h,a,b,c)
    ROUND(54,c,d,e,f,g,h,a,b)
    ROUND(55,b,c,d,e,f,g,h,a)

    ROUND(56,a,b,c,d,e,f,g,h)
    ROUND(57,h,a,b,c,d,e,f,g)
    ROUND(58,g,h,a,b,c,d,e,f)
    ROUND(59,f,g,h,a,b,c,d,e)
    ROUND(60,e,f,g,h,a,b,c,d)
    ROUND(61,d,e,f,g,h,a,b,c)
    ROUND(62,c,d,e,f,g,h,a,b)
    ROUND(63,b,c,d,e,f,g,h,a)
#endif

    H[0] += a;
    H[1] += b;
    H[2] += c;
    H[3] += d;
    H[4] += e;
    H[5] += f;
    H[6] += g;
    H[7] += h;
  }
#undef ROUND
}

#undef s0
#undef s1
#undef S0
#undef S1

void 
SHA256_Update_nss(SHA256Context *ctx, const unsigned char *input,
		    unsigned int inputLen)
{
    unsigned int inBuf = ctx->sizeLo & 0x3f;
    if (!inputLen)
    	return;

    /* Add inputLen into the count of bytes processed, before processing */
    if ((ctx->sizeLo += inputLen) < inputLen)
    	ctx->sizeHi++;

    /* if data already in buffer, attemp to fill rest of buffer */
    if (inBuf) {
    	unsigned int todo = SHA256_BLOCK_LENGTH - inBuf;
	if (inputLen < todo)
	    todo = inputLen;
	memcpy(B + inBuf, input, todo);
	input    += todo;
	inputLen -= todo;
	if (inBuf + todo == SHA256_BLOCK_LENGTH)
	    SHA256_Compress(ctx);
    }

    /* if enough data to fill one or more whole buffers, process them. */
    while (inputLen >= SHA256_BLOCK_LENGTH) {
    	memcpy(B, input, SHA256_BLOCK_LENGTH);
	input    += SHA256_BLOCK_LENGTH;
	inputLen -= SHA256_BLOCK_LENGTH;
	SHA256_Compress(ctx);
    }
    /* if data left over, fill it into buffer */
    if (inputLen) 
    	memcpy(B, input, inputLen);
}

void 
SHA256_End(SHA256Context *ctx, unsigned char *digest,
           unsigned int *digestLen, unsigned int maxDigestLen)
{
    unsigned int inBuf = ctx->sizeLo & 0x3f;
    unsigned int padLen = (inBuf < 56) ? (56 - inBuf) : (56 + 64 - inBuf);
    PRUint32 hi, lo;
#ifdef SWAP4MASK
    PRUint32 t1;
#endif

    hi = (ctx->sizeHi << 3) | (ctx->sizeLo >> 29);
    lo = (ctx->sizeLo << 3);

    SHA256_Update_nss(ctx, pad, padLen);

#if defined(IS_LITTLE_ENDIAN)
#if !ASM_SWAP
    be32enc(&W[14], hi);
    be32enc(&W[15], lo);
#else
    W[14] = SHA_HTONL(hi);
    W[15] = SHA_HTONL(lo);
#endif
#else
    W[14] = hi;
    W[15] = lo;
#endif
    SHA256_Compress(ctx);

    /* now output the answer */
#if defined(IS_LITTLE_ENDIAN)
#if !ASM_SWAP
    be32enc_vect(digest, H, 32);
#else 
    BYTESWAP4(H[0]);
    BYTESWAP4(H[1]);
    BYTESWAP4(H[2]);
    BYTESWAP4(H[3]);
    BYTESWAP4(H[4]);
    BYTESWAP4(H[5]);
    BYTESWAP4(H[6]);
    BYTESWAP4(H[7]);
#endif
#endif
    padLen = PR_MIN(SHA256_LENGTH, maxDigestLen);
#if ASM_SWAP
    memcpy(digest, H, padLen);
#endif
    if (digestLen)
	*digestLen = padLen;
}

void
SHA256_EndRaw(SHA256Context *ctx, unsigned char *digest,
	      unsigned int *digestLen, unsigned int maxDigestLen)
{
    PRUint32 h[8];
    unsigned int len;
#ifdef SWAP4MASK
    PRUint32 t1;
#endif

    memcpy(h, ctx->h, sizeof(h));

#if defined(IS_LITTLE_ENDIAN)
    BYTESWAP4(h[0]);
    BYTESWAP4(h[1]);
    BYTESWAP4(h[2]);
    BYTESWAP4(h[3]);
    BYTESWAP4(h[4]);
    BYTESWAP4(h[5]);
    BYTESWAP4(h[6]);
    BYTESWAP4(h[7]);
#endif

    len = PR_MIN(SHA256_LENGTH, maxDigestLen);
    memcpy(digest, h, len);
    if (digestLen)
	*digestLen = len;
}

int 
SHA256_HashBuf(unsigned char *dest, const unsigned char *src, 
               unsigned short src_length)
{
    SHA256Context ctx;
    unsigned int outLen;

    SHA256_Begin(&ctx);
    SHA256_Update_nss(&ctx, src, src_length);
    SHA256_End(&ctx, dest, &outLen, SHA256_LENGTH);
    memset(&ctx, 0, sizeof ctx);

    return 1;
}


int 
SHA256_Hash(unsigned char *dest, const char *src)
{
    return SHA256_HashBuf(dest, (const unsigned char *)src, PORT_Strlen(src));
}


void SHA256_TraceState(SHA256Context *ctx) { }

unsigned int 
SHA256_FlattenSize(SHA256Context *ctx)
{
    return sizeof *ctx;
}

int 
SHA256_Flatten(SHA256Context *ctx,unsigned char *space)
{
    PORT_Memcpy(space, ctx, sizeof *ctx);
    return 1;
}

SHA256Context * 
SHA256_Resurrect(unsigned char *space, void *arg)
{
    SHA256Context *ctx = SHA256_NewContext();
    if (ctx) 
	PORT_Memcpy(ctx, space, sizeof *ctx);
    return ctx;
}

void SHA256_Clone(SHA256Context *dest, SHA256Context *src) 
{
    memcpy(dest, src, sizeof *dest);
}
