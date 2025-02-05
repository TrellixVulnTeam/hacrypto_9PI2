/*-
 * Copyright (c) 2010 Konstantin Belousov <kib@FreeBSD.org>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHORS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHORS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: stable/10/sys/crypto/aesni/aesni.h 258212 2013-11-16 09:01:24Z jmg $
 */

#ifndef _AESNI_H_
#define _AESNI_H_

#include <sys/types.h>
#include <sys/malloc.h>
#include <sys/queue.h>

#include <opencrypto/cryptodev.h>

#if defined(__amd64__) || (defined(__i386__) && !defined(PC98))
#include <machine/cpufunc.h>
#include <machine/cputypes.h>
#include <machine/md_var.h>
#include <machine/specialreg.h>
#endif
#if defined(__i386__)
#include <machine/npx.h>
#elif defined(__amd64__)
#include <machine/fpu.h>
#endif

#define	AES128_ROUNDS	10
#define	AES192_ROUNDS	12
#define	AES256_ROUNDS	14
#define	AES_SCHED_LEN	((AES256_ROUNDS + 1) * AES_BLOCK_LEN)

struct aesni_session {
	uint8_t enc_schedule[AES_SCHED_LEN] __aligned(16);
	uint8_t dec_schedule[AES_SCHED_LEN] __aligned(16);
	uint8_t xts_schedule[AES_SCHED_LEN] __aligned(16);
	uint8_t iv[AES_BLOCK_LEN];
	int algo;
	int rounds;
	/* uint8_t *ses_ictx; */
	/* uint8_t *ses_octx; */
	/* int ses_mlen; */
	int used;
	uint32_t id;
	TAILQ_ENTRY(aesni_session) next;
	struct fpu_kern_ctx *fpu_ctx;
};

/*
 * Internal functions, implemented in assembler.
 */
void aesni_set_enckey(const uint8_t *userkey,
    uint8_t *encrypt_schedule /*__aligned(16)*/, int number_of_rounds);
void aesni_set_deckey(const uint8_t *encrypt_schedule /*__aligned(16)*/,
    uint8_t *decrypt_schedule /*__aligned(16)*/, int number_of_rounds);

/*
 * Slightly more public interfaces.
 */
void aesni_encrypt_cbc(int rounds, const void *key_schedule /*__aligned(16)*/,
    size_t len, const uint8_t *from, uint8_t *to,
    const uint8_t iv[AES_BLOCK_LEN]);
void aesni_decrypt_cbc(int rounds, const void *key_schedule /*__aligned(16)*/,
    size_t len, uint8_t *buf, const uint8_t iv[AES_BLOCK_LEN]);
void aesni_encrypt_ecb(int rounds, const void *key_schedule /*__aligned(16)*/,
    size_t len, const uint8_t *from, uint8_t *to);
void aesni_decrypt_ecb(int rounds, const void *key_schedule /*__aligned(16)*/,
    size_t len, const uint8_t *from, uint8_t *to);

void aesni_encrypt_xts(int rounds, const void *data_schedule /*__aligned(16)*/,
    const void *tweak_schedule /*__aligned(16)*/, size_t len,
    const uint8_t *from, uint8_t *to, const uint8_t iv[AES_BLOCK_LEN]);
void aesni_decrypt_xts(int rounds, const void *data_schedule /*__aligned(16)*/,
    const void *tweak_schedule /*__aligned(16)*/, size_t len,
    const uint8_t *from, uint8_t *to, const uint8_t iv[AES_BLOCK_LEN]);

int aesni_cipher_setup(struct aesni_session *ses,
    struct cryptoini *encini);
int aesni_cipher_process(struct aesni_session *ses,
    struct cryptodesc *enccrd, struct cryptop *crp);

uint8_t *aesni_cipher_alloc(struct cryptodesc *enccrd, struct cryptop *crp,
    int *allocated);

#endif /* _AESNI_H_ */
