#ifndef CRYPTOPP_ESIGN_H
#define CRYPTOPP_ESIGN_H

/** \file
	This file contains classes that implement the
	ESIGN signature schemes as defined in IEEE P1363a.
*/

#include "pubkey.h"
#include "integer.h"

NAMESPACE_BEGIN(CryptoPP)

//! .
class ESIGNFunction : public TrapdoorFunction, public PublicKey, public ASN1CryptoMaterial
{
	typedef ESIGNFunction ThisClass;

public:
	void Initialize(const Integer &n, const Integer &e)
		{m_n = n; m_e = e;}

	// PublicKey
	void BERDecode(BufferedTransformation &bt);
	void DEREncode(BufferedTransformation &bt) const;

	// CryptoMaterial
	bool Validate(RandomNumberGenerator &rng, unsigned int level) const;
	bool GetVoidValue(const char *name, const std::type_info &valueType, void *pValue) const;
	void AssignFrom(const NameValuePairs &source);

	// TrapdoorFunction
	Integer ApplyFunction(const Integer &x) const;
	Integer PreimageBound() const {return m_n;}
	Integer ImageBound() const {return Integer::Power2(GetK());}

	// non-derived
	const Integer & GetModulus() const {return m_n;}
	const Integer & GetPublicExponent() const {return m_e;}

	void SetModulus(const Integer &n) {m_n = n;}
	void SetPublicExponent(const Integer &e) {m_e = e;}

protected:
	unsigned int GetK() const {return m_n.BitCount()/3-1;}

	Integer m_n, m_e;
};

//! .
class InvertibleESIGNFunction : public ESIGNFunction, public RandomizedTrapdoorFunctionInverse, public PrivateKey
{
	typedef InvertibleESIGNFunction ThisClass;

public:
	void Initialize(const Integer &n, const Integer &e, const Integer &p, const Integer &q)
		{m_n = n; m_e = e; m_p = p; m_q = q;}
	// generate a random private key
	void Initialize(RandomNumberGenerator &rng, unsigned int modulusBits)
		{GenerateRandomWithKeySize(rng, modulusBits);}

	void BERDecode(BufferedTransformation &bt);
	void DEREncode(BufferedTransformation &bt) const;

	Integer CalculateRandomizedInverse(RandomNumberGenerator &rng, const Integer &x) const;

	// GeneratibleCryptoMaterial
	bool Validate(RandomNumberGenerator &rng, unsigned int level) const;
	bool GetVoidValue(const char *name, const std::type_info &valueType, void *pValue) const;
	void AssignFrom(const NameValuePairs &source);
	/*! parameters: (ModulusSize) */
	void GenerateRandom(RandomNumberGenerator &rng, const NameValuePairs &alg);

	const Integer& GetPrime1() const {return m_p;}
	const Integer& GetPrime2() const {return m_q;}

	void SetPrime1(const Integer &p) {m_p = p;}
	void SetPrime2(const Integer &q) {m_q = q;}

protected:
	Integer m_p, m_q;
};

//! .
template <class T>
class EMSA5Pad : public PK_NonreversiblePaddingAlgorithm
{
public:
	static const char *StaticAlgorithmName() {return "EMSA5";}
	
	unsigned int MaxUnpaddedLength(unsigned int paddedLength) const {return UINT_MAX;}

	void Pad(RandomNumberGenerator &rng, const byte *raw, unsigned int inputLength, byte *padded, unsigned int paddedLength) const
	{
		unsigned int paddedByteLength = BitsToBytes(paddedLength);
		memset(padded, 0, paddedByteLength);
		T::GenerateAndMask(padded, paddedByteLength, raw, inputLength);
		if (paddedLength % 8 != 0)
			padded[0] = (byte)Crop(padded[0], paddedLength % 8);
	}
};

//! EMSA5, for use with ESIGN
struct P1363_EMSA5 : public SignatureStandard
{
	template <class H> struct SignaturePaddingAlgorithm {typedef EMSA5Pad<P1363_MGF1<H> > type;};
	template <class H> struct DecoratedHashingAlgorithm {typedef H type;};
};

template<> struct CryptoStandardTraits<P1363_EMSA5> : public P1363_EMSA5 {};

struct ESIGN_Keys
{
	static std::string StaticAlgorithmName() {return "ESIGN";}
	typedef ESIGNFunction PublicKey;
	typedef InvertibleESIGNFunction PrivateKey;
};

//! ESIGN, as defined in IEEE P1363a
template <class H, class STANDARD = P1363_EMSA5>
struct ESIGN : public TF_SSA<STANDARD, H, ESIGN_Keys>
{
};

NAMESPACE_END

#endif
