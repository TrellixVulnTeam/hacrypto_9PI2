/* ssl/d1_meth.h */
/*
 * DTLS implementation written by Nagendra Modadugu
 * (nagendra@cs.stanford.edu) for the OpenSSL project 2005. 
 */
/* ====================================================================
 * Copyright (c) 1999-2005 The OpenSSL Project.  All rights reserved.
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
 * Hudson (tjh@cryptsoft.com). */

#include "ssl_locl.h"


static const SSL_PROTOCOL_METHOD DTLS_protocol_method = {
    1 /* is_dtls */,
    dtls1_new,
    dtls1_free,
    dtls1_accept,
    dtls1_connect,
    ssl3_read,
    ssl3_peek,
    ssl3_write,
    dtls1_shutdown,
    ssl3_renegotiate,
    ssl3_renegotiate_check,
    dtls1_get_message,
    dtls1_read_bytes,
    dtls1_write_app_data_bytes,
    dtls1_dispatch_alert,
    dtls1_ctrl,
    ssl3_ctx_ctrl,
    ssl3_pending,
    ssl3_num_ciphers,
    dtls1_get_cipher,
    ssl3_callback_ctrl,
    ssl3_ctx_callback_ctrl,
    DTLS1_HM_HEADER_LENGTH,
    dtls1_set_handshake_header,
    dtls1_handshake_write,
};

const SSL_METHOD *DTLS_method(void) {
  static const SSL_METHOD method = {
      0,
      &DTLS_protocol_method,
  };
  return &method;
}

/* Legacy version-locked methods. */

const SSL_METHOD *DTLSv1_2_method(void) {
  static const SSL_METHOD method = {
      DTLS1_2_VERSION,
      &DTLS_protocol_method,
  };
  return &method;
}

const SSL_METHOD *DTLSv1_method(void) {
  static const SSL_METHOD method = {
      DTLS1_VERSION,
      &DTLS_protocol_method,
  };
  return &method;
}

/* Legacy side-specific methods. */

const SSL_METHOD *DTLSv1_2_server_method(void) {
  return DTLSv1_2_method();
}

const SSL_METHOD *DTLSv1_server_method(void) {
  return DTLSv1_method();
}

const SSL_METHOD *DTLSv1_2_client_method(void) {
  return DTLSv1_2_method();
}

const SSL_METHOD *DTLSv1_client_method(void) {
  return DTLSv1_method();
}

const SSL_METHOD *DTLS_server_method(void) {
  return DTLS_method();
}

const SSL_METHOD *DTLS_client_method(void) {
  return DTLS_method();
}
