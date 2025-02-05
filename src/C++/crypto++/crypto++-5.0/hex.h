#ifndef CRYPTOPP_HEX_H
#define CRYPTOPP_HEX_H

#include "basecode.h"

NAMESPACE_BEGIN(CryptoPP)

//! Converts given data to base 16
class HexEncoder : public SimpleProxyFilter
{
public:
	HexEncoder(BufferedTransformation *attachment = NULL, bool uppercase = true, int outputGroupSize = 0, const std::string &seperator = ":", const std::string &terminator = "")
		: SimpleProxyFilter(new BaseN_Encoder(new Grouper), attachment)
	{
		IsolatedInitialize(MakeParameters("Uppercase", uppercase)("GroupSize", outputGroupSize)("Seperator", ConstByteArrayParameter(seperator)));
	}

	void IsolatedInitialize(const NameValuePairs &parameters);
};

//! Decode 16 bit data back to bytes
class HexDecoder : public BaseN_Decoder
{
public:
	HexDecoder(BufferedTransformation *attachment = NULL)
		: BaseN_Decoder(GetDecodingLookupArray(), 4, attachment) {}

	void IsolatedInitialize(const NameValuePairs &parameters) {}

private:
	static const int *GetDecodingLookupArray();
};

NAMESPACE_END

#endif
