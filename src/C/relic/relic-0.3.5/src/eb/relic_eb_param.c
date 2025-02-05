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
 * Implementation of the binary elliptic curve utilities.
 *
 * @version $Id: relic_eb_param.c 1520 2013-08-27 15:52:42Z dfaranha $
 * @ingroup eb
 */

#include <string.h>

#include "relic_core.h"
#include "relic_eb.h"
#include "relic_util.h"
#include "relic_conf.h"
#include "relic_arch.h"

/*============================================================================*/
/* Private definitions                                                        */
/*============================================================================*/

#if defined(EB_ORDIN) && FB_POLYN == 163
/**
 * Parameters for the NIST B-163 binary elliptic curve.
 */
/** @{ */
#define NIST_B163_A		"1"
#define NIST_B163_B		"20A601907B8C953CA1481EB10512F78744A3205FD"
#define NIST_B163_X		"3F0EBA16286A2D57EA0991168D4994637E8343E36"
#define NIST_B163_Y		"0D51FBC6C71A0094FA2CDD545B11C5C0C797324F1"
#define NIST_B163_R		"40000000000000000000292FE77E70C12A4234C33"
#define NIST_B163_H		"2"
/** @} */
#endif

#if defined(EB_KBLTZ) && FB_POLYN == 163
/**
 * Parameters for the NIST K-163 binary elliptic curve.
 */
/** @{ */
#define NIST_K163_A		"1"
#define NIST_K163_B		"1"
#define NIST_K163_X		"2FE13C0537BBC11ACAA07D793DE4E6D5E5C94EEE8"
#define NIST_K163_Y		"289070FB05D38FF58321F2E800536D538CCDAA3D9"
#define NIST_K163_R		"4000000000000000000020108A2E0CC0D99F8A5EF"
#define NIST_K163_H		"2"
/** @} */
#endif

#if defined(EB_ORDIN) && FB_POLYN == 233
/**
 * Parameters for the NIST B-233 binary elliptic curve.
 */
/** @{ */
#define NIST_B233_A		"1"
#define NIST_B233_B		"066647EDE6C332C7F8C0923BB58213B333B20E9CE4281FE115F7D8F90AD"
#define NIST_B233_X		"0FAC9DFCBAC8313BB2139F1BB755FEF65BC391F8B36F8F8EB7371FD558B"
#define NIST_B233_Y		"1006A08A41903350678E58528BEBF8A0BEFF867A7CA36716F7E01F81052"
#define NIST_B233_R		"1000000000000000000000000000013E974E72F8A6922031D2603CFE0D7"
#define NIST_B233_H		"2"
/** @} */
#endif

#if defined(EB_KBLTZ) && FB_POLYN == 233
/**
 * Parameters for the NIST K-233 binary elliptic curve.
 */
/** @{ */
#define NIST_K233_A		"0"
#define NIST_K233_B		"1"
#define NIST_K233_X		"17232BA853A7E731AF129F22FF4149563A419C26BF50A4C9D6EEFAD6126"
#define NIST_K233_Y		"1DB537DECE819B7F70F555A67C427A8CD9BF18AEB9B56E0C11056FAE6A3"
#define NIST_K233_R		"08000000000000000000000000000069D5BB915BCD46EFB1AD5F173ABDF"
#define NIST_K233_H		"4"
/** @} */
#endif

#if defined(EB_KBLTZ) && FB_POLYN == 239
/**
 * Parameters for the SECG K-239 binary elliptic curve.
 */
/** @{ */
#define SECG_K239_A		"0"
#define SECG_K239_B		"1"
#define SECG_K239_X		"29A0B6A887A983E9730988A68727A8B2D126C44CC2CC7B2A6555193035DC"
#define SECG_K239_Y		"76310804F12E549BDB011C103089E73510ACB275FC312A5DC6B76553F0CA"
#define SECG_K239_R		"2000000000000000000000000000005A79FEC67CB6E91F1C1DA800E478A5"
#define SECG_K239_H		"4"
/** @} */
#endif

#if defined(EB_ORDIN) && FB_POLYN == 251
/**
 * Parameters for the eBATS B-251 binary elliptic curve.
 */
/** @{ */
#define EBACS_B251_A	"0"
#define EBACS_B251_B	"2387"
#define EBACS_B251_X	"6AD0278D8686F4BA4250B2DE565F0A373AA54D9A154ABEFACB90DC03501D57C"
#define EBACS_B251_Y	"50B1D29DAD5616363249F477B05A1592BA16045BE1A9F218180C5150ABE8573"
#define EBACS_B251_R	"1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF3E3AA131A2E1A8200BEF3B9ABB767E1"
#define EBACS_B251_H	"4"
/** @} */
#endif

#if defined(EB_ORDIN) && FB_POLYN == 257
/**
 * Parameters for a curve over GF(2^257) which is really nice for halving.
 */
/** @{ */
#define HALVE_B257_A	"1"
#define HALVE_B257_B	"0585C1ACA676891D5E1940D6F619FD0DD9FDA8AC46AE9331B76F172EDEF520CF0"
#define HALVE_B257_X	"048E8B0E542D879A1E992E7407F8648F0C8248469B97DD3AF9F21F603E9632D4B"
#define HALVE_B257_Y	"138D19BC9810C03D3FB499801F81C9E6A338B944338514E0D2D9BEDFEB0B8A310"
#define HALVE_B257_R	"0FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFF60349585A0B2E1B2133B4E36C1672179"
#define HALVE_B257_H	"2"
/** @} */
#endif

#if defined(EB_ORDIN) && FB_POLYN == 283
/**
 * Parameters for the NIST B-283 binary elliptic curve.
 */
/** @{ */
#define NIST_B283_A		"1"
#define NIST_B283_B		"027B680AC8B8596DA5A4AF8A19A0303FCA97FD7645309FA2A581485AF6263E313B79A2F5"
#define NIST_B283_X		"05F939258DB7DD90E1934F8C70B0DFEC2EED25B8557EAC9C80E2E198F8CDBECD86B12053"
#define NIST_B283_Y		"03676854FE24141CB98FE6D4B20D02B4516FF702350EDDB0826779C813F0DF45BE8112F4"
#define NIST_B283_R		"03FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFEF90399660FC938A90165B042A7CEFADB307"
#define NIST_B283_H		"2"
/** @} */
#endif

#if defined(EB_KBLTZ) && FB_POLYN == 283
/**
 * Parameters for the NIST K-283 binary elliptic curve.
 */
/** @{ */
#define NIST_K283_A		"0"
#define NIST_K283_B		"1"
#define NIST_K283_X		"0503213F78CA44883F1A3B8162F188E553CD265F23C1567A16876913B0C2AC2458492836"
#define NIST_K283_Y		"01CCDA380F1C9E318D90F95D07E5426FE87E45C0E8184698E45962364E34116177DD2259"
#define NIST_K283_R		"01FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE9AE2ED07577265DFF7F94451E061E163C61"
#define NIST_K283_H		"4"
/** @} */
#endif

#if defined(EB_ORDIN) && FB_POLYN == 409
/**
 * Parameters for the NIST B-409 binary elliptic curve.
 */
/** @{ */
#define NIST_B409_A		"1"
#define NIST_B409_B		"021A5C2C8EE9FEB5C4B9A753B7B476B7FD6422EF1F3DD674761FA99D6AC27C8A9A197B272822F6CD57A55AA4F50AE317B13545F"
#define NIST_B409_X		"15D4860D088DDB3496B0C6064756260441CDE4AF1771D4DB01FFE5B34E59703DC255A868A1180515603AEAB60794E54BB7996A7"
#define NIST_B409_Y		"061B1CFAB6BE5F32BBFA78324ED106A7636B9C5A7BD198D0158AA4F5488D08F38514F1FDF4B4F40D2181B3681C364BA0273C706"
#define NIST_B409_R		"10000000000000000000000000000000000000000000000000001E2AAD6A612F33307BE5FA47C3C9E052F838164CD37D9A21173"
#define NIST_B409_H		"2"
/** @} */
#endif

#if defined(EB_KBLTZ) && FB_POLYN == 409
/**
 * Parameters for the NIST K-409 binary elliptic curve.
 */
/** @{ */
#define NIST_K409_A		"0"
#define NIST_K409_B		"1"
#define NIST_K409_X		"060F05F658F49C1AD3AB1890F7184210EFD0987E307C84C27ACCFB8F9F67CC2C460189EB5AAAA62EE222EB1B35540CFE9023746"
#define NIST_K409_Y		"1E369050B7C4E42ACBA1DACBF04299C3460782F918EA427E6325165E9EA10E3DA5F6C42E9C55215AA9CA27A5863EC48D8E0286B"
#define NIST_K409_R		"07FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE5F83B2D4EA20400EC4557D5ED3E3E7CA5B4B5C83B8E01E5FCF"
#define NIST_K409_H		"4"
/** @} */
#endif

#if defined(EB_ORDIN) && FB_POLYN == 571
/**
 * Parameters for the NIST B-571 binary elliptic curve.
 */
/** @{ */
#define NIST_B571_A		"1"
#define NIST_B571_B		"2F40E7E2221F295DE297117B7F3D62F5C6A97FFCB8CEFF1CD6BA8CE4A9A18AD84FFABBD8EFA59332BE7AD6756A66E294AFD185A78FF12AA520E4DE739BACA0C7FFEFF7F2955727A"
#define NIST_B571_X		"303001D34B856296C16C0D40D3CD7750A93D1D2955FA80AA5F40FC8DB7B2ABDBDE53950F4C0D293CDD711A35B67FB1499AE60038614F1394ABFA3B4C850D927E1E7769C8EEC2D19"
#define NIST_B571_Y		"37BF27342DA639B6DCCFFFEB73D69D78C6C27A6009CBBCA1980F8533921E8A684423E43BAB08A576291AF8F461BB2A8B3531D2F0485C19B16E2F1516E23DD3C1A4827AF1B8AC15B"
#define NIST_B571_R		"3FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE661CE18FF55987308059B186823851EC7DD9CA1161DE93D5174D66E8382E9BB2FE84E47"
#define NIST_B571_H		"2"
/** @} */
#endif

#if defined(EB_KBLTZ) && FB_POLYN == 571
/**
 * Parameters for the NIST K-571 binary elliptic curve.
 */
/** @{ */
#define NIST_K571_A		"0"
#define NIST_K571_B		"1"
#define NIST_K571_X		"26EB7A859923FBC82189631F8103FE4AC9CA2970012D5D46024804801841CA44370958493B205E647DA304DB4CEB08CBBD1BA39494776FB988B47174DCA88C7E2945283A01C8972"
#define NIST_K571_Y		"349DC807F4FBF374F4AEADE3BCA95314DD58CEC9F307A54FFC61EFC006D8A2C9D4979C0AC44AEA74FBEBBB9F772AEDCB620B01A7BA7AF1B320430C8591984F601CD4C143EF1C7A3"
#define NIST_K571_R		"20000000000000000000000000000000000000000000000000000000000000000000000131850E1F19A63E4B391A8DB917F4138B630D84BE5D639381E91DEB45CFE778F637C1001"
#define NIST_K571_H		"4"
/** @} */
#endif

#if defined(EB_SUPER) && FB_POLYN == 271
/**
 * Parameters for the MIRACL supersingular pairing-friendly elliptic curve over
 * GF(2^271).
 */
/** @{ */
#define ETAT_P271_A		"1"
#define ETAT_P271_B		"0"
#define ETAT_P271_C		"1"
#define ETAT_P271_X		"10B175C041258C778D1DD76AEF912696E510A16F0C4E5357F2F6591B401498D66271"
#define ETAT_P271_Y		"7C026D0DB856E16E70976A84C19620F8D8B92B65C2A7BAF3B9FD80DC7F385B9C26BF"
#define ETAT_P271_R		"000011325723001F4DA29DB638FB520315B3B99DAE4BC727E10745F086979F3D4FD5"
#define ETAT_P271_H		"7717D"
/** @} */
#endif

#if defined(EB_SUPER) && FB_POLYN == 271
/**
 * Parameters for the MIRACL supersingular pairing-friendly elliptic curve over
 * GF(2^271).
 */
/** @{ */
#define ETAT_T271_A		"1"
#define ETAT_T271_B		"0"
#define ETAT_T271_C		"1"
#define ETAT_T271_X		"33797D0E4348C31F6867373A566F85F720B6BDF204A9DB557CDE08CB249963C93D86"
#define ETAT_T271_Y		"3B519E11ADDE45B02AD36ED5A55F3ECD8CD9517460CAC25B187224D6BB73D9C49B1C"
#define ETAT_T271_R		"000011325723001F4DA29DB638FB520315B3B99DAE4BC727E10745F086979F3D4FD5"
#define ETAT_T271_H		"7717D"
/** @} */
#endif

#if defined(EB_SUPER) && FB_POLYN == 353
/**
 * Parameters for a supersingular pairing-friendly elliptic curve over
 * GF(2^353).
 */
/** @{ */
#define ETAT_S353_A		"1"
#define ETAT_S353_B		"1"
#define ETAT_S353_C		"1"
#if WORD == 8
#define ETAT_S353_X		"0000000000080000000000000000000000000000000000000000000000000000000000000000000000000000"
#define ETAT_S353_Y		"10C47D4C5BF5C9394CB2EA8DE924D6F0D9FD33973F27252DFB70964838DB7BC428F0BB9D8FC1240F392351DE"
#else
#define ETAT_S353_X		"0000000000000000000000000000000000000000000100000000000000000000000000000000000000000002"
#define ETAT_S353_Y		"1195A1CDA741AD3DB75BCC11B76D96F8F79AC9E2E83C8185E29A9B2DA0DA3FE4FABB88D111B4C50D56B3E5B6"
#endif
#define ETAT_S353_R		"1FFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE00000000000000000000000000000000000000000001"
#define ETAT_S353_H		"1"
/** @} */
#endif

#if defined(EB_SUPER) && FB_POLYN == 1223
/**
 * Parameters for the MIRACL supersingular pairing-friendly elliptic curve over
 * GF(2^1223).
 */
/** @{ */
#define ETAT_S1223_A	"1"
#define ETAT_S1223_B	"0"
#define ETAT_S1223_C	"1"
#define ETAT_S1223_X	"30D8B774485EC8763A0EE8E94216EF96C7C5239853E08EB5E68E81E02C8D33154C93165EB90A336E07E9B2C1C6B1A89CBD55E673F18ABFB80BD60EAFF7368DD9296C65CF6A626A1354B63665F8F7D678FD5E31E9510A32DB291CC0BAF4C44D3D69AFBCEC6E460967591DD80D37AC0AEC950E2391A0EE43A8983E2F907B3D226A0B9CAD915096B9B4EEEB95985A0E2815B71BF7C56B079396F4"
#define ETAT_S1223_Y	"0E6D5B0B3C21C6194FBAFD79ABB0E0738FBD1DE871D5D060055EBEA8166FACD7A18299F137B4A08746CAD8F896152D93B85951A40BBF9F03AD9E00B459430A8FD13AEB0EDB8AF0E67913BDFB047A9BBC9AAE61ACD5AE213059BCDAFE0B192BF535F3E8821B7FA64871CD6F66D547855B1312C1137FE6D11E11DE15EAA7EA17954C7A53BC107F9C279F53BC7D9DEC41F80C9DBD95D5DD7658CC"
#define ETAT_S1223_R	"199999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999ccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccd"
#define ETAT_S1223_H	"5"
/** @} */

#endif

/**
 * Assigns a set of ordinary elliptic curve parameters.
 *
 * @param[in] CURVE		- the curve parameters to assign.
 * @param[in] FIELD		- the finite field identifier.
 */
#define ASSIGN(CURVE, FIELD)												\
	fb_param_set(FIELD);													\
	FETCH(str, CURVE##_A, sizeof(CURVE##_A));								\
	fb_read(a, str, strlen(str), 16);										\
	FETCH(str, CURVE##_B, sizeof(CURVE##_B));								\
	fb_read(b, str, strlen(str), 16);										\
	FETCH(str, CURVE##_X, sizeof(CURVE##_X));								\
	fb_read(g->x, str, strlen(str), 16);									\
	FETCH(str, CURVE##_Y, sizeof(CURVE##_Y));								\
	fb_read(g->y, str, strlen(str), 16);									\
	FETCH(str, CURVE##_R, sizeof(CURVE##_R));								\
	bn_read_str(r, str, strlen(str), 16);									\
	FETCH(str, CURVE##_H, sizeof(CURVE##_H));								\
	bn_read_str(h, str, strlen(str), 16);									\

/**
 * Assigns a set of supersingular elliptic curve parameters.
 *
 * @param[in] CURVE		- the curve parameters to assign.
 * @param[in] FIELD		- the finite field identifier.
 */
#define ASSIGNS(CURVE, FIELD)												\
	ASSIGN(CURVE, FIELD);													\
	FETCH(str, CURVE##_C, sizeof(CURVE##_C));								\
	fb_read(c, str, strlen(str), 16);										\

/*============================================================================*/
/* Public definitions                                                         */
/*============================================================================*/

int eb_param_get() {
	return core_get()->eb_id;
}

void eb_param_set(int param) {
	int super = 0;
	int ordin = 0;
	int kbltz = 0;
	char str[2 * FB_BYTES + 1];
	fb_t a, b, c;
	eb_t g;
	bn_t r;
	bn_t h;

	fb_null(a);
	fb_null(b);
	fb_null(c);
	eb_null(g);
	bn_null(r);
	bn_null(h);

	TRY {
		fb_new(a);
		fb_new(b);
		fb_new(c);
		eb_new(g);
		bn_new(r);
		bn_new(h);

		core_get()->eb_id = 0;

		switch (param) {
#if defined(EB_ORDIN) && FB_POLYN == 163
			case NIST_B163:
				ASSIGN(NIST_B163, NIST_163);
				ordin = 1;
				break;
#endif
#if defined(EB_KBLTZ) && FB_POLYN == 163
			case NIST_K163:
				ASSIGN(NIST_K163, NIST_163);
				kbltz = 1;
				break;
#endif
#if defined(EB_ORDIN) && FB_POLYN == 233
			case NIST_B233:
				ASSIGN(NIST_B233, NIST_233);
				ordin = 1;
				break;
#endif
#if defined(EB_KBLTZ) && FB_POLYN == 233
			case NIST_K233:
				ASSIGN(NIST_K233, NIST_233);
				kbltz = 1;
				break;
#endif
#if defined(EB_ORDIN) && FB_POLYN == 251
			case EBACS_B251:
				ASSIGN(EBACS_B251, PENTA_251);
				ordin = 1;
				break;
#endif
#if defined(EB_ORDIN) && FB_POLYN == 257
			case HALVE_B257:
				ASSIGN(HALVE_B257, TRINO_257);
				ordin = 1;
				break;
#endif
#if defined(EB_ORDIN) && FB_POLYN == 283
			case NIST_B283:
				ASSIGN(NIST_B283, NIST_283);
				ordin = 1;
				break;
#endif
#if defined(EB_KBLTZ) && FB_POLYN == 283
			case NIST_K283:
				ASSIGN(NIST_K283, NIST_283);
				kbltz = 1;
				break;
#endif
#if defined(EB_KBLTZ) && FB_POLYN == 239
			case SECG_K239:
				ASSIGN(SECG_K239, SECG_239);
				kbltz = 1;
				break;
#endif
#if defined(EB_ORDIN) && FB_POLYN == 409
			case NIST_B409:
				ASSIGN(NIST_B409, NIST_409);
				ordin = 1;
				break;
#endif
#if defined(EB_KBLTZ) && FB_POLYN == 409
			case NIST_K409:
				ASSIGN(NIST_K409, NIST_409);
				kbltz = 1;
				break;
#endif
#if defined(EB_ORDIN) && FB_POLYN == 571
			case NIST_B571:
				ASSIGN(NIST_B571, NIST_571);
				ordin = 1;
				break;
#endif
#if defined(EB_KBLTZ) && FB_POLYN == 571
			case NIST_K571:
				ASSIGN(NIST_K571, NIST_571);
				kbltz = 1;
				break;
#endif
#if defined(EB_SUPER) && FB_POLYN == 271
			case ETAT_P271:
				ASSIGNS(ETAT_P271, PENTA_271);
				super = 1;
				break;
			case ETAT_T271:
				ASSIGNS(ETAT_T271, TRINO_271);
				super = 1;
				break;
#endif
#if defined(EB_SUPER) && FB_POLYN == 353
			case ETAT_S353:
				ASSIGNS(ETAT_S353, TRINO_353);
				super = 1;
				break;
#endif
#if defined(EB_SUPER) && FB_POLYN == 1223
			case ETAT_S1223:
				ASSIGNS(ETAT_S1223, TRINO_1223);
				super = 1;
				break;
#endif
			default:
				(void)str;
				THROW(ERR_NO_VALID);
				break;
		}

		/* Do not generate warnings. */
		(void)kbltz;
		(void)ordin;
		(void)super;
		(void)c;

		fb_zero(g->z);
		fb_set_bit(g->z, 0, 1);
		g->norm = 1;

#if defined(EB_ORDIN)
		if (ordin) {
			eb_curve_set_ordin(a, b, g, r, h);
			core_get()->eb_id = param;
		}
#endif

#if defined(EB_KBLTZ)
		if (kbltz) {
			eb_curve_set_kbltz(a, g, r, h);
			core_get()->eb_id = param;
		}
#elif defined(EB_ORDIN)
		if (kbltz) {
			eb_curve_set_ordin(a, b, g, r, h);
			core_get()->eb_id = param;
		}
#endif

#if defined(EB_SUPER)
		if (super) {
			eb_curve_set_super(a, b, c, g, r, h);
			core_get()->eb_id = param;
		}
#endif
	}
	CATCH_ANY {
		THROW(ERR_CAUGHT);
	}
	FINALLY {
		fb_free(a);
		fb_free(b);
		fb_free(c);
		eb_free(g);
		bn_free(r);
		bn_free(h);
	}
}

int eb_param_set_any() {
	int r0, r1, r2;

	r0 = eb_param_set_any_ordin();
	if (r0 == STS_ERR) {
		r1 = eb_param_set_any_kbltz();
		if (r1 == STS_ERR) {
			r2 = eb_param_set_any_super();
			if (r2 == STS_ERR) {
				return STS_ERR;
			}
		}
	}
	return STS_OK;
}

int eb_param_set_any_ordin() {
	int r = STS_OK;
#if defined(EB_ORDIN)
#if FB_POLYN == 163
	eb_param_set(NIST_B163);
#elif FB_POLYN == 233
	eb_param_set(NIST_B233);
#elif FB_POLYN == 251
	eb_param_set(EBACS_B251);
#elif FB_POLYN == 257
	eb_param_set(HALVE_B257);
#elif FB_POLYN == 283
	eb_param_set(NIST_B283);
#elif FB_POLYN == 409
	eb_param_set(NIST_B409);
#elif FB_POLYN == 571
	eb_param_set(NIST_B571);
#else
	r = STS_ERR;
#endif
#else
	r = STS_ERR;
#endif
	return r;
}

int eb_param_set_any_kbltz() {
	int r = STS_OK;
#if defined(EB_KBLTZ)
#if FB_POLYN == 163
	eb_param_set(NIST_K163);
#elif FB_POLYN == 233
	eb_param_set(NIST_K233);
#elif FB_POLYN == 239
	eb_param_set(SECG_K239);
#elif FB_POLYN == 283
	eb_param_set(NIST_K283);
#elif FB_POLYN == 409
	eb_param_set(NIST_K409);
#elif FB_POLYN == 571
	eb_param_set(NIST_K571);
#else
	r = STS_ERR;
#endif
#else
	r = STS_ERR;
#endif
	return r;
}

int eb_param_set_any_super() {
	int r = STS_OK;
#if defined(EB_SUPER)
#if FB_POLYN == 271
#ifdef FB_TRINO
	eb_param_set(ETAT_T271);
#else
	eb_param_set(ETAT_P271);
#endif
#elif FB_POLYN == 353
	eb_param_set(ETAT_S353);
#elif FB_POLYN == 1223
	eb_param_set(ETAT_S1223);
#else
	r = STS_ERR;
#endif
#else
	r = STS_ERR;
#endif
	return r;
}

void eb_param_print() {
	switch (core_get()->eb_id) {
		case NIST_B163:
			util_banner("Curve NIST-B163:", 0);
			break;
		case NIST_K163:
			util_banner("Curve NIST-K163:", 0);
			break;
		case NIST_B233:
			util_banner("Curve NIST-B233:", 0);
			break;
		case NIST_K233:
			util_banner("Curve NIST-K233:", 0);
			break;
		case SECG_K239:
			util_banner("Curve SECG-K239:", 0);
			break;
		case EBACS_B251:
			util_banner("Curve EBACS-B251:", 0);
			break;
		case HALVE_B257:
			util_banner("Curve HALVE-B257:", 0);
			break;
		case NIST_B283:
			util_banner("Curve NIST-B283:", 0);
			break;
		case NIST_K283:
			util_banner("Curve NIST-K283:", 0);
			break;
		case NIST_B409:
			util_banner("Curve NIST-B409:", 0);
			break;
		case NIST_K409:
			util_banner("Curve NIST-K409:", 0);
			break;
		case NIST_B571:
			util_banner("Curve NIST-B571:", 0);
			break;
		case NIST_K571:
			util_banner("Curve NIST-K571:", 0);
			break;
		case ETAT_P271:
			util_banner("Curve ETAT-P271:", 0);
			break;
		case ETAT_T271:
			util_banner("Curve ETAT-T271:", 0);
			break;
		case ETAT_S353:
			util_banner("Curve ETAT-S353:", 0);
			break;
		case ETAT_S1223:
			util_banner("Curve ETAT-S1223:", 0);
			break;
	}
}

int eb_param_level() {
	switch (core_get()->eb_id) {
		case NIST_B163:
		case NIST_K163:
			return 80;
		case NIST_B233:
		case NIST_K233:
		case SECG_K239:
			return 112;
		case EBACS_B251:
		case HALVE_B257:
		case NIST_B283:
		case NIST_K283:
			return 128;
		case NIST_B409:
		case NIST_K409:
			return 192;
		case NIST_B571:
		case NIST_K571:
			return 256;
		case ETAT_P271:
		case ETAT_T271:
			return 64;
		case ETAT_S353:
			return 80;
		case ETAT_S1223:
			return 128;
	}
	return 0;
}
