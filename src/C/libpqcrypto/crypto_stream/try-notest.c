/*
 * crypto_stream/try-notest.c version 20180223
 * D. J. Bernstein
 * Public domain.
 * Auto-generated by trygen-notest.py; do not edit.
 */

#include "crypto_stream.h"
#include "try.h"

const char *primitiveimplementation = crypto_stream_implementation;

#define TUNE_BYTES 1536
#ifdef SMALL
#define MAXTEST_BYTES 128
#else
#define MAXTEST_BYTES 4096
#endif
#ifdef SMALL
#define LOOPS 512
#else
#define LOOPS 4096
#endif

static unsigned char *k;
static unsigned char *n;
static unsigned char *m;
static unsigned char *c;
static unsigned char *s;
static unsigned char *k2;
static unsigned char *n2;
static unsigned char *m2;
static unsigned char *c2;
static unsigned char *s2;
#define klen crypto_stream_KEYBYTES
#define nlen crypto_stream_NONCEBYTES
unsigned long long mlen;
unsigned long long clen;
unsigned long long slen;

void preallocate(void)
{
}

void allocate(void)
{
  unsigned long long alloclen = 0;
  if (alloclen < TUNE_BYTES) alloclen = TUNE_BYTES;
  if (alloclen < MAXTEST_BYTES) alloclen = MAXTEST_BYTES;
  if (alloclen < crypto_stream_KEYBYTES) alloclen = crypto_stream_KEYBYTES;
  if (alloclen < crypto_stream_NONCEBYTES) alloclen = crypto_stream_NONCEBYTES;
  k = alignedcalloc(alloclen);
  n = alignedcalloc(alloclen);
  m = alignedcalloc(alloclen);
  c = alignedcalloc(alloclen);
  s = alignedcalloc(alloclen);
  k2 = alignedcalloc(alloclen);
  n2 = alignedcalloc(alloclen);
  m2 = alignedcalloc(alloclen);
  c2 = alignedcalloc(alloclen);
  s2 = alignedcalloc(alloclen);
}

void predoit(void)
{
}

void doit(void)
{
  crypto_stream_xor(c,m,TUNE_BYTES,n,k);
}
