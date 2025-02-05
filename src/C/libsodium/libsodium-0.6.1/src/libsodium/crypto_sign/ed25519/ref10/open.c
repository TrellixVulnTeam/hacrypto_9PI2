
#include <limits.h>
#include <string.h>

#include "api.h"
#include "crypto_hash_sha512.h"
#include "crypto_verify_32.h"
#include "ge.h"
#include "sc.h"
#include "utils.h"

int
crypto_sign_verify_detached(const unsigned char *sig, const unsigned char *m,
                            unsigned long long mlen, const unsigned char *pk)
{
    crypto_hash_sha512_state hs;
    unsigned char h[64];
    unsigned char rcheck[32];
    unsigned int  i;
    unsigned char d = 0;
    ge_p3 A;
    ge_p2 R;

    if (sig[63] & 224) {
        return -1;
    }
    if (ge_frombytes_negate_vartime(&A, pk) != 0) {
        return -1;
    }
    for (i = 0; i < 32; ++i) {
        d |= pk[i];
    }
    if (d == 0) {
        return -1;
    }
    crypto_hash_sha512_init(&hs);
    crypto_hash_sha512_update(&hs, sig, 32);
    crypto_hash_sha512_update(&hs, pk, 32);
    crypto_hash_sha512_update(&hs, m, mlen);
    crypto_hash_sha512_final(&hs, h);
    sc_reduce(h);

    ge_double_scalarmult_vartime(&R, h, &A, sig + 32);
    ge_tobytes(rcheck, &R);

    if (crypto_verify_32(rcheck, sig) != 0) {
        return -1;
    }
    if (sig == rcheck) {
        return -1;
    }
    if (sodium_memcmp(sig, rcheck, 32) != 0) {
        return -1;
    }
    return 0;
}

int
crypto_sign_open(unsigned char *m, unsigned long long *mlen,
                 const unsigned char *sm, unsigned long long smlen,
                 const unsigned char *pk)
{
    if (smlen < 64 || smlen > SIZE_MAX) {
        goto badsig;
    }
    if (crypto_sign_verify_detached(sm, sm + 64, smlen - 64, pk) != 0) {
        memset(m, 0, smlen - 64);
        goto badsig;
    }
    *mlen = smlen - 64;
    memmove(m, sm + 64, *mlen);

    return 0;

badsig:
    *mlen = 0;
    return -1;
}
