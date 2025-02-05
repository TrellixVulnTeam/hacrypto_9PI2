/*
 * Copyright (C) 2011-2012 Free Software Foundation, Inc.
 *
 * Author: Simon Josefsson
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

#ifndef VERIFY_HIGH_H
# define VERIFY_HIGH_H

struct gnutls_x509_trust_list_st {
	unsigned int size;
	struct node_st *node;

	gnutls_x509_crt_t *blacklisted;
	unsigned int blacklisted_size;
	
	char* pkcs11_token;
};

int _gnutls_trustlist_inlist(gnutls_x509_trust_list_t list,
			     gnutls_x509_crt_t cert);

#endif
