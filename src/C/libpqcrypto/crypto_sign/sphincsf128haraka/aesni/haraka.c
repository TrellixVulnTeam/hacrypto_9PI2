/*
Plain C implementation of the Haraka256 and Haraka512 permutations.
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "haraka.h"
#include "immintrin.h"

#define HARAKAS_RATE 32

#define u64 unsigned long
#define u128 __m128i

#define LOAD(src) _mm_load_si128((u128 *)(src))
#define STORE(dest,src) _mm_storeu_si128((u128 *)(dest),src)

#define XOR128(a, b) _mm_xor_si128(a, b)

#define AES2(s0, s1, rci) \
  s0 = _mm_aesenc_si128(s0, *(rci)); \
  s1 = _mm_aesenc_si128(s1, *(rci + 1)); \
  s0 = _mm_aesenc_si128(s0, *(rci + 2)); \
  s1 = _mm_aesenc_si128(s1, *(rci + 3));

#define AES4(s0, s1, s2, s3, rci) \
  s0 = _mm_aesenc_si128(s0, *(rci)); \
  s1 = _mm_aesenc_si128(s1, *(rci + 1)); \
  s2 = _mm_aesenc_si128(s2, *(rci + 2)); \
  s3 = _mm_aesenc_si128(s3, *(rci + 3)); \
  s0 = _mm_aesenc_si128(s0, *(rci + 4)); \
  s1 = _mm_aesenc_si128(s1, *(rci + 5)); \
  s2 = _mm_aesenc_si128(s2, *(rci + 6)); \
  s3 = _mm_aesenc_si128(s3, *(rci + 7));

#define MIX2(s0, s1) \
  tmp = _mm_unpacklo_epi32(s0, s1); \
  s1 = _mm_unpackhi_epi32(s0, s1); \
  s0 = tmp;

#define MIX4(s0, s1, s2, s3) \
  tmp  = _mm_unpacklo_epi32(s0, s1); \
  s0 = _mm_unpackhi_epi32(s0, s1); \
  s1 = _mm_unpacklo_epi32(s2, s3); \
  s2 = _mm_unpackhi_epi32(s2, s3); \
  s3 = _mm_unpacklo_epi32(s0, s2); \
  s0 = _mm_unpackhi_epi32(s0, s2); \
  s2 = _mm_unpackhi_epi32(s1, tmp); \
  s1 = _mm_unpacklo_epi32(s1, tmp);

#define TRUNCSTORE(out, s0, s1, s2, s3) \
  _mm_storeu_si128((u128 *)out, \
                   (__m128i)_mm_shuffle_pd((__m128d)s0, (__m128d)s1, 3)); \
  _mm_storeu_si128((u128 *)(out + 16), \
                   (__m128i)_mm_shuffle_pd((__m128d)s2, (__m128d)s3, 0));

u128 rc[40];
u128 rc_sseed[40];

void load_haraka_constants()
{
    rc[0] = _mm_set_epi32(0x0684704c,0xe620c00a,0xb2c5fef0,0x75817b9d);
    rc[1] = _mm_set_epi32(0x8b66b4e1,0x88f3a06b,0x640f6ba4,0x2f08f717);
    rc[2] = _mm_set_epi32(0x3402de2d,0x53f28498,0xcf029d60,0x9f029114);
    rc[3] = _mm_set_epi32(0x0ed6eae6,0x2e7b4f08,0xbbf3bcaf,0xfd5b4f79);
    rc[4] = _mm_set_epi32(0xcbcfb0cb,0x4872448b,0x79eecd1c,0xbe397044);
    rc[5] = _mm_set_epi32(0x7eeacdee,0x6e9032b7,0x8d5335ed,0x2b8a057b);
    rc[6] = _mm_set_epi32(0x67c28f43,0x5e2e7cd0,0xe2412761,0xda4fef1b);
    rc[7] = _mm_set_epi32(0x2924d9b0,0xafcacc07,0x675ffde2,0x1fc70b3b);
    rc[8] = _mm_set_epi32(0xab4d63f1,0xe6867fe9,0xecdb8fca,0xb9d465ee);
    rc[9] = _mm_set_epi32(0x1c30bf84,0xd4b7cd64,0x5b2a404f,0xad037e33);
    rc[10] = _mm_set_epi32(0xb2cc0bb9,0x941723bf,0x69028b2e,0x8df69800);
    rc[11] = _mm_set_epi32(0xfa0478a6,0xde6f5572,0x4aaa9ec8,0x5c9d2d8a);
    rc[12] = _mm_set_epi32(0xdfb49f2b,0x6b772a12,0x0efa4f2e,0x29129fd4);
    rc[13] = _mm_set_epi32(0x1ea10344,0xf449a236,0x32d611ae,0xbb6a12ee);
    rc[14] = _mm_set_epi32(0xaf044988,0x4b050084,0x5f9600c9,0x9ca8eca6);
    rc[15] = _mm_set_epi32(0x21025ed8,0x9d199c4f,0x78a2c7e3,0x27e593ec);
    rc[16] = _mm_set_epi32(0xbf3aaaf8,0xa759c9b7,0xb9282ecd,0x82d40173);
    rc[17] = _mm_set_epi32(0x6260700d,0x6186b017,0x37f2efd9,0x10307d6b);
    rc[18] = _mm_set_epi32(0x5aca45c2,0x21300443,0x81c29153,0xf6fc9ac6);
    rc[19] = _mm_set_epi32(0x9223973c,0x226b68bb,0x2caf92e8,0x36d1943a);
    rc[20] = _mm_set_epi32(0xd3bf9238,0x225886eb,0x6cbab958,0xe51071b4);
    rc[21] = _mm_set_epi32(0xdb863ce5,0xaef0c677,0x933dfddd,0x24e1128d);
    rc[22] = _mm_set_epi32(0xbb606268,0xffeba09c,0x83e48de3,0xcb2212b1);
    rc[23] = _mm_set_epi32(0x734bd3dc,0xe2e4d19c,0x2db91a4e,0xc72bf77d);
    rc[24] = _mm_set_epi32(0x43bb47c3,0x61301b43,0x4b1415c4,0x2cb3924e);
    rc[25] = _mm_set_epi32(0xdba775a8,0xe707eff6,0x03b231dd,0x16eb6899);
    rc[26] = _mm_set_epi32(0x6df3614b,0x3c755977,0x8e5e2302,0x7eca472c);
    rc[27] = _mm_set_epi32(0xcda75a17,0xd6de7d77,0x6d1be5b9,0xb88617f9);
    rc[28] = _mm_set_epi32(0xec6b43f0,0x6ba8e9aa,0x9d6c069d,0xa946ee5d);
    rc[29] = _mm_set_epi32(0xcb1e6950,0xf957332b,0xa2531159,0x3bf327c1);
    rc[30] = _mm_set_epi32(0x2cee0c75,0x00da619c,0xe4ed0353,0x600ed0d9);
    rc[31] = _mm_set_epi32(0xf0b1a5a1,0x96e90cab,0x80bbbabc,0x63a4a350);
    rc[32] = _mm_set_epi32(0xae3db102,0x5e962988,0xab0dde30,0x938dca39);
    rc[33] = _mm_set_epi32(0x17bb8f38,0xd554a40b,0x8814f3a8,0x2e75b442);
    rc[34] = _mm_set_epi32(0x34bb8a5b,0x5f427fd7,0xaeb6b779,0x360a16f6);
    rc[35] = _mm_set_epi32(0x26f65241,0xcbe55438,0x43ce5918,0xffbaafde);
    rc[36] = _mm_set_epi32(0x4ce99a54,0xb9f3026a,0xa2ca9cf7,0x839ec978);
    rc[37] = _mm_set_epi32(0xae51a51a,0x1bdff7be,0x40c06e28,0x22901235);
    rc[38] = _mm_set_epi32(0xa0c1613c,0xba7ed22b,0xc173bc0f,0x48a659cf);
    rc[39] = _mm_set_epi32(0x756acc03,0x02288288,0x4ad6bdfd,0xe9c59da1);
}

void tweak_constants(const unsigned char *pk_seed, const unsigned char *sk_seed,
                     unsigned long long seed_length)
{
    int i;
    unsigned char buf[40*16];

    /* Use the standard constants to generate tweaked ones. */
    load_haraka_constants();

    /* Constants for sk.seed */
    if (sk_seed != NULL) {
        haraka_S(buf, 40*16, sk_seed, seed_length);
        /* Tweak constants with the pub_seed */
        for (i = 0; i < 40; i++) {
            rc_sseed[i] = LOAD(buf + i*16);
        }
    }

    /* Constants for pk.seed */
    haraka_S(buf, 40*16, pk_seed, seed_length);

    /* Tweak constants with the pub_seed */
    for (i = 0; i < 40; i++) {
        rc[i] = LOAD(buf + i*16);
    }
}

static void haraka_S_absorb(unsigned char *s, unsigned int r,
                            const unsigned char *m, unsigned long long mlen,
                            unsigned char p)
{
    unsigned long long i;
    unsigned char t[64];

    while (mlen >= r) {
        // XOR block to state
        STORE(s, XOR128(LOAD(s), LOAD(m)));
        STORE(s + 16, XOR128(LOAD(s + 16), LOAD(m + 16)));
        haraka512_perm(s, s);
        mlen -= r;
        m += r;
    }

    for (i = 0; i < r; ++i) {
        t[i] = 0;
    }
    for (i = 0; i < mlen; ++i) {
        t[i] = m[i];
    }
    t[i] = p;
    t[r - 1] |= 128;
    STORE(s, XOR128(LOAD(s), LOAD(t)));
    STORE(s + 16, XOR128(LOAD(s + 16), LOAD(t + 16)));
}

static void haraka_S_squeezeblocks(unsigned char *h, unsigned long long nblocks,
                                   unsigned char *s, unsigned int r)
{
    while (nblocks > 0) {
        haraka512_perm(s, s);
        STORE(h, LOAD(s));
        STORE(h + 16, LOAD(s + 16));
        h += r;
        nblocks--;
    }
}


void haraka_S(unsigned char *out, unsigned long long outlen,
              const unsigned char *in, unsigned long long inlen)
{
    unsigned long long i;
    unsigned char s[64];
    unsigned char d[32];

    for (i = 0; i < 64; i++) {
        s[i] = 0;
    }
    haraka_S_absorb(s, HARAKAS_RATE, in, inlen, 0x1F);

    haraka_S_squeezeblocks(out, outlen / HARAKAS_RATE, s, HARAKAS_RATE);
    out += (outlen / HARAKAS_RATE) * HARAKAS_RATE;

    if (outlen % HARAKAS_RATE) {
        haraka_S_squeezeblocks(d, 1, s, HARAKAS_RATE);
        for (i = 0; i < outlen % HARAKAS_RATE; i++) {
            out[i] = d[i];
        }
    }
}

void haraka512_perm(unsigned char *out, const unsigned char *in) 
{
    u128 s[4], tmp;
  
    s[0] = LOAD(in);
    s[1] = LOAD(in + 16);
    s[2] = LOAD(in + 32);
    s[3] = LOAD(in + 48);
  
    AES4(s[0], s[1], s[2], s[3], rc);
    MIX4(s[0], s[1], s[2], s[3]);
  
    AES4(s[0], s[1], s[2], s[3], rc + 8);
    MIX4(s[0], s[1], s[2], s[3]);
  
    AES4(s[0], s[1], s[2], s[3], rc + 16);
    MIX4(s[0], s[1], s[2], s[3]);
  
    AES4(s[0], s[1], s[2], s[3], rc + 24);
    MIX4(s[0], s[1], s[2], s[3]);
  
    AES4(s[0], s[1], s[2], s[3], rc + 32);
    MIX4(s[0], s[1], s[2], s[3]);
  
    STORE(out, s[0]);
    STORE(out + 16, s[1]);
    STORE(out + 32, s[2]);
    STORE(out + 48, s[3]);
}

void haraka512(unsigned char *out, const unsigned char *in)
{
    u128 s[4], tmp;

    s[0] = LOAD(in);
    s[1] = LOAD(in + 16);
    s[2] = LOAD(in + 32);
    s[3] = LOAD(in + 48);

    AES4(s[0], s[1], s[2], s[3], rc);
    MIX4(s[0], s[1], s[2], s[3]);

    AES4(s[0], s[1], s[2], s[3], rc + 8);
    MIX4(s[0], s[1], s[2], s[3]);

    AES4(s[0], s[1], s[2], s[3], rc + 16);
    MIX4(s[0], s[1], s[2], s[3]);

    AES4(s[0], s[1], s[2], s[3], rc + 24);
    MIX4(s[0], s[1], s[2], s[3]);

    AES4(s[0], s[1], s[2], s[3], rc + 32);
    MIX4(s[0], s[1], s[2], s[3]);

    s[0] = XOR128(s[0], LOAD(in));
    s[1] = XOR128(s[1], LOAD(in + 16));
    s[2] = XOR128(s[2], LOAD(in + 32));
    s[3] = XOR128(s[3], LOAD(in + 48));

    // truncate and store result
    TRUNCSTORE(out, s[0], s[1], s[2], s[3]);
}


void haraka256(unsigned char *out, const unsigned char *in) {
    u128 s[2], tmp;
  
    s[0] = LOAD(in);
    s[1] = LOAD(in + 16);
  
    AES2(s[0], s[1], rc);
    MIX2(s[0], s[1]);
  
    AES2(s[0], s[1], rc + 4);
    MIX2(s[0], s[1]);
  
    AES2(s[0], s[1], rc + 8);
    MIX2(s[0], s[1]);
  
    AES2(s[0], s[1], rc + 12);
    MIX2(s[0], s[1]);
  
    AES2(s[0], s[1], rc + 16);
    MIX2(s[0], s[1]);
  
    s[0] = XOR128(s[0], LOAD(in));
    s[1] = XOR128(s[1], LOAD(in + 16));
  
    STORE(out, s[0]);
    STORE(out + 16, s[1]);
}


void haraka256_sk(unsigned char *out, const unsigned char *in) {
    u128 s[2], tmp;
  
    s[0] = LOAD(in);
    s[1] = LOAD(in + 16);
  
    AES2(s[0], s[1], rc_sseed);
    MIX2(s[0], s[1]);
  
    AES2(s[0], s[1], rc_sseed + 4);
    MIX2(s[0], s[1]);
  
    AES2(s[0], s[1], rc_sseed + 8);
    MIX2(s[0], s[1]);
  
    AES2(s[0], s[1], rc_sseed + 12);
    MIX2(s[0], s[1]);
  
    AES2(s[0], s[1], rc_sseed + 16);
    MIX2(s[0], s[1]);
  
    s[0] = XOR128(s[0], LOAD(in));
    s[1] = XOR128(s[1], LOAD(in + 16));
  
    STORE(out, s[0]);
    STORE(out + 16, s[1]);
}
