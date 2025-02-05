/*
 * RELIC is an Efficient LIbrary for Cryptography
 * Copyright (C) 2007-2013 RELIC Authors
 *
 * This file is part of RELIC. RELIC is legal property of its developers,
 * whose names are not listed here. Please refer to the COPYRIGHT file
 * for contact information.
 *
 * RELIC is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * RELIC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with RELIC. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * @file
 *
 * Implementation of configuration of prime elliptic curves over quadratic
 * extensions.
 *
 * @version $Id: relic_ep2_curve.c 1589 2013-09-03 08:19:40Z dfaranha $
 * @ingroup epx
 */

#include "relic_core.h"

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

#if defined(EP_KBLTZ) && FP_PRIME == 158
/**
 * Parameters for a pairing-friendly prime curve over a quadratic extension.
 */
/** @{ */
#define BN_P158_A0		"0"
#define BN_P158_A1		"0"
#define BN_P158_B0		"4"
#define BN_P158_B1		"240000006ED000007FE9C000419FEC800CA035C6"
#define BN_P158_X0		"172C0A466DAFB4ACF48C9BDD0C12A435CB36CE6C"
#define BN_P158_X1		"0CE0287269D7E317EB91AF3DCD27CC373114299E"
#define BN_P158_Y0		"19A185D6B6241576480E965463B4A6A66875C184"
#define BN_P158_Y1		"074866EA7BD0AB4C67C77F70E0467F1FF32D800D"
#define BN_P158_R		"240000006ED000007FE96000419F59800C9FFD81"
/** @} */
#endif

#if defined(EP_KBLTZ) && FP_PRIME == 254
/**
 * Parameters for a pairing-friendly prime curve over a quadratic extension.
 */
/** @{ */
#define BN_P254_A0		"0"
#define BN_P254_A1		"0"
#define BN_P254_B0		"1"
#define BN_P254_B1		"2523648240000001BA344D80000000086121000000000013A700000000000012"
#define BN_P254_X0		"061A10BB519EB62FEB8D8C7E8C61EDB6A4648BBB4898BF0D91EE4224C803FB2B"
#define BN_P254_X1		"0516AAF9BA737833310AA78C5982AA5B1F4D746BAE3784B70D8C34C1E7D54CF3"
#define BN_P254_Y0		"021897A06BAF93439A90E096698C822329BD0AE6BDBE09BD19F0E07891CD2B9A"
#define BN_P254_Y1		"0EBB2B0E7C8B15268F6D4456F5F38D37B09006FFD739C9578A2D1AEC6B3ACE9B"
#define BN_P254_R		"2523648240000001BA344D8000000007FF9F800000000010A10000000000000D"
/** @} */
#endif

#if defined(EP_KBLTZ) && FP_PRIME == 256
/**
 * Parameters for a pairing-friendly prime curve over a quadratic extension.
 */
/** @{ */
#define BN_P256_A0		"0"
#define BN_P256_A1		"0"
#define BN_P256_B0		"4"
#define BN_P256_B1		"B64000000000FF2F2200000085FD5480B0001F44B6B88BF142BC818F95E3E6AE"
#define BN_P256_X0		"0C77AE4A1D6E145166739CF23DAFACA9DD396E9046424FC5479BD57692904538"
#define BN_P256_X1		"8D1705B45D9EAAD78A9198FD8D76E2013D1BC119B4D95721A8D32F819A544F51"
#define BN_P256_Y0		"A906E963E4988478E458A4959EF7D61B570358814E28A04EF9B8C794064D73A7"
#define BN_P256_Y1		"A033144CA161E3E3271624B3F0CC1CE607ACD2CBCE9E9253C732CF3E1016DEE7"
#define BN_P256_R		"B64000000000FF2F2200000085FD547FD8001F44B6B7F4B7C2BC818F7B6BEF99"
/** @} */
#endif

#if defined(EP_KBLTZ) && FP_PRIME == 638
/**
 * Parameters for a pairing-friendly prime curve over a quadratic extension.
 */
/** @{ */
#define BN_P638_A0		"0"
#define BN_P638_A1		"0"
#define BN_P638_B0		"2"
#define BN_P638_B1		"23FFFFFDC000000D7FFFFFB8000001D3FFFFF942D000165E3FFF94870000D52FFFFDD0E00008DE55C00086520021E55BFFFFF51FFFF4EB800000004C80015ACDFFFFFFFFFFFFECE00000000000000066"
#define BN_P638_X0		"C6BA9612456EFF0E3CD291C9C1A9116FB5EEF4992E052BC5C5126F0F55F67A7D190ED74C3D6229BC3D2F645328C94554AA032352A4D7D667542F793C8FEA25AD39606CA97025AA6EF16BAC2438B1DD3"
#define BN_P638_X1		"17BE713D379D46F3D77CFF94B7226EFFB4AD01CC67A8BA712DECB3FE8FFE58A027A45523200BF6FDA534F3F59763A1F6A6461F5D2DCAC172774C0CD24BA091A37B42C6E89A1E92F3B12E3B5AFFC222BB"
#define BN_P638_Y0		"E4197B30E3A9DD98A75E6C4D2C6561B6B96083E943230D578E944E2354482212ADAAA94CA54FC4A29D6CA873EFFB27C4B61B9B822C3C217D388C6C5D04C821F1A3A8A13A37C9807323AE9CAEDD021EC"
#define BN_P638_Y1		"1A650343ACEF6895FE4EC59B49F40E043DEB05DEF170DFD71B44CAB9496E2EADD034EC0E9238544556902D2D51AB93D224DC757AD720F4DE8ED3BFA4E22DB0ECE92369F681543F23A908A9B319D5FAEF"
#define BN_P638_R		"23FFFFFDC000000D7FFFFFB8000001D3FFFFF942D000165E3FFF94870000D52FFFFDD0E00008DE55600086550021E555FFFFF54FFFF4EAC000000049800154D9FFFFFFFFFFFFEDA00000000000000061"
/** @} */
#endif

#if defined(EP_KBLTZ) && FP_PRIME == 638
/**
 * Parameters for a pairing-friendly prime curve over a quadratic extension.
 */
/** @{ */
#define B12_P638_A0		"0"
#define B12_P638_A1		"0"
#define B12_P638_B0		"4"
#define B12_P638_B1		"4"
#define B12_P638_X0		"3C57F8F05130D336804E30CAA7BB45E06244DFC0BA836056B036038703719449A42CCC7C34452B4EE2DCA3CBCE0B7637E14E9CA88BDEF105440FB3F84AA95C75DE0BA05686394492B8648BB71D5E7F39"
#define B12_P638_X1		"07B30040203566584002D6DBB49A3DA1D99ECA3CBCD113C07E0CF1FFB3FA4F87F034A034C86F56DB380F2810AC329ED8BD6FE0F4D5C1FA26949739AF82D3AAD4702D2186862B0293E16C5EDDDDA3C922"
#define B12_P638_Y0		"29ED1A1C4F3F5AFC64AB2BA97CFA4D17998061179331A1C34E024B7D82134C60A3569F644E4155753C48698C8A01C80C0C3CEC9E3BDE2E5E22D81BBB514FD24DE186FEBA69B82E88809BFCCE51A1840F"
#define B12_P638_Y1		"3795191221DB4917EEE4B7B85BC7D7CA0C60E82116064463FED0892BA82ACECF905E6DB8083C5F589F04DB80E3203C1B2BEB52ACDED6DF96FC515F36761E7152AEED13369A504FE38C4FF93860B89550"
#define B12_P638_R		"50F94035FF4000FFFFFFFFFFF9406BFDC0040000000000000035FB801DFFBFFFFFFFFFFFFFFF401BFF80000000000000000000FFC01"
/** @} */
#endif

/**
 * Assigns a set of ordinary elliptic curve parameters.
 *
 * @param[in] CURVE		- the curve parameters to assign.
 */
#define ASSIGN(CURVE)														\
	FETCH(str, CURVE##_A0, sizeof(CURVE##_A0));								\
	fp_read(a[0], str, strlen(str), 16);									\
	FETCH(str, CURVE##_A1, sizeof(CURVE##_A1));								\
	fp_read(a[1], str, strlen(str), 16);									\
	FETCH(str, CURVE##_B0, sizeof(CURVE##_B0));								\
	fp_read(b[0], str, strlen(str), 16);									\
	FETCH(str, CURVE##_B1, sizeof(CURVE##_B1));								\
	fp_read(b[1], str, strlen(str), 16);									\
	FETCH(str, CURVE##_X0, sizeof(CURVE##_X0));								\
	fp_read(g->x[0], str, strlen(str), 16);									\
	FETCH(str, CURVE##_X1, sizeof(CURVE##_X1));								\
	fp_read(g->x[1], str, strlen(str), 16);									\
	FETCH(str, CURVE##_Y0, sizeof(CURVE##_Y0));								\
	fp_read(g->y[0], str, strlen(str), 16);									\
	FETCH(str, CURVE##_Y1, sizeof(CURVE##_Y1));								\
	fp_read(g->y[1], str, strlen(str), 16);									\
	FETCH(str, CURVE##_R, sizeof(CURVE##_R));								\
	bn_read_str(r, str, strlen(str), 16);									\

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

void ep2_curve_init(void) {
	ctx_t *ctx = core_get();

#ifdef EP_PRECO
	for (int i = 0; i < EP_TABLE; i++) {
		ctx->ep2_ptr[i] = &(ctx->ep2_pre[i]);
	}
#endif

#if ALLOC == STATIC || ALLOC == DYNAMIC || ALLOC == STACK
	ctx->ep2_g.x[0] = ctx->ep2_gx[0];
	ctx->ep2_g.x[1] = ctx->ep2_gx[1];
	ctx->ep2_g.y[0] = ctx->ep2_gy[0];
	ctx->ep2_g.y[1] = ctx->ep2_gy[1];
	ctx->ep2_g.z[0] = ctx->ep2_gz[0];
	ctx->ep2_g.z[1] = ctx->ep2_gz[1];
#endif

#ifdef EP_PRECO
#if ALLOC == STATIC || ALLOC == DYNAMIC
	for (int i = 0; i < EP_TABLE; i++) {
		fp2_new(ctx->ep2_pre[i].x);
		fp2_new(ctx->ep2_pre[i].y);
		fp2_new(ctx->ep2_pre[i].z);
	}
#elif ALLOC == STACK
	for (int i = 0; i < EP_TABLE; i++) {
		ctx->ep2_pre[i].x[0] = ctx->_ep2_pre[3 * i][0];
		ctx->ep2_pre[i].x[1] = ctx->_ep2_pre[3 * i][1];
		ctx->ep2_pre[i].y[0] = ctx->_ep2_pre[3 * i + 1][0];
		ctx->ep2_pre[i].y[1] = ctx->_ep2_pre[3 * i + 1][1];
		ctx->ep2_pre[i].z[0] = ctx->_ep2_pre[3 * i + 2][0];
		ctx->ep2_pre[i].z[1] = ctx->_ep2_pre[3 * i + 2][1];
	}
#endif
#endif
	ep2_set_infty(&(ctx->ep2_g));
	bn_init(&(ctx->ep2_r), FP_DIGS);
}

void ep2_curve_clean(void) {
	ctx_t *ctx = core_get();
#ifdef EP_PRECO
	for (int i = 0; i < EP_TABLE; i++) {
		fp2_free(ctx->ep2_pre[i].x);
		fp2_free(ctx->ep2_pre[i].y);
		fp2_free(ctx->ep2_pre[i].z);
	}
#endif
	bn_clean(&(ctx->ep2_r));
}

int ep2_curve_is_twist() {
	return core_get()->ep2_is_twist;
}

void ep2_curve_get_gen(ep2_t g) {
	ep2_copy(g, &(core_get()->ep2_g));
}

void ep2_curve_get_a(fp2_t a) {
	ctx_t *ctx = core_get();
	fp_copy(a[0], ctx->ep2_a[0]);
	fp_copy(a[1], ctx->ep2_a[1]);
}

void ep2_curve_get_b(fp2_t b) {
	ctx_t *ctx = core_get();
	fp_copy(b[0], ctx->ep2_b[0]);
	fp_copy(b[1], ctx->ep2_b[1]);
}

void ep2_curve_get_ord(bn_t n) {
	ctx_t *ctx = core_get();
	if (ctx->ep2_is_twist) {
		ep_curve_get_ord(n);
	} else {
		bn_copy(n, &(ctx->ep2_r));
	}
}

#if defined(EP_PRECO)

ep2_t *ep2_curve_get_tab() {
#if ALLOC == AUTO
	return (ep2_t *)*(core_get()->ep2_ptr);
#else
	return core_get()->ep2_ptr;
#endif
}

#endif

void ep2_curve_set(int twist) {
	char str[2 * FP_BYTES + 1];
	ctx_t *ctx = core_get();
	ep2_t g;
	fp2_t a;
	fp2_t b;
	bn_t r;

	ep2_null(g);
	fp2_null(a);
	fp2_null(b);
	bn_null(r);

	TRY {
		ep2_new(g);
		fp2_new(a);
		fp2_new(b);
		bn_new(r);

		switch (ep_param_get()) {
#if FP_PRIME == 158
			case BN_P158:
				ASSIGN(BN_P158);
				break;
#elif FP_PRIME == 254
			case BN_P254:
				ASSIGN(BN_P254);
				break;
#elif FP_PRIME == 256
			case BN_P256:
				ASSIGN(BN_P256);
				break;
#elif FP_PRIME == 638
			case BN_P638:
				ASSIGN(BN_P638);
				break;
			case B12_P638:
				ASSIGN(B12_P638);
				break;
#endif
			default:
				(void)str;
				THROW(ERR_NO_VALID);
				break;
		}

		fp2_zero(g->z);
		fp_set_dig(g->z[0], 1);
		g->norm = 1;

		ctx->ep2_is_twist = twist;

		ep2_copy(&(ctx->ep2_g), g);
		fp_copy(ctx->ep2_a[0], a[0]);
		fp_copy(ctx->ep2_a[1], a[1]);
		fp_copy(ctx->ep2_b[0], b[0]);
		fp_copy(ctx->ep2_b[1], b[1]);
		bn_copy(&(ctx->ep2_r), r);

		/* I don't have a better place for this. */
		fp_prime_calc();

#if defined(EP_PRECO)
		ep2_mul_pre(ep2_curve_get_tab(), &(ctx->ep2_g));
#endif
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		ep2_free(g);
		fp2_free(a);
		fp2_free(b);
		bn_free(r);
	}
}
