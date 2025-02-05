/* fips_shatest.c */
/* Written by Dr Stephen N Henson (steve@openssl.org) for the OpenSSL
 * project 2005.
 */
/* ====================================================================
 * Copyright (c) 2005 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.OpenSSL.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    licensing@OpenSSL.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.OpenSSL.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 * This product includes cryptographic software written by Eric Young
 * (eay@cryptsoft.com).  This product includes software written by Tim
 * Hudson (tjh@cryptsoft.com).
 *
 */

#define OPENSSL_FIPSAPI

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/bn.h>

#include "utl/fips_cryptodev.h"

#ifndef OPENSSL_FIPS

int main(int argc, char *argv[])
{
    printf("No FIPS SHAXXX support\n");
    return(0);
}

#else

#include <openssl/fips.h>

#include "fips_utl.h"

static int dgst_test(FILE *out, FILE *in);
static int print_dgst(enum cryptodev_crypto_op_t md, FILE *out,
		unsigned char *Msg, int Msglen);
static int print_monte(enum cryptodev_crypto_op_t md, FILE *out,
		unsigned char *Seed, int SeedLen);

#ifdef FIPS_ALGVS
int fips_shatest_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
	{
	FILE *in = NULL, *out = NULL;

	int ret = 1;
	fips_algtest_init();

	if (argc == 1)
		in = stdin;
	else
		in = fopen(argv[1], "r");

	if (argc < 2)
		out = stdout;
	else
		out = fopen(argv[2], "w");

	if (!in)
		{
		fprintf(stderr, "FATAL input initialization error\n");
		goto end;
		}

	if (!out)
		{
		fprintf(stderr, "FATAL output initialization error\n");
		goto end;
		}

	if (!dgst_test(out, in))
		{
		fprintf(stderr, "FATAL digest file processing error\n");
		goto end;
		}
	else
		ret = 0;

	end:

	if (in && (in != stdin))
		fclose(in);
	if (out && (out != stdout))
		fclose(out);

	return ret;

	}

#define SHA_TEST_MAX_BITS	102400
#define SHA_TEST_MAXLINELEN	(((SHA_TEST_MAX_BITS >> 3) * 2) + 100)

int dgst_test(FILE *out, FILE *in)
	{
	enum cryptodev_crypto_op_t md = CRYPTO_ALGORITHM_ALL;
	char *linebuf, *olinebuf, *p, *q;
	char *keyword, *value;
	unsigned char *Msg = NULL, *Seed = NULL;
	long MsgLen = -1, Len = -1, SeedLen = -1;
	int ret = 0;
	int lnum = 0;

	olinebuf = OPENSSL_malloc(SHA_TEST_MAXLINELEN);
	linebuf = OPENSSL_malloc(SHA_TEST_MAXLINELEN);

	if (!linebuf || !olinebuf)
		goto error;


	while (fgets(olinebuf, SHA_TEST_MAXLINELEN, in))
		{
		lnum++;
		strcpy(linebuf, olinebuf);
		keyword = linebuf;
		/* Skip leading space */
		while (isspace((unsigned char)*keyword))
			keyword++;

		/* Look for = sign */
		p = strchr(linebuf, '=');

		/* If no = or starts with [ (for [L=20] line) just copy */
		if (!p)
			{
			fputs(olinebuf, out);
			continue;
			}

		q = p - 1;

		/* Remove trailing space */
		while (isspace((unsigned char)*q))
			*q-- = 0;

		*p = 0;
		value = p + 1;

		/* Remove leading space from value */
		while (isspace((unsigned char)*value))
			value++;

		/* Remove trailing space from value */
		p = value + strlen(value) - 1;
		while (*p == '\n' || isspace((unsigned char)*p))
			*p-- = 0;

		if (!strcmp(keyword,"[L") && *p==']')
			{
			switch (atoi(value))
				{
				case 20: md=CRYPTO_SHA1;     break;
				case 28: md=CRYPTO_SHA2_224; break;
				case 32: md=CRYPTO_SHA2_256; break;
				case 48: md=CRYPTO_SHA2_384; break;
				case 64: md=CRYPTO_SHA2_512; break;
				default: goto parse_error;
				}
			}
		else if (!strcmp(keyword, "Len"))
			{
			if (Len != -1)
				goto parse_error;
			Len = atoi(value);
			if (Len < 0)
				goto parse_error;
			/* Only handle multiples of 8 bits */
			if (Len & 0x7)
				goto parse_error;
			if (Len > SHA_TEST_MAX_BITS)
				goto parse_error;
			MsgLen = Len >> 3;
			}

		else if (!strcmp(keyword, "Msg"))
			{
			long tmplen;
			if (strlen(value) & 1)
				*(--value) = '0';
			if (Msg)
				goto parse_error;
			Msg = hex2bin_m(value, &tmplen);
			if (!Msg)
				goto parse_error;
			}
		else if (!strcmp(keyword, "Seed"))
			{
			if (strlen(value) & 1)
				*(--value) = '0';
			if (Seed)
				goto parse_error;
			Seed = hex2bin_m(value, &SeedLen);
			if (!Seed)
				goto parse_error;
			}
		else if (!strcmp(keyword, "MD"))
			continue;
		else
			goto parse_error;

		fputs(olinebuf, out);

		if (CRYPTO_ALGORITHM_ALL != md && Msg && (MsgLen >= 0))
			{
			if (!print_dgst(md, out, Msg, MsgLen))
				goto error;
			OPENSSL_free(Msg);
			Msg = NULL;
			MsgLen = -1;
			Len = -1;
			}
		else if (CRYPTO_ALGORITHM_ALL != md && Seed && (SeedLen > 0))
			{
			if (!print_monte(md, out, Seed, SeedLen))
				goto error;
			OPENSSL_free(Seed);
			Seed = NULL;
			SeedLen = -1;
			}
	

		}


	ret = 1;


	error:

	if (olinebuf)
		OPENSSL_free(olinebuf);
	if (linebuf)
		OPENSSL_free(linebuf);
	if (Msg)
		OPENSSL_free(Msg);
	if (Seed)
		OPENSSL_free(Seed);

	return ret;

	parse_error:

	fprintf(stderr, "FATAL parse error processing line %d\n", lnum);

	goto error;

	}

static int print_dgst(enum cryptodev_crypto_op_t emd, FILE *out,
		unsigned char *Msg, int Msglen)
	{
	int i, mdlen;
	unsigned char md[EVP_MAX_MD_SIZE];
	struct session_op session = { .mac = emd };
	struct crypt_op op = {
		.len = Msglen,
		.src = Msg,
		.mac = md
	};

	switch(emd) {
		case CRYPTO_SHA1:     mdlen = 20; break;
		case CRYPTO_SHA2_224: mdlen = 28; break;
		case CRYPTO_SHA2_256: mdlen = 32; break;
		case CRYPTO_SHA2_384: mdlen = 48; break;
		case CRYPTO_SHA2_512: mdlen = 64; break;
		default: return 0;
	}

	if(!cryptodev_op(session, op))
		{
		fputs("Error calculating HASH\n", stderr);
		return 0;
		}
	fputs("MD = ", out);
	for (i = 0; i < mdlen; i++)
		fprintf(out, "%02x", md[i]);
	fputs(RESP_EOL, out);
	return 1;
	}

static int print_monte(enum cryptodev_crypto_op_t md, FILE *out,
		unsigned char *Seed, int SeedLen)
	{
	unsigned int i, j, k;
	int ret = 0;
	unsigned int mlen;
	int cryptofd = -1;

	struct session_op session = { .mac = md };
	struct crypt_op ops[] = {
		{ .flags = COP_FLAG_UPDATE },
		{ .flags = COP_FLAG_UPDATE },
		{ .flags = COP_FLAG_UPDATE },
		{ .flags = COP_FLAG_FINAL  }
	};
	struct crypt_op *tmp,
		*m1       = ops + 0,
		*m2       = ops + 1,
		*m3       = ops + 2,
		*finalize = ops + 3;

	if(!cryptodev_start_session(&session, &cryptofd)) return 0;

	if (SeedLen > EVP_MAX_MD_SIZE)
		mlen = SeedLen;
	else
		mlen = EVP_MAX_MD_SIZE;

	m1->src = OPENSSL_malloc(mlen);
	m2->src = OPENSSL_malloc(mlen);
	m3->src = OPENSSL_malloc(mlen);

	if (!m1->src || !m2->src || !m3->src)
		goto mc_error;

	m1->len = m2->len = m3->len = SeedLen;
	memcpy(m1->src, Seed, SeedLen);
	memcpy(m2->src, Seed, SeedLen);
	memcpy(m3->src, Seed, SeedLen);

	fputs(RESP_EOL, out);

	for (j = 0; j < 100; j++)
		{
		for (i = 0; i < 1000; i++)
			{
			int success = 1;
			m1->flags |= COP_FLAG_RESET;
			success = success && cryptodev_session_op(session, cryptofd, *m1);
			m1->flags &= ~COP_FLAG_RESET;
			success = success && cryptodev_session_op(session, cryptofd, *m2);
			success = success && cryptodev_session_op(session, cryptofd, *m3);
			tmp = m1;
			m1 = m2;
			m2 = m3;
			m3 = tmp;
			finalize->mac = m3->src;
			success = success && cryptodev_session_op(session, cryptofd, *finalize);
			if(!success) goto mc_error;
			}
		fprintf(out, "COUNT = %d" RESP_EOL, j);
		fputs("MD = ", out);
		for (k = 0; k < m3->len; k++)
			fprintf(out, "%02x", m3->src[k]);
		fputs(RESP_EOL RESP_EOL, out);
		memcpy(m1->src, m3->src, m3->len);
		memcpy(m2->src, m3->src, m3->len);
		m1->len = m2->len = m3->len;
		}

	ret = 1;

	mc_error:
	if (m1->src)
		OPENSSL_free(m1->src);
	if (m2->src)
		OPENSSL_free(m2->src);
	if (m3->src)
		OPENSSL_free(m3->src);

	cryptodev_end_session(session, cryptofd);

	return ret;
	}

#endif
