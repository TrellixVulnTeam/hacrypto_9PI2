/* Copyright (c) 2014, Google Inc.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE. */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <openssl/bio.h>
#include <openssl/bytestring.h>
#include <openssl/crypto.h>
#include <openssl/digest.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>


/* kExampleRSAKeyDER is an RSA private key in ASN.1, DER format. Of course, you
 * should never use this key anywhere but in an example. */
static const uint8_t kExampleRSAKeyDER[] = {
    0x30, 0x82, 0x02, 0x5c, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81, 0x00, 0xf8,
    0xb8, 0x6c, 0x83, 0xb4, 0xbc, 0xd9, 0xa8, 0x57, 0xc0, 0xa5, 0xb4, 0x59,
    0x76, 0x8c, 0x54, 0x1d, 0x79, 0xeb, 0x22, 0x52, 0x04, 0x7e, 0xd3, 0x37,
    0xeb, 0x41, 0xfd, 0x83, 0xf9, 0xf0, 0xa6, 0x85, 0x15, 0x34, 0x75, 0x71,
    0x5a, 0x84, 0xa8, 0x3c, 0xd2, 0xef, 0x5a, 0x4e, 0xd3, 0xde, 0x97, 0x8a,
    0xdd, 0xff, 0xbb, 0xcf, 0x0a, 0xaa, 0x86, 0x92, 0xbe, 0xb8, 0x50, 0xe4,
    0xcd, 0x6f, 0x80, 0x33, 0x30, 0x76, 0x13, 0x8f, 0xca, 0x7b, 0xdc, 0xec,
    0x5a, 0xca, 0x63, 0xc7, 0x03, 0x25, 0xef, 0xa8, 0x8a, 0x83, 0x58, 0x76,
    0x20, 0xfa, 0x16, 0x77, 0xd7, 0x79, 0x92, 0x63, 0x01, 0x48, 0x1a, 0xd8,
    0x7b, 0x67, 0xf1, 0x52, 0x55, 0x49, 0x4e, 0xd6, 0x6e, 0x4a, 0x5c, 0xd7,
    0x7a, 0x37, 0x36, 0x0c, 0xde, 0xdd, 0x8f, 0x44, 0xe8, 0xc2, 0xa7, 0x2c,
    0x2b, 0xb5, 0xaf, 0x64, 0x4b, 0x61, 0x07, 0x02, 0x03, 0x01, 0x00, 0x01,
    0x02, 0x81, 0x80, 0x74, 0x88, 0x64, 0x3f, 0x69, 0x45, 0x3a, 0x6d, 0xc7,
    0x7f, 0xb9, 0xa3, 0xc0, 0x6e, 0xec, 0xdc, 0xd4, 0x5a, 0xb5, 0x32, 0x85,
    0x5f, 0x19, 0xd4, 0xf8, 0xd4, 0x3f, 0x3c, 0xfa, 0xc2, 0xf6, 0x5f, 0xee,
    0xe6, 0xba, 0x87, 0x74, 0x2e, 0xc7, 0x0c, 0xd4, 0x42, 0xb8, 0x66, 0x85,
    0x9c, 0x7b, 0x24, 0x61, 0xaa, 0x16, 0x11, 0xf6, 0xb5, 0xb6, 0xa4, 0x0a,
    0xc9, 0x55, 0x2e, 0x81, 0xa5, 0x47, 0x61, 0xcb, 0x25, 0x8f, 0xc2, 0x15,
    0x7b, 0x0e, 0x7c, 0x36, 0x9f, 0x3a, 0xda, 0x58, 0x86, 0x1c, 0x5b, 0x83,
    0x79, 0xe6, 0x2b, 0xcc, 0xe6, 0xfa, 0x2c, 0x61, 0xf2, 0x78, 0x80, 0x1b,
    0xe2, 0xf3, 0x9d, 0x39, 0x2b, 0x65, 0x57, 0x91, 0x3d, 0x71, 0x99, 0x73,
    0xa5, 0xc2, 0x79, 0x20, 0x8c, 0x07, 0x4f, 0xe5, 0xb4, 0x60, 0x1f, 0x99,
    0xa2, 0xb1, 0x4f, 0x0c, 0xef, 0xbc, 0x59, 0x53, 0x00, 0x7d, 0xb1, 0x02,
    0x41, 0x00, 0xfc, 0x7e, 0x23, 0x65, 0x70, 0xf8, 0xce, 0xd3, 0x40, 0x41,
    0x80, 0x6a, 0x1d, 0x01, 0xd6, 0x01, 0xff, 0xb6, 0x1b, 0x3d, 0x3d, 0x59,
    0x09, 0x33, 0x79, 0xc0, 0x4f, 0xde, 0x96, 0x27, 0x4b, 0x18, 0xc6, 0xd9,
    0x78, 0xf1, 0xf4, 0x35, 0x46, 0xe9, 0x7c, 0x42, 0x7a, 0x5d, 0x9f, 0xef,
    0x54, 0xb8, 0xf7, 0x9f, 0xc4, 0x33, 0x6c, 0xf3, 0x8c, 0x32, 0x46, 0x87,
    0x67, 0x30, 0x7b, 0xa7, 0xac, 0xe3, 0x02, 0x41, 0x00, 0xfc, 0x2c, 0xdf,
    0x0c, 0x0d, 0x88, 0xf5, 0xb1, 0x92, 0xa8, 0x93, 0x47, 0x63, 0x55, 0xf5,
    0xca, 0x58, 0x43, 0xba, 0x1c, 0xe5, 0x9e, 0xb6, 0x95, 0x05, 0xcd, 0xb5,
    0x82, 0xdf, 0xeb, 0x04, 0x53, 0x9d, 0xbd, 0xc2, 0x38, 0x16, 0xb3, 0x62,
    0xdd, 0xa1, 0x46, 0xdb, 0x6d, 0x97, 0x93, 0x9f, 0x8a, 0xc3, 0x9b, 0x64,
    0x7e, 0x42, 0xe3, 0x32, 0x57, 0x19, 0x1b, 0xd5, 0x6e, 0x85, 0xfa, 0xb8,
    0x8d, 0x02, 0x41, 0x00, 0xbc, 0x3d, 0xde, 0x6d, 0xd6, 0x97, 0xe8, 0xba,
    0x9e, 0x81, 0x37, 0x17, 0xe5, 0xa0, 0x64, 0xc9, 0x00, 0xb7, 0xe7, 0xfe,
    0xf4, 0x29, 0xd9, 0x2e, 0x43, 0x6b, 0x19, 0x20, 0xbd, 0x99, 0x75, 0xe7,
    0x76, 0xf8, 0xd3, 0xae, 0xaf, 0x7e, 0xb8, 0xeb, 0x81, 0xf4, 0x9d, 0xfe,
    0x07, 0x2b, 0x0b, 0x63, 0x0b, 0x5a, 0x55, 0x90, 0x71, 0x7d, 0xf1, 0xdb,
    0xd9, 0xb1, 0x41, 0x41, 0x68, 0x2f, 0x4e, 0x39, 0x02, 0x40, 0x5a, 0x34,
    0x66, 0xd8, 0xf5, 0xe2, 0x7f, 0x18, 0xb5, 0x00, 0x6e, 0x26, 0x84, 0x27,
    0x14, 0x93, 0xfb, 0xfc, 0xc6, 0x0f, 0x5e, 0x27, 0xe6, 0xe1, 0xe9, 0xc0,
    0x8a, 0xe4, 0x34, 0xda, 0xe9, 0xa2, 0x4b, 0x73, 0xbc, 0x8c, 0xb9, 0xba,
    0x13, 0x6c, 0x7a, 0x2b, 0x51, 0x84, 0xa3, 0x4a, 0xe0, 0x30, 0x10, 0x06,
    0x7e, 0xed, 0x17, 0x5a, 0x14, 0x00, 0xc9, 0xef, 0x85, 0xea, 0x52, 0x2c,
    0xbc, 0x65, 0x02, 0x40, 0x51, 0xe3, 0xf2, 0x83, 0x19, 0x9b, 0xc4, 0x1e,
    0x2f, 0x50, 0x3d, 0xdf, 0x5a, 0xa2, 0x18, 0xca, 0x5f, 0x2e, 0x49, 0xaf,
    0x6f, 0xcc, 0xfa, 0x65, 0x77, 0x94, 0xb5, 0xa1, 0x0a, 0xa9, 0xd1, 0x8a,
    0x39, 0x37, 0xf4, 0x0b, 0xa0, 0xd7, 0x82, 0x27, 0x5e, 0xae, 0x17, 0x17,
    0xa1, 0x1e, 0x54, 0x34, 0xbf, 0x6e, 0xc4, 0x8e, 0x99, 0x5d, 0x08, 0xf1,
    0x2d, 0x86, 0x9d, 0xa5, 0x20, 0x1b, 0xe5, 0xdf,
};

static const uint8_t kMsg[] = {1, 2, 3, 4};

static const uint8_t kSignature[] = {
    0xa5, 0xf0, 0x8a, 0x47, 0x5d, 0x3c, 0xb3, 0xcc, 0xa9, 0x79, 0xaf, 0x4d,
    0x8c, 0xae, 0x4c, 0x14, 0xef, 0xc2, 0x0b, 0x34, 0x36, 0xde, 0xf4, 0x3e,
    0x3d, 0xbb, 0x4a, 0x60, 0x5c, 0xc8, 0x91, 0x28, 0xda, 0xfb, 0x7e, 0x04,
    0x96, 0x7e, 0x63, 0x13, 0x90, 0xce, 0xb9, 0xb4, 0x62, 0x7a, 0xfd, 0x09,
    0x3d, 0xc7, 0x67, 0x78, 0x54, 0x04, 0xeb, 0x52, 0x62, 0x6e, 0x24, 0x67,
    0xb4, 0x40, 0xfc, 0x57, 0x62, 0xc6, 0xf1, 0x67, 0xc1, 0x97, 0x8f, 0x6a,
    0xa8, 0xae, 0x44, 0x46, 0x5e, 0xab, 0x67, 0x17, 0x53, 0x19, 0x3a, 0xda,
    0x5a, 0xc8, 0x16, 0x3e, 0x86, 0xd5, 0xc5, 0x71, 0x2f, 0xfc, 0x23, 0x48,
    0xd9, 0x0b, 0x13, 0xdd, 0x7b, 0x5a, 0x25, 0x79, 0xef, 0xa5, 0x7b, 0x04,
    0xed, 0x44, 0xf6, 0x18, 0x55, 0xe4, 0x0a, 0xe9, 0x57, 0x79, 0x5d, 0xd7,
    0x55, 0xa7, 0xab, 0x45, 0x02, 0x97, 0x60, 0x42,
};

/* kExamplePSSCert is an example self-signed certificate, signed with
 * kExampleRSAKeyDER using RSA-PSS with default hash functions. */
static const uint8_t kExamplePSSCert[] = {
    0x30, 0x82, 0x02, 0x62, 0x30, 0x82, 0x01, 0xc6, 0xa0, 0x03, 0x02, 0x01,
    0x02, 0x02, 0x09, 0x00, 0x8d, 0xea, 0x53, 0x24, 0xfa, 0x48, 0x87, 0xf3,
    0x30, 0x12, 0x06, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01,
    0x0a, 0x30, 0x05, 0xa2, 0x03, 0x02, 0x01, 0x6a, 0x30, 0x45, 0x31, 0x0b,
    0x30, 0x09, 0x06, 0x03, 0x55, 0x04, 0x06, 0x13, 0x02, 0x41, 0x55, 0x31,
    0x13, 0x30, 0x11, 0x06, 0x03, 0x55, 0x04, 0x08, 0x0c, 0x0a, 0x53, 0x6f,
    0x6d, 0x65, 0x2d, 0x53, 0x74, 0x61, 0x74, 0x65, 0x31, 0x21, 0x30, 0x1f,
    0x06, 0x03, 0x55, 0x04, 0x0a, 0x0c, 0x18, 0x49, 0x6e, 0x74, 0x65, 0x72,
    0x6e, 0x65, 0x74, 0x20, 0x57, 0x69, 0x64, 0x67, 0x69, 0x74, 0x73, 0x20,
    0x50, 0x74, 0x79, 0x20, 0x4c, 0x74, 0x64, 0x30, 0x1e, 0x17, 0x0d, 0x31,
    0x34, 0x31, 0x30, 0x30, 0x39, 0x31, 0x39, 0x30, 0x39, 0x35, 0x35, 0x5a,
    0x17, 0x0d, 0x31, 0x35, 0x31, 0x30, 0x30, 0x39, 0x31, 0x39, 0x30, 0x39,
    0x35, 0x35, 0x5a, 0x30, 0x45, 0x31, 0x0b, 0x30, 0x09, 0x06, 0x03, 0x55,
    0x04, 0x06, 0x13, 0x02, 0x41, 0x55, 0x31, 0x13, 0x30, 0x11, 0x06, 0x03,
    0x55, 0x04, 0x08, 0x0c, 0x0a, 0x53, 0x6f, 0x6d, 0x65, 0x2d, 0x53, 0x74,
    0x61, 0x74, 0x65, 0x31, 0x21, 0x30, 0x1f, 0x06, 0x03, 0x55, 0x04, 0x0a,
    0x0c, 0x18, 0x49, 0x6e, 0x74, 0x65, 0x72, 0x6e, 0x65, 0x74, 0x20, 0x57,
    0x69, 0x64, 0x67, 0x69, 0x74, 0x73, 0x20, 0x50, 0x74, 0x79, 0x20, 0x4c,
    0x74, 0x64, 0x30, 0x81, 0x9f, 0x30, 0x0d, 0x06, 0x09, 0x2a, 0x86, 0x48,
    0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x03, 0x81, 0x8d, 0x00,
    0x30, 0x81, 0x89, 0x02, 0x81, 0x81, 0x00, 0xf8, 0xb8, 0x6c, 0x83, 0xb4,
    0xbc, 0xd9, 0xa8, 0x57, 0xc0, 0xa5, 0xb4, 0x59, 0x76, 0x8c, 0x54, 0x1d,
    0x79, 0xeb, 0x22, 0x52, 0x04, 0x7e, 0xd3, 0x37, 0xeb, 0x41, 0xfd, 0x83,
    0xf9, 0xf0, 0xa6, 0x85, 0x15, 0x34, 0x75, 0x71, 0x5a, 0x84, 0xa8, 0x3c,
    0xd2, 0xef, 0x5a, 0x4e, 0xd3, 0xde, 0x97, 0x8a, 0xdd, 0xff, 0xbb, 0xcf,
    0x0a, 0xaa, 0x86, 0x92, 0xbe, 0xb8, 0x50, 0xe4, 0xcd, 0x6f, 0x80, 0x33,
    0x30, 0x76, 0x13, 0x8f, 0xca, 0x7b, 0xdc, 0xec, 0x5a, 0xca, 0x63, 0xc7,
    0x03, 0x25, 0xef, 0xa8, 0x8a, 0x83, 0x58, 0x76, 0x20, 0xfa, 0x16, 0x77,
    0xd7, 0x79, 0x92, 0x63, 0x01, 0x48, 0x1a, 0xd8, 0x7b, 0x67, 0xf1, 0x52,
    0x55, 0x49, 0x4e, 0xd6, 0x6e, 0x4a, 0x5c, 0xd7, 0x7a, 0x37, 0x36, 0x0c,
    0xde, 0xdd, 0x8f, 0x44, 0xe8, 0xc2, 0xa7, 0x2c, 0x2b, 0xb5, 0xaf, 0x64,
    0x4b, 0x61, 0x07, 0x02, 0x03, 0x01, 0x00, 0x01, 0xa3, 0x50, 0x30, 0x4e,
    0x30, 0x1d, 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04, 0x16, 0x04, 0x14, 0xd0,
    0x41, 0xfb, 0x89, 0x41, 0x1e, 0xa7, 0xad, 0x5a, 0xec, 0x34, 0x5d, 0x49,
    0x11, 0xf9, 0x55, 0x81, 0x78, 0x1f, 0x13, 0x30, 0x1f, 0x06, 0x03, 0x55,
    0x1d, 0x23, 0x04, 0x18, 0x30, 0x16, 0x80, 0x14, 0xd0, 0x41, 0xfb, 0x89,
    0x41, 0x1e, 0xa7, 0xad, 0x5a, 0xec, 0x34, 0x5d, 0x49, 0x11, 0xf9, 0x55,
    0x81, 0x78, 0x1f, 0x13, 0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x04,
    0x05, 0x30, 0x03, 0x01, 0x01, 0xff, 0x30, 0x12, 0x06, 0x09, 0x2a, 0x86,
    0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x0a, 0x30, 0x05, 0xa2, 0x03, 0x02,
    0x01, 0x6a, 0x03, 0x81, 0x81, 0x00, 0x49, 0x4c, 0xb6, 0x45, 0x97, 0x20,
    0x35, 0xb3, 0x50, 0x64, 0x0d, 0x3f, 0xec, 0x5f, 0x95, 0xd5, 0x84, 0xcb,
    0x11, 0x7c, 0x03, 0xd7, 0xa6, 0xe6, 0xfa, 0x24, 0x95, 0x9f, 0x31, 0xb0,
    0xb5, 0xec, 0x66, 0x41, 0x51, 0x18, 0x21, 0x91, 0xbb, 0xe0, 0xaf, 0xf0,
    0xc5, 0xb7, 0x59, 0x41, 0xd4, 0xdb, 0xa4, 0xd2, 0x64, 0xa7, 0x54, 0x0f,
    0x8c, 0xf7, 0xe1, 0xd3, 0x3b, 0x1a, 0xb7, 0x0e, 0x9d, 0x9a, 0xde, 0x50,
    0xa1, 0x9f, 0x0a, 0xf0, 0xda, 0x34, 0x0e, 0x34, 0x7d, 0x76, 0x07, 0xfe,
    0x5a, 0xfb, 0xf9, 0x58, 0x9b, 0xc9, 0x50, 0x84, 0x01, 0xa0, 0x05, 0x4d,
    0x67, 0x42, 0x0b, 0xf8, 0xe4, 0x05, 0xcf, 0xaf, 0x8b, 0x71, 0x31, 0xf1,
    0x0f, 0x6e, 0xc9, 0x24, 0x27, 0x9b, 0xac, 0x04, 0xd7, 0x64, 0x0d, 0x30,
    0x4e, 0x11, 0x93, 0x40, 0x39, 0xbb, 0x72, 0xb2, 0xfe, 0x6b, 0xe4, 0xae,
    0x8c, 0x16,
};

/* kExampleRSAKeyPKCS8 is kExampleRSAKeyDER encoded in a PKCS #8
 * PrivateKeyInfo. */
static const uint8_t kExampleRSAKeyPKCS8[] = {
    0x30, 0x82, 0x02, 0x76, 0x02, 0x01, 0x00, 0x30, 0x0d, 0x06, 0x09, 0x2a,
    0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01, 0x01, 0x01, 0x05, 0x00, 0x04, 0x82,
    0x02, 0x60, 0x30, 0x82, 0x02, 0x5c, 0x02, 0x01, 0x00, 0x02, 0x81, 0x81,
    0x00, 0xf8, 0xb8, 0x6c, 0x83, 0xb4, 0xbc, 0xd9, 0xa8, 0x57, 0xc0, 0xa5,
    0xb4, 0x59, 0x76, 0x8c, 0x54, 0x1d, 0x79, 0xeb, 0x22, 0x52, 0x04, 0x7e,
    0xd3, 0x37, 0xeb, 0x41, 0xfd, 0x83, 0xf9, 0xf0, 0xa6, 0x85, 0x15, 0x34,
    0x75, 0x71, 0x5a, 0x84, 0xa8, 0x3c, 0xd2, 0xef, 0x5a, 0x4e, 0xd3, 0xde,
    0x97, 0x8a, 0xdd, 0xff, 0xbb, 0xcf, 0x0a, 0xaa, 0x86, 0x92, 0xbe, 0xb8,
    0x50, 0xe4, 0xcd, 0x6f, 0x80, 0x33, 0x30, 0x76, 0x13, 0x8f, 0xca, 0x7b,
    0xdc, 0xec, 0x5a, 0xca, 0x63, 0xc7, 0x03, 0x25, 0xef, 0xa8, 0x8a, 0x83,
    0x58, 0x76, 0x20, 0xfa, 0x16, 0x77, 0xd7, 0x79, 0x92, 0x63, 0x01, 0x48,
    0x1a, 0xd8, 0x7b, 0x67, 0xf1, 0x52, 0x55, 0x49, 0x4e, 0xd6, 0x6e, 0x4a,
    0x5c, 0xd7, 0x7a, 0x37, 0x36, 0x0c, 0xde, 0xdd, 0x8f, 0x44, 0xe8, 0xc2,
    0xa7, 0x2c, 0x2b, 0xb5, 0xaf, 0x64, 0x4b, 0x61, 0x07, 0x02, 0x03, 0x01,
    0x00, 0x01, 0x02, 0x81, 0x80, 0x74, 0x88, 0x64, 0x3f, 0x69, 0x45, 0x3a,
    0x6d, 0xc7, 0x7f, 0xb9, 0xa3, 0xc0, 0x6e, 0xec, 0xdc, 0xd4, 0x5a, 0xb5,
    0x32, 0x85, 0x5f, 0x19, 0xd4, 0xf8, 0xd4, 0x3f, 0x3c, 0xfa, 0xc2, 0xf6,
    0x5f, 0xee, 0xe6, 0xba, 0x87, 0x74, 0x2e, 0xc7, 0x0c, 0xd4, 0x42, 0xb8,
    0x66, 0x85, 0x9c, 0x7b, 0x24, 0x61, 0xaa, 0x16, 0x11, 0xf6, 0xb5, 0xb6,
    0xa4, 0x0a, 0xc9, 0x55, 0x2e, 0x81, 0xa5, 0x47, 0x61, 0xcb, 0x25, 0x8f,
    0xc2, 0x15, 0x7b, 0x0e, 0x7c, 0x36, 0x9f, 0x3a, 0xda, 0x58, 0x86, 0x1c,
    0x5b, 0x83, 0x79, 0xe6, 0x2b, 0xcc, 0xe6, 0xfa, 0x2c, 0x61, 0xf2, 0x78,
    0x80, 0x1b, 0xe2, 0xf3, 0x9d, 0x39, 0x2b, 0x65, 0x57, 0x91, 0x3d, 0x71,
    0x99, 0x73, 0xa5, 0xc2, 0x79, 0x20, 0x8c, 0x07, 0x4f, 0xe5, 0xb4, 0x60,
    0x1f, 0x99, 0xa2, 0xb1, 0x4f, 0x0c, 0xef, 0xbc, 0x59, 0x53, 0x00, 0x7d,
    0xb1, 0x02, 0x41, 0x00, 0xfc, 0x7e, 0x23, 0x65, 0x70, 0xf8, 0xce, 0xd3,
    0x40, 0x41, 0x80, 0x6a, 0x1d, 0x01, 0xd6, 0x01, 0xff, 0xb6, 0x1b, 0x3d,
    0x3d, 0x59, 0x09, 0x33, 0x79, 0xc0, 0x4f, 0xde, 0x96, 0x27, 0x4b, 0x18,
    0xc6, 0xd9, 0x78, 0xf1, 0xf4, 0x35, 0x46, 0xe9, 0x7c, 0x42, 0x7a, 0x5d,
    0x9f, 0xef, 0x54, 0xb8, 0xf7, 0x9f, 0xc4, 0x33, 0x6c, 0xf3, 0x8c, 0x32,
    0x46, 0x87, 0x67, 0x30, 0x7b, 0xa7, 0xac, 0xe3, 0x02, 0x41, 0x00, 0xfc,
    0x2c, 0xdf, 0x0c, 0x0d, 0x88, 0xf5, 0xb1, 0x92, 0xa8, 0x93, 0x47, 0x63,
    0x55, 0xf5, 0xca, 0x58, 0x43, 0xba, 0x1c, 0xe5, 0x9e, 0xb6, 0x95, 0x05,
    0xcd, 0xb5, 0x82, 0xdf, 0xeb, 0x04, 0x53, 0x9d, 0xbd, 0xc2, 0x38, 0x16,
    0xb3, 0x62, 0xdd, 0xa1, 0x46, 0xdb, 0x6d, 0x97, 0x93, 0x9f, 0x8a, 0xc3,
    0x9b, 0x64, 0x7e, 0x42, 0xe3, 0x32, 0x57, 0x19, 0x1b, 0xd5, 0x6e, 0x85,
    0xfa, 0xb8, 0x8d, 0x02, 0x41, 0x00, 0xbc, 0x3d, 0xde, 0x6d, 0xd6, 0x97,
    0xe8, 0xba, 0x9e, 0x81, 0x37, 0x17, 0xe5, 0xa0, 0x64, 0xc9, 0x00, 0xb7,
    0xe7, 0xfe, 0xf4, 0x29, 0xd9, 0x2e, 0x43, 0x6b, 0x19, 0x20, 0xbd, 0x99,
    0x75, 0xe7, 0x76, 0xf8, 0xd3, 0xae, 0xaf, 0x7e, 0xb8, 0xeb, 0x81, 0xf4,
    0x9d, 0xfe, 0x07, 0x2b, 0x0b, 0x63, 0x0b, 0x5a, 0x55, 0x90, 0x71, 0x7d,
    0xf1, 0xdb, 0xd9, 0xb1, 0x41, 0x41, 0x68, 0x2f, 0x4e, 0x39, 0x02, 0x40,
    0x5a, 0x34, 0x66, 0xd8, 0xf5, 0xe2, 0x7f, 0x18, 0xb5, 0x00, 0x6e, 0x26,
    0x84, 0x27, 0x14, 0x93, 0xfb, 0xfc, 0xc6, 0x0f, 0x5e, 0x27, 0xe6, 0xe1,
    0xe9, 0xc0, 0x8a, 0xe4, 0x34, 0xda, 0xe9, 0xa2, 0x4b, 0x73, 0xbc, 0x8c,
    0xb9, 0xba, 0x13, 0x6c, 0x7a, 0x2b, 0x51, 0x84, 0xa3, 0x4a, 0xe0, 0x30,
    0x10, 0x06, 0x7e, 0xed, 0x17, 0x5a, 0x14, 0x00, 0xc9, 0xef, 0x85, 0xea,
    0x52, 0x2c, 0xbc, 0x65, 0x02, 0x40, 0x51, 0xe3, 0xf2, 0x83, 0x19, 0x9b,
    0xc4, 0x1e, 0x2f, 0x50, 0x3d, 0xdf, 0x5a, 0xa2, 0x18, 0xca, 0x5f, 0x2e,
    0x49, 0xaf, 0x6f, 0xcc, 0xfa, 0x65, 0x77, 0x94, 0xb5, 0xa1, 0x0a, 0xa9,
    0xd1, 0x8a, 0x39, 0x37, 0xf4, 0x0b, 0xa0, 0xd7, 0x82, 0x27, 0x5e, 0xae,
    0x17, 0x17, 0xa1, 0x1e, 0x54, 0x34, 0xbf, 0x6e, 0xc4, 0x8e, 0x99, 0x5d,
    0x08, 0xf1, 0x2d, 0x86, 0x9d, 0xa5, 0x20, 0x1b, 0xe5, 0xdf,
};

/* kExampleECKeyDER is a sample EC private key encoded as an ECPrivateKey
 * structure. */
static const uint8_t kExampleECKeyDER[] = {
    0x30, 0x77, 0x02, 0x01, 0x01, 0x04, 0x20, 0x07, 0x0f, 0x08, 0x72, 0x7a,
    0xd4, 0xa0, 0x4a, 0x9c, 0xdd, 0x59, 0xc9, 0x4d, 0x89, 0x68, 0x77, 0x08,
    0xb5, 0x6f, 0xc9, 0x5d, 0x30, 0x77, 0x0e, 0xe8, 0xd1, 0xc9, 0xce, 0x0a,
    0x8b, 0xb4, 0x6a, 0xa0, 0x0a, 0x06, 0x08, 0x2a, 0x86, 0x48, 0xce, 0x3d,
    0x03, 0x01, 0x07, 0xa1, 0x44, 0x03, 0x42, 0x00, 0x04, 0xe6, 0x2b, 0x69,
    0xe2, 0xbf, 0x65, 0x9f, 0x97, 0xbe, 0x2f, 0x1e, 0x0d, 0x94, 0x8a, 0x4c,
    0xd5, 0x97, 0x6b, 0xb7, 0xa9, 0x1e, 0x0d, 0x46, 0xfb, 0xdd, 0xa9, 0xa9,
    0x1e, 0x9d, 0xdc, 0xba, 0x5a, 0x01, 0xe7, 0xd6, 0x97, 0xa8, 0x0a, 0x18,
    0xf9, 0xc3, 0xc4, 0xa3, 0x1e, 0x56, 0xe2, 0x7c, 0x83, 0x48, 0xdb, 0x16,
    0x1a, 0x1c, 0xf5, 0x1d, 0x7e, 0xf1, 0x94, 0x2d, 0x4b, 0xcf, 0x72, 0x22,
    0xc1,
};

/* kExampleBadECKeyDER is a sample EC private key encoded as an ECPrivateKey
 * structure. The private key is equal to the order and will fail to import */
static const uint8_t kExampleBadECKeyDER[] = {
    0x30, 0x66, 0x02, 0x01, 0x00, 0x30, 0x13, 0x06, 0x07, 0x2A, 0x86, 0x48,
    0xCE, 0x3D, 0x02, 0x01, 0x06, 0x08, 0x2A, 0x86, 0x48, 0xCE, 0x3D, 0x03,
    0x01, 0x07, 0x04, 0x4C, 0x30, 0x4A, 0x02, 0x01, 0x01, 0x04, 0x20, 0xFF,
    0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xBC, 0xE6, 0xFA, 0xAD, 0xA7, 0x17, 0x9E, 0x84, 0xF3,
    0xB9, 0xCA, 0xC2, 0xFC, 0x63, 0x25, 0x51, 0xA1, 0x23, 0x03, 0x21, 0x00,
    0x00, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xBC, 0xE6, 0xFA, 0xAD, 0xA7, 0x17, 0x9E, 0x84,
    0xF3, 0xB9, 0xCA, 0xC2, 0xFC, 0x63, 0x25, 0x51
};

static EVP_PKEY *load_example_rsa_key(void) {
  EVP_PKEY *ret = NULL;
  const uint8_t *derp = kExampleRSAKeyDER;
  EVP_PKEY *pkey = NULL;
  RSA *rsa = NULL;

  if (!d2i_RSAPrivateKey(&rsa, &derp, sizeof(kExampleRSAKeyDER))) {
    return NULL;
  }

  pkey = EVP_PKEY_new();
  if (pkey == NULL || !EVP_PKEY_set1_RSA(pkey, rsa)) {
    goto out;
  }

  ret = pkey;
  pkey = NULL;

out:
  if (pkey) {
    EVP_PKEY_free(pkey);
  }
  if (rsa) {
    RSA_free(rsa);
  }

  return ret;
}

static int test_EVP_DigestSignInit(void) {
  int ret = 0;
  EVP_PKEY *pkey = NULL;
  uint8_t *sig = NULL;
  size_t sig_len = 0;
  EVP_MD_CTX md_ctx, md_ctx_verify;

  EVP_MD_CTX_init(&md_ctx);
  EVP_MD_CTX_init(&md_ctx_verify);

  pkey = load_example_rsa_key();
  if (pkey == NULL ||
      !EVP_DigestSignInit(&md_ctx, NULL, EVP_sha256(), NULL, pkey) ||
      !EVP_DigestSignUpdate(&md_ctx, kMsg, sizeof(kMsg))) {
    goto out;
  }
  /* Determine the size of the signature. */
  if (!EVP_DigestSignFinal(&md_ctx, NULL, &sig_len)) {
    goto out;
  }
  /* Sanity check for testing. */
  if (sig_len != EVP_PKEY_size(pkey)) {
    fprintf(stderr, "sig_len mismatch\n");
    goto out;
  }

  sig = malloc(sig_len);
  if (sig == NULL || !EVP_DigestSignFinal(&md_ctx, sig, &sig_len)) {
    goto out;
  }

  /* Ensure that the signature round-trips. */
  if (!EVP_DigestVerifyInit(&md_ctx_verify, NULL, EVP_sha256(), NULL, pkey) ||
      !EVP_DigestVerifyUpdate(&md_ctx_verify, kMsg, sizeof(kMsg)) ||
      !EVP_DigestVerifyFinal(&md_ctx_verify, sig, sig_len)) {
    goto out;
  }

  ret = 1;

out:
  if (!ret) {
    BIO_print_errors_fp(stderr);
  }

  EVP_MD_CTX_cleanup(&md_ctx);
  EVP_MD_CTX_cleanup(&md_ctx_verify);
  if (pkey) {
    EVP_PKEY_free(pkey);
  }
  if (sig) {
    free(sig);
  }

  return ret;
}

static int test_EVP_DigestVerifyInit(void) {
  int ret = 0;
  EVP_PKEY *pkey = NULL;
  EVP_MD_CTX md_ctx;

  EVP_MD_CTX_init(&md_ctx);

  pkey = load_example_rsa_key();
  if (pkey == NULL ||
      !EVP_DigestVerifyInit(&md_ctx, NULL, EVP_sha256(), NULL, pkey) ||
      !EVP_DigestVerifyUpdate(&md_ctx, kMsg, sizeof(kMsg)) ||
      !EVP_DigestVerifyFinal(&md_ctx, kSignature, sizeof(kSignature))) {
    goto out;
  }
  ret = 1;

out:
  if (!ret) {
    BIO_print_errors_fp(stderr);
  }

  EVP_MD_CTX_cleanup(&md_ctx);
  if (pkey) {
    EVP_PKEY_free(pkey);
  }

  return ret;
}

/* test_algorithm_roundtrip signs a message using an already-initialized
 * |md_ctx|, sampling the AlgorithmIdentifier. It then uses |pkey| and the
 * AlgorithmIdentifier to verify the signature. */
static int test_algorithm_roundtrip(EVP_MD_CTX *md_ctx, EVP_PKEY *pkey) {
  int ret = 0;
  uint8_t *sig = NULL;
  size_t sig_len = 0;
  EVP_MD_CTX md_ctx_verify;
  X509_ALGOR *algor = NULL;

  EVP_MD_CTX_init(&md_ctx_verify);

  if (!EVP_DigestSignUpdate(md_ctx, kMsg, sizeof(kMsg))) {
    goto out;
  }

  /* Save the algorithm. */
  algor = X509_ALGOR_new();
  if (algor == NULL || !EVP_DigestSignAlgorithm(md_ctx, algor)) {
    goto out;
  }

  /* Determine the size of the signature. */
  if (!EVP_DigestSignFinal(md_ctx, NULL, &sig_len)) {
    goto out;
  }
  /* Sanity check for testing. */
  if (sig_len != EVP_PKEY_size(pkey)) {
    fprintf(stderr, "sig_len mismatch\n");
    goto out;
  }

  sig = malloc(sig_len);
  if (sig == NULL || !EVP_DigestSignFinal(md_ctx, sig, &sig_len)) {
    goto out;
  }

  /* Ensure that the signature round-trips. */
  if (!EVP_DigestVerifyInitFromAlgorithm(&md_ctx_verify, algor, pkey) ||
      !EVP_DigestVerifyUpdate(&md_ctx_verify, kMsg, sizeof(kMsg)) ||
      !EVP_DigestVerifyFinal(&md_ctx_verify, sig, sig_len)) {
    goto out;
  }

  ret = 1;

out:
  EVP_MD_CTX_cleanup(&md_ctx_verify);
  if (sig) {
    free(sig);
  }
  if (algor) {
    X509_ALGOR_free(algor);
  }

  return ret;
}

static int test_EVP_DigestSignAlgorithm(void) {
  int ret = 0;
  EVP_PKEY *pkey = NULL;
  EVP_MD_CTX md_ctx;
  EVP_PKEY_CTX *pkey_ctx;

  EVP_MD_CTX_init(&md_ctx);

  pkey = load_example_rsa_key();
  if (pkey == NULL) {
    goto out;
  }

  /* Test a simple AlgorithmIdentifier. */
  if (!EVP_DigestSignInit(&md_ctx, &pkey_ctx, EVP_sha256(), NULL, pkey) ||
      !test_algorithm_roundtrip(&md_ctx, pkey)) {
    fprintf(stderr, "RSA with SHA-256 failed\n");
    goto out;
  }

  EVP_MD_CTX_cleanup(&md_ctx);
  EVP_MD_CTX_init(&md_ctx);

  /* Test RSA-PSS with custom parameters. */
  if (!EVP_DigestSignInit(&md_ctx, &pkey_ctx, EVP_sha256(), NULL, pkey) ||
      !EVP_PKEY_CTX_set_rsa_padding(pkey_ctx, RSA_PKCS1_PSS_PADDING) ||
      !EVP_PKEY_CTX_set_rsa_mgf1_md(pkey_ctx, EVP_sha512()) ||
      !test_algorithm_roundtrip(&md_ctx, pkey)) {
    fprintf(stderr, "RSA-PSS failed\n");
    goto out;
  }

  ret = 1;

out:
  if (!ret) {
    BIO_print_errors_fp(stderr);
  }

  EVP_MD_CTX_cleanup(&md_ctx);
  if (pkey) {
    EVP_PKEY_free(pkey);
  }

  return ret;
}

static int test_EVP_DigestVerifyInitFromAlgorithm(void) {
  int ret = 0;
  CBS cert, cert_body, tbs_cert, algorithm, signature;
  uint8_t padding;
  X509_ALGOR *algor = NULL;
  const uint8_t *derp;
  EVP_PKEY *pkey = NULL;
  EVP_MD_CTX md_ctx;

  EVP_MD_CTX_init(&md_ctx);

  CBS_init(&cert, kExamplePSSCert, sizeof(kExamplePSSCert));
  if (!CBS_get_asn1(&cert, &cert_body, CBS_ASN1_SEQUENCE) ||
      CBS_len(&cert) != 0 ||
      !CBS_get_any_asn1_element(&cert_body, &tbs_cert, NULL, NULL) ||
      !CBS_get_asn1_element(&cert_body, &algorithm, CBS_ASN1_SEQUENCE) ||
      !CBS_get_asn1(&cert_body, &signature, CBS_ASN1_BITSTRING) ||
      CBS_len(&cert_body) != 0) {
    fprintf(stderr, "Failed to parse certificate\n");
    goto out;
  }

  /* Signatures are BIT STRINGs, but they have are multiple of 8 bytes, so the
     leading phase byte is just a zero. */
  if (!CBS_get_u8(&signature, &padding) || padding != 0) {
    fprintf(stderr, "Invalid signature padding\n");
    goto out;
  }

  derp = CBS_data(&algorithm);
  if (!d2i_X509_ALGOR(&algor, &derp, CBS_len(&algorithm)) ||
      derp != CBS_data(&algorithm) + CBS_len(&algorithm)) {
    fprintf(stderr, "Failed to parse algorithm\n");
  }

  pkey = load_example_rsa_key();
  if (pkey == NULL ||
      !EVP_DigestVerifyInitFromAlgorithm(&md_ctx, algor, pkey) ||
      !EVP_DigestVerifyUpdate(&md_ctx, CBS_data(&tbs_cert),
                              CBS_len(&tbs_cert)) ||
      !EVP_DigestVerifyFinal(&md_ctx, CBS_data(&signature),
                             CBS_len(&signature))) {
    goto out;
  }
  ret = 1;

out:
  if (!ret) {
    BIO_print_errors_fp(stderr);
  }

  EVP_MD_CTX_cleanup(&md_ctx);
  if (pkey) {
    EVP_PKEY_free(pkey);
  }
  if (algor != NULL) {
    X509_ALGOR_free(algor);
  }

  return ret;
}

static int test_d2i_AutoPrivateKey(const uint8_t *input, size_t input_len,
                                   int expected_id) {
  int ret = 0;
  const uint8_t *p;
  EVP_PKEY *pkey = NULL;

  p = input;
  pkey = d2i_AutoPrivateKey(NULL, &p, input_len);
  if (pkey == NULL || p != input + input_len) {
    fprintf(stderr, "d2i_AutoPrivateKey failed\n");
    goto done;
  }

  if (EVP_PKEY_id(pkey) != expected_id) {
    fprintf(stderr, "Did not decode expected type\n");
    goto done;
  }

  ret = 1;

done:
  if (!ret) {
    BIO_print_errors_fp(stderr);
  }

  if (pkey != NULL) {
    EVP_PKEY_free(pkey);
  }
  return ret;
}

/* Tests loading a bad key in PKCS8 format */
static int test_EVP_PKCS82PKEY(void) {
  int ret = 0;
  const uint8_t *derp = kExampleBadECKeyDER;
  PKCS8_PRIV_KEY_INFO *p8inf = NULL;
  EVP_PKEY *pkey = NULL;

  p8inf = d2i_PKCS8_PRIV_KEY_INFO(NULL, &derp, sizeof(kExampleBadECKeyDER));

  if (!p8inf || derp != kExampleBadECKeyDER + sizeof(kExampleBadECKeyDER)) {
    fprintf(stderr, "Failed to parse key\n");
    goto done;
  }

  pkey = EVP_PKCS82PKEY(p8inf);
  if (pkey) {
    fprintf(stderr, "Imported invalid EC key\n");
    goto done;
  }

  ret = 1;

done:
  if (p8inf != NULL) {
    PKCS8_PRIV_KEY_INFO_free(p8inf);
  }

  if (pkey != NULL) {
    EVP_PKEY_free(pkey);
  }

  return ret;
}

int main(void) {
  CRYPTO_library_init();
  ERR_load_crypto_strings();

  if (!test_EVP_DigestSignInit()) {
    fprintf(stderr, "EVP_DigestSignInit failed\n");
    return 1;
  }

  if (!test_EVP_DigestVerifyInit()) {
    fprintf(stderr, "EVP_DigestVerifyInit failed\n");
    return 1;
  }

  if (!test_EVP_DigestSignAlgorithm()) {
    fprintf(stderr, "EVP_DigestSignInit failed\n");
    return 1;
  }

  if (!test_EVP_DigestVerifyInitFromAlgorithm()) {
    fprintf(stderr, "EVP_DigestVerifyInitFromAlgorithm failed\n");
    return 1;
  }

  if (!test_d2i_AutoPrivateKey(kExampleRSAKeyDER, sizeof(kExampleRSAKeyDER),
                               EVP_PKEY_RSA)) {
    fprintf(stderr, "d2i_AutoPrivateKey(kExampleRSAKeyDER) failed\n");
    return 1;
  }

  if (!test_d2i_AutoPrivateKey(kExampleRSAKeyPKCS8, sizeof(kExampleRSAKeyPKCS8),
                               EVP_PKEY_RSA)) {
    fprintf(stderr, "d2i_AutoPrivateKey(kExampleRSAKeyPKCS8) failed\n");
    return 1;
  }

  if (!test_d2i_AutoPrivateKey(kExampleECKeyDER, sizeof(kExampleECKeyDER),
                               EVP_PKEY_EC)) {
    fprintf(stderr, "d2i_AutoPrivateKey(kExampleECKeyDER) failed\n");
    return 1;
  }

  if (!test_EVP_PKCS82PKEY()) {
    fprintf(stderr, "test_EVP_PKCS82PKEY failed\n");
    return 1;
  }

  printf("PASS\n");
  return 0;
}
