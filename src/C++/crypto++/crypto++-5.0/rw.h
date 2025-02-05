#ifndef CRYPTOPP_RW_H
#define CRYPTOPP_RW_H

/** \file
	This file contains classes that implement the
	Rabin-Williams signature schemes as defined in IEEE P1363.
*/

#include "pubkey.h"
#include "integer.h"

NAMESPACE_BEGIN(CryptoPP)

const word IFSSR_R = 6;
const word IFSSA_R = 12;

//! .
template <word r>
class RWFunction : virtual public TrapdoorFunction, public PublicKey
{
	typedef RWFunction ThisClass;

public:
	void Initialize(const Integer &n)
		{m_n = n;}

	void BERDecode(BufferedTransformation &bt);
	void DEREncode(BufferedTransformation &bt) const;

	Integer ApplyFunction(const Integer &x) const;
	Integer PreimageBound() const {return m_n;}
	Integer ImageBound() const {return ++(m_n>>1);}

	bool Validate(RandomNumberGenerator &rng, unsigned int level) const;
	bool GetVoidValue(const char *name, const std::type_info &valueType, void *pValue) const;
	void AssignFrom(const NameValuePairs &source);

	const Integer& GetModulus() const {return m_n;}
	void SetModulus(const Integer &n) {m_n = n;}

protected:
	Integer m_n;
};

//! .
template <word r>
class InvertibleRWFunction : public RWFunction<r>, public TrapdoorFunctionInverse, public PrivateKey
{
	typedef InvertibleRWFunction ThisClass;

public:
	void Initialize(const Integer &n, const Integer &p, const Integer &q, const Integer &u)
		{m_n = n; m_p = p; m_q = q; m_u = u;}
	// generate a random private key
	void Initialize(RandomNumberGenerator &rng, unsigned int modulusBits)
		{GenerateRandomWithKeySize(rng, modulusBits);}

	void BERDecode(BufferedTransformation &bt);
	void DEREncode(BufferedTransformation &bt) const;

	Integer CalculateInverse(const Integer &x) const;

	// GeneratibleCryptoMaterial
	bool Validate(RandomNumberGenerator &rng, unsigned int level) const;
	bool GetVoidValue(const char *name, const std::type_info &valueType, void *pValue) const;
	void AssignFrom(const NameValuePairs &source);
	/*! parameters: (ModulusSize) */
	void GenerateRandom(RandomNumberGenerator &rng, const NameValuePairs &alg);

	const Integer& GetPrime1() const {return m_p;}
	const Integer& GetPrime2() const {return m_q;}
	const Integer& GetMultiplicativeInverseOfPrime2ModPrime1() const {return m_u;}

	void SetPrime1(const Integer &p) {m_p = p;}
	void SetPrime2(const Integer &q) {m_q = q;}
	void SetMultiplicativeInverseOfPrime2ModPrime1(const Integer &u) {m_u = u;}

protected:
	Integer m_p, m_q, m_u;
};

//! .
class EMSA2Pad : public PK_PaddingAlgorithm
{
public:
	static const char *StaticAlgorithmName() {return "EMSA2";}
	
	unsigned int MaxUnpaddedLength(unsigned int paddedLength) const {return (paddedLength+1)/8-2;}

	void Pad(RandomNumberGenerator &rng, const byte *raw, unsigned int inputLength, byte *padded, unsigned int paddedLength) const;
	DecodingResult Unpad(const byte *padded, unsigned int paddedLength, byte *raw) const;
};

//! .
template <class H>
class EMSA2DecoratedHashModule : public HashTransformationWithDefaultTruncation
{
public:
	EMSA2DecoratedHashModule() : empty(true) {}
	void Update(const byte *input, unsigned int length)
		{h.Update(input, length); empty = empty && length==0;}
	unsigned int DigestSize() const;
	void Final(byte *digest);
	void Restart() {h.Restart(); empty=true;}

private:
	H h;
	bool empty;
};

template <class H> struct EMSA2DigestDecoration
{
	static const byte decoration;
};

//! EMSA2, for use with RW
/*! The following hash functions are supported: SHA, RIPEMD160. */
struct P1363_EMSA2 : public SignatureStandard
{
	template <class H> struct SignaturePaddingAlgorithm {typedef EMSA2Pad type;};
	template <class H> struct DecoratedHashingAlgorithm {typedef EMSA2DecoratedHashModule<H> type;};
};

template<> struct CryptoStandardTraits<P1363_EMSA2> : public P1363_EMSA2 {};

// EMSA2DecoratedHashModule can be instantiated with the following two classes.
class SHA;
class RIPEMD160;

template <class H>
void EMSA2DecoratedHashModule<H>::Final(byte *digest)
{
	digest[0] = empty ? 0x4b : 0x6b;
	h.Final(digest+1);
	digest[DigestSize()-1] = EMSA2DigestDecoration<H>::decoration;
	empty=true;
}

template <class H>
unsigned int EMSA2DecoratedHashModule<H>::DigestSize() const
{
	return h.DigestSize() + 2;
}

//! .
template <word r>
struct RW
{
	static std::string StaticAlgorithmName() {return "RW";}
	typedef RWFunction<r> PublicKey;
	typedef InvertibleRWFunction<r> PrivateKey;
};

//! RW
template <class H, class STANDARD = P1363_EMSA2>
struct RWSSA : public TF_SSA<STANDARD, H, RW<IFSSA_R> >
{
};

NAMESPACE_END

#endif
