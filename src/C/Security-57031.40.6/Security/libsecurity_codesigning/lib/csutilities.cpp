/*
 * Copyright (c) 2006-2013 Apple Inc. All Rights Reserved.
 * 
 * @APPLE_LICENSE_HEADER_START@
 * 
 * This file contains Original Code and/or Modifications of Original Code
 * as defined in and that are subject to the Apple Public Source License
 * Version 2.0 (the 'License'). You may not use this file except in
 * compliance with the License. Please obtain a copy of the License at
 * http://www.opensource.apple.com/apsl/ and read it before using this
 * file.
 * 
 * The Original Code and all software distributed under the License are
 * distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
 * EXPRESS OR IMPLIED, AND APPLE HEREBY DISCLAIMS ALL SUCH WARRANTIES,
 * INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR NON-INFRINGEMENT.
 * Please see the License for the specific language governing rights and
 * limitations under the License.
 * 
 * @APPLE_LICENSE_HEADER_END@
 */

//
// csutilities - miscellaneous utilities for the code signing implementation
//
#include "csutilities.h"
#include <Security/SecCertificatePriv.h>
#include <security_codesigning/requirement.h>
#include <security_utilities/hashing.h>
#include <security_utilities/debugging.h>
#include <security_utilities/errors.h>

namespace Security {
namespace CodeSigning {


//
// The (SHA-1) hash of the canonical Apple certificate root anchor
//
static const SHA1::Digest gAppleAnchorHash =
	{ 0x61, 0x1e, 0x5b, 0x66, 0x2c, 0x59, 0x3a, 0x08, 0xff, 0x58,
	  0xd1, 0x4a, 0xe2, 0x24, 0x52, 0xd1, 0x98, 0xdf, 0x6c, 0x60 };



//
// Test for the canonical Apple CA certificate
//
bool isAppleCA(SecCertificateRef cert)
{
	return verifyHash(cert, gAppleAnchorHash);
}

bool isAppleCA(const Hashing::Byte *sha1)
{
	return !memcmp(sha1, gAppleAnchorHash, SHA1::digestLength);
}


//
// Calculate the canonical hash of a certificate, given its raw (DER) data.
//
void hashOfCertificate(const void *certData, size_t certLength, SHA1::Digest digest)
{
	SHA1 hasher;
	hasher(certData, certLength);
	hasher.finish(digest);
}


//
// Ditto, given a SecCertificateRef
//
void hashOfCertificate(SecCertificateRef cert, SHA1::Digest digest)
{
	assert(cert);
	CSSM_DATA certData;
	MacOSError::check(SecCertificateGetData(cert, &certData));
	hashOfCertificate(certData.Data, certData.Length, digest);
}


//
// One-stop hash-certificate-and-compare
//
bool verifyHash(SecCertificateRef cert, const Hashing::Byte *digest)
{
	SHA1::Digest dig;
	hashOfCertificate(cert, dig);
	return !memcmp(dig, digest, SHA1::digestLength);
}


//
// Check to see if a certificate contains a particular field, by OID. This works for extensions,
// even ones not recognized by the local CL. It does not return any value, only presence.
//
bool certificateHasField(SecCertificateRef cert, const CSSM_OID &oid)
{
	assert(cert);
	CSSM_DATA *value;
	switch (OSStatus rc = SecCertificateCopyFirstFieldValue(cert, &oid, &value)) {
	case errSecSuccess:
		MacOSError::check(SecCertificateReleaseFirstFieldValue(cert, &oid, value));
		return true;					// extension found by oid
	case errSecUnknownTag:
		break;							// oid not recognized by CL - continue below
	default:
		MacOSError::throwMe(rc);		// error: fail
	}
	
	// check the CL's bag of unrecognized extensions
	CSSM_DATA **values;
	bool found = false;
	if (SecCertificateCopyFieldValues(cert, &CSSMOID_X509V3CertificateExtensionCStruct, &values))
		return false;	// no unrecognized extensions - no match
	if (values)
		for (CSSM_DATA **p = values; *p; p++) {
			const CSSM_X509_EXTENSION *ext = (const CSSM_X509_EXTENSION *)(*p)->Data;
			if (oid == ext->extnId) {
				found = true;
				break;
			}
		}
	MacOSError::check(SecCertificateReleaseFieldValues(cert, &CSSMOID_X509V3CertificateExtensionCStruct, values));
	return found;
}
    
    
//
// Retrieve X.509 policy extension OIDs, if any.
// This currently ignores policy qualifiers.
//
bool certificateHasPolicy(SecCertificateRef cert, const CSSM_OID &policyOid)
{
	bool matched = false;
	assert(cert);
	CSSM_DATA *data;
	if (OSStatus rc = SecCertificateCopyFirstFieldValue(cert, &CSSMOID_CertificatePolicies, &data))
		MacOSError::throwMe(rc);
	if (data && data->Data && data->Length == sizeof(CSSM_X509_EXTENSION)) {
		const CSSM_X509_EXTENSION *ext = (const CSSM_X509_EXTENSION *)data->Data;
		assert(ext->format == CSSM_X509_DATAFORMAT_PARSED);
		const CE_CertPolicies *policies = (const CE_CertPolicies *)ext->value.parsedValue;
		if (policies)
			for (unsigned int n = 0; n < policies->numPolicies; n++) {
				const CE_PolicyInformation &cp = policies->policies[n];
				if (cp.certPolicyId == policyOid) {
					matched = true;
					break;
				}
			}
	}
	SecCertificateReleaseFirstFieldValue(cert, &CSSMOID_PolicyConstraints, data);
	return matched;
}


//
// Copyfile
//
Copyfile::Copyfile()
{
	if (!(mState = copyfile_state_alloc()))
		UnixError::throwMe();
}
	
void Copyfile::set(uint32_t flag, const void *value)
{
	check(::copyfile_state_set(mState, flag, value));
}

void Copyfile::get(uint32_t flag, void *value)
{
	check(::copyfile_state_set(mState, flag, value));
}
	
void Copyfile::operator () (const char *src, const char *dst, copyfile_flags_t flags)
{
	check(::copyfile(src, dst, mState, flags));
}

void Copyfile::check(int rc)
{
	if (rc < 0)
		UnixError::throwMe();
}


//
// MessageTracer support
//
MessageTrace::MessageTrace(const char *domain, const char *signature)
{
	mAsl = asl_new(ASL_TYPE_MSG);
	if (domain)
		asl_set(mAsl, "com.apple.message.domain", domain);
	if (signature)
		asl_set(mAsl, "com.apple.message.signature", signature);
}

void MessageTrace::add(const char *key, const char *format, ...)
{
	va_list args;
	va_start(args, format);
	char value[200];
	vsnprintf(value, sizeof(value), format, args);
	va_end(args);
	asl_set(mAsl, (string("com.apple.message.") + key).c_str(), value);
}
	
void MessageTrace::send(const char *format, ...)
{
	va_list args;
	va_start(args, format);
	asl_vlog(NULL, mAsl, ASL_LEVEL_NOTICE, format, args);
	va_end(args);
}



// Resource limited async workers for doing work on nested bundles
LimitedAsync::LimitedAsync(bool async)
{
	// validate multiple resources concurrently if bundle resides on solid-state media

	// How many async workers to spin off. If zero, validating only happens synchronously.
	long async_workers = 0;

	long ncpu = sysconf(_SC_NPROCESSORS_ONLN);

	if (async && ncpu > 0)
		async_workers = ncpu - 1; // one less because this thread also validates

	mResourceSemaphore = new Dispatch::Semaphore(async_workers);
}

LimitedAsync::LimitedAsync(LimitedAsync &limitedAsync)
{
	mResourceSemaphore = new Dispatch::Semaphore(*limitedAsync.mResourceSemaphore);
}

LimitedAsync::~LimitedAsync()
{
	delete mResourceSemaphore;
}

bool LimitedAsync::perform(Dispatch::Group &groupRef, void (^block)()) {
	__block Dispatch::SemaphoreWait wait(*mResourceSemaphore, DISPATCH_TIME_NOW);

	if (wait.acquired()) {
		dispatch_queue_t defaultQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);

		groupRef.enqueue(defaultQueue, ^{
			// Hold the semaphore count until the worker is done validating.
			Dispatch::SemaphoreWait innerWait(wait);
			block();
		});
		return true;
	} else {
		block();
		return false;
	}
}

} // end namespace CodeSigning
} // end namespace Security
