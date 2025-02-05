/* $OpenBSD: ts_err.c,v 1.3 2014/06/12 15:49:31 deraadt Exp $ */
/* ====================================================================
 * Copyright (c) 1999-2007 The OpenSSL Project.  All rights reserved.
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
 *    openssl-core@OpenSSL.org.
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

/* NOTE: this file was auto generated by the mkerr.pl script: any changes
 * made to it will be overwritten when the script next updates this file,
 * only reason strings will be preserved.
 */

#include <stdio.h>
#include <openssl/err.h>
#include <openssl/ts.h>

/* BEGIN ERROR CODES */
#ifndef OPENSSL_NO_ERR

#define ERR_FUNC(func) ERR_PACK(ERR_LIB_TS,func,0)
#define ERR_REASON(reason) ERR_PACK(ERR_LIB_TS,0,reason)

static ERR_STRING_DATA TS_str_functs[] = {
	{ERR_FUNC(TS_F_D2I_TS_RESP),	"d2i_TS_RESP"},
	{ERR_FUNC(TS_F_DEF_SERIAL_CB),	"DEF_SERIAL_CB"},
	{ERR_FUNC(TS_F_DEF_TIME_CB),	"DEF_TIME_CB"},
	{ERR_FUNC(TS_F_ESS_ADD_SIGNING_CERT),	"ESS_ADD_SIGNING_CERT"},
	{ERR_FUNC(TS_F_ESS_CERT_ID_NEW_INIT),	"ESS_CERT_ID_NEW_INIT"},
	{ERR_FUNC(TS_F_ESS_SIGNING_CERT_NEW_INIT),	"ESS_SIGNING_CERT_NEW_INIT"},
	{ERR_FUNC(TS_F_INT_TS_RESP_VERIFY_TOKEN),	"INT_TS_RESP_VERIFY_TOKEN"},
	{ERR_FUNC(TS_F_PKCS7_TO_TS_TST_INFO),	"PKCS7_to_TS_TST_INFO"},
	{ERR_FUNC(TS_F_TS_ACCURACY_SET_MICROS),	"TS_ACCURACY_set_micros"},
	{ERR_FUNC(TS_F_TS_ACCURACY_SET_MILLIS),	"TS_ACCURACY_set_millis"},
	{ERR_FUNC(TS_F_TS_ACCURACY_SET_SECONDS),	"TS_ACCURACY_set_seconds"},
	{ERR_FUNC(TS_F_TS_CHECK_IMPRINTS),	"TS_CHECK_IMPRINTS"},
	{ERR_FUNC(TS_F_TS_CHECK_NONCES),	"TS_CHECK_NONCES"},
	{ERR_FUNC(TS_F_TS_CHECK_POLICY),	"TS_CHECK_POLICY"},
	{ERR_FUNC(TS_F_TS_CHECK_SIGNING_CERTS),	"TS_CHECK_SIGNING_CERTS"},
	{ERR_FUNC(TS_F_TS_CHECK_STATUS_INFO),	"TS_CHECK_STATUS_INFO"},
	{ERR_FUNC(TS_F_TS_COMPUTE_IMPRINT),	"TS_COMPUTE_IMPRINT"},
	{ERR_FUNC(TS_F_TS_CONF_SET_DEFAULT_ENGINE),	"TS_CONF_set_default_engine"},
	{ERR_FUNC(TS_F_TS_GET_STATUS_TEXT),	"TS_GET_STATUS_TEXT"},
	{ERR_FUNC(TS_F_TS_MSG_IMPRINT_SET_ALGO),	"TS_MSG_IMPRINT_set_algo"},
	{ERR_FUNC(TS_F_TS_REQ_SET_MSG_IMPRINT),	"TS_REQ_set_msg_imprint"},
	{ERR_FUNC(TS_F_TS_REQ_SET_NONCE),	"TS_REQ_set_nonce"},
	{ERR_FUNC(TS_F_TS_REQ_SET_POLICY_ID),	"TS_REQ_set_policy_id"},
	{ERR_FUNC(TS_F_TS_RESP_CREATE_RESPONSE),	"TS_RESP_create_response"},
	{ERR_FUNC(TS_F_TS_RESP_CREATE_TST_INFO),	"TS_RESP_CREATE_TST_INFO"},
	{ERR_FUNC(TS_F_TS_RESP_CTX_ADD_FAILURE_INFO),	"TS_RESP_CTX_add_failure_info"},
	{ERR_FUNC(TS_F_TS_RESP_CTX_ADD_MD),	"TS_RESP_CTX_add_md"},
	{ERR_FUNC(TS_F_TS_RESP_CTX_ADD_POLICY),	"TS_RESP_CTX_add_policy"},
	{ERR_FUNC(TS_F_TS_RESP_CTX_NEW),	"TS_RESP_CTX_new"},
	{ERR_FUNC(TS_F_TS_RESP_CTX_SET_ACCURACY),	"TS_RESP_CTX_set_accuracy"},
	{ERR_FUNC(TS_F_TS_RESP_CTX_SET_CERTS),	"TS_RESP_CTX_set_certs"},
	{ERR_FUNC(TS_F_TS_RESP_CTX_SET_DEF_POLICY),	"TS_RESP_CTX_set_def_policy"},
	{ERR_FUNC(TS_F_TS_RESP_CTX_SET_SIGNER_CERT),	"TS_RESP_CTX_set_signer_cert"},
	{ERR_FUNC(TS_F_TS_RESP_CTX_SET_STATUS_INFO),	"TS_RESP_CTX_set_status_info"},
	{ERR_FUNC(TS_F_TS_RESP_GET_POLICY),	"TS_RESP_GET_POLICY"},
	{ERR_FUNC(TS_F_TS_RESP_SET_GENTIME_WITH_PRECISION),	"TS_RESP_SET_GENTIME_WITH_PRECISION"},
	{ERR_FUNC(TS_F_TS_RESP_SET_STATUS_INFO),	"TS_RESP_set_status_info"},
	{ERR_FUNC(TS_F_TS_RESP_SET_TST_INFO),	"TS_RESP_set_tst_info"},
	{ERR_FUNC(TS_F_TS_RESP_SIGN),	"TS_RESP_SIGN"},
	{ERR_FUNC(TS_F_TS_RESP_VERIFY_SIGNATURE),	"TS_RESP_verify_signature"},
	{ERR_FUNC(TS_F_TS_RESP_VERIFY_TOKEN),	"TS_RESP_verify_token"},
	{ERR_FUNC(TS_F_TS_TST_INFO_SET_ACCURACY),	"TS_TST_INFO_set_accuracy"},
	{ERR_FUNC(TS_F_TS_TST_INFO_SET_MSG_IMPRINT),	"TS_TST_INFO_set_msg_imprint"},
	{ERR_FUNC(TS_F_TS_TST_INFO_SET_NONCE),	"TS_TST_INFO_set_nonce"},
	{ERR_FUNC(TS_F_TS_TST_INFO_SET_POLICY_ID),	"TS_TST_INFO_set_policy_id"},
	{ERR_FUNC(TS_F_TS_TST_INFO_SET_SERIAL),	"TS_TST_INFO_set_serial"},
	{ERR_FUNC(TS_F_TS_TST_INFO_SET_TIME),	"TS_TST_INFO_set_time"},
	{ERR_FUNC(TS_F_TS_TST_INFO_SET_TSA),	"TS_TST_INFO_set_tsa"},
	{ERR_FUNC(TS_F_TS_VERIFY),	"TS_VERIFY"},
	{ERR_FUNC(TS_F_TS_VERIFY_CERT),	"TS_VERIFY_CERT"},
	{ERR_FUNC(TS_F_TS_VERIFY_CTX_NEW),	"TS_VERIFY_CTX_new"},
	{0, NULL}
};

static ERR_STRING_DATA TS_str_reasons[]= {
	{ERR_REASON(TS_R_BAD_PKCS7_TYPE)         , "bad pkcs7 type"},
	{ERR_REASON(TS_R_BAD_TYPE)               , "bad type"},
	{ERR_REASON(TS_R_CERTIFICATE_VERIFY_ERROR), "certificate verify error"},
	{ERR_REASON(TS_R_COULD_NOT_SET_ENGINE)   , "could not set engine"},
	{ERR_REASON(TS_R_COULD_NOT_SET_TIME)     , "could not set time"},
	{ERR_REASON(TS_R_D2I_TS_RESP_INT_FAILED) , "d2i ts resp int failed"},
	{ERR_REASON(TS_R_DETACHED_CONTENT)       , "detached content"},
	{ERR_REASON(TS_R_ESS_ADD_SIGNING_CERT_ERROR), "ess add signing cert error"},
	{ERR_REASON(TS_R_ESS_SIGNING_CERTIFICATE_ERROR), "ess signing certificate error"},
	{ERR_REASON(TS_R_INVALID_NULL_POINTER)   , "invalid null pointer"},
	{ERR_REASON(TS_R_INVALID_SIGNER_CERTIFICATE_PURPOSE), "invalid signer certificate purpose"},
	{ERR_REASON(TS_R_MESSAGE_IMPRINT_MISMATCH), "message imprint mismatch"},
	{ERR_REASON(TS_R_NONCE_MISMATCH)         , "nonce mismatch"},
	{ERR_REASON(TS_R_NONCE_NOT_RETURNED)     , "nonce not returned"},
	{ERR_REASON(TS_R_NO_CONTENT)             , "no content"},
	{ERR_REASON(TS_R_NO_TIME_STAMP_TOKEN)    , "no time stamp token"},
	{ERR_REASON(TS_R_PKCS7_ADD_SIGNATURE_ERROR), "pkcs7 add signature error"},
	{ERR_REASON(TS_R_PKCS7_ADD_SIGNED_ATTR_ERROR), "pkcs7 add signed attr error"},
	{ERR_REASON(TS_R_PKCS7_TO_TS_TST_INFO_FAILED), "pkcs7 to ts tst info failed"},
	{ERR_REASON(TS_R_POLICY_MISMATCH)        , "policy mismatch"},
	{ERR_REASON(TS_R_PRIVATE_KEY_DOES_NOT_MATCH_CERTIFICATE), "private key does not match certificate"},
	{ERR_REASON(TS_R_RESPONSE_SETUP_ERROR)   , "response setup error"},
	{ERR_REASON(TS_R_SIGNATURE_FAILURE)      , "signature failure"},
	{ERR_REASON(TS_R_THERE_MUST_BE_ONE_SIGNER), "there must be one signer"},
	{ERR_REASON(TS_R_TIME_SYSCALL_ERROR)     , "time syscall error"},
	{ERR_REASON(TS_R_TOKEN_NOT_PRESENT)      , "token not present"},
	{ERR_REASON(TS_R_TOKEN_PRESENT)          , "token present"},
	{ERR_REASON(TS_R_TSA_NAME_MISMATCH)      , "tsa name mismatch"},
	{ERR_REASON(TS_R_TSA_UNTRUSTED)          , "tsa untrusted"},
	{ERR_REASON(TS_R_TST_INFO_SETUP_ERROR)   , "tst info setup error"},
	{ERR_REASON(TS_R_TS_DATASIGN)            , "ts datasign"},
	{ERR_REASON(TS_R_UNACCEPTABLE_POLICY)    , "unacceptable policy"},
	{ERR_REASON(TS_R_UNSUPPORTED_MD_ALGORITHM), "unsupported md algorithm"},
	{ERR_REASON(TS_R_UNSUPPORTED_VERSION)    , "unsupported version"},
	{ERR_REASON(TS_R_WRONG_CONTENT_TYPE)     , "wrong content type"},
	{0, NULL}
};

#endif

void
ERR_load_TS_strings(void)
{
#ifndef OPENSSL_NO_ERR
	if (ERR_func_error_string(TS_str_functs[0].error) == NULL) {
		ERR_load_strings(0, TS_str_functs);
		ERR_load_strings(0, TS_str_reasons);
	}
#endif
}
