/*
 * Copyright (C) 2013 Nikos Mavrogiannopoulos
 * 
 * This file is part of GnuTLS.
 *
 * The GnuTLS is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>
 *
 */

#include "gnutls_int.h"
#include "gnutls_auth.h"
#include "gnutls_errors.h"
#include "gnutls_num.h"
#include <ext/dumbfw.h>

static int _gnutls_dumbfw_send_params(gnutls_session_t session,
				    gnutls_buffer_st * extdata);

extension_entry_st ext_mod_dumbfw = {
	.name = "DUMBFW",
	.type = GNUTLS_EXTENSION_DUMBFW,
	.parse_type = GNUTLS_EXT_APPLICATION,

	.recv_func = NULL,
	.send_func = _gnutls_dumbfw_send_params,
	.pack_func = NULL,
	.unpack_func = NULL,
	.deinit_func = NULL,
};

static int
_gnutls_dumbfw_send_params(gnutls_session_t session,
			 gnutls_buffer_st * extdata)
{
	int total_size = 0, ret;
	uint8_t pad[257];
	unsigned pad_size;

	if (session->security_parameters.entity == GNUTLS_SERVER ||
	    session->internals.priorities.dumbfw == 0 ||
	    IS_DTLS(session) != 0 ||
	    (extdata->length < 256 || extdata->length >= 512)) {
		return 0;
	} else {
		/* 256 <= extdata->length < 512 */
		pad_size = 512 - extdata->length;
	        memset(pad, 0, pad_size);

		ret =
		    _gnutls_buffer_append_data_prefix(extdata, 16,
							      pad,
							      pad_size);
		if (ret < 0)
			return gnutls_assert_val(ret);

		total_size += 2 + pad_size;
	}

	return total_size;
}

