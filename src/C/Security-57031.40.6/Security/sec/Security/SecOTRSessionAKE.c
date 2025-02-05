/*
 * Copyright (c) 2011-2014 Apple Inc. All Rights Reserved.
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


#include "SecOTRSession.h"

#include "SecOTRMath.h"
#include "SecOTRIdentityPriv.h"
#include "SecOTRSessionPriv.h"
#include "SecOTRPackets.h"
#include "SecOTRPacketData.h"
#include "SecOTRDHKey.h"

#include <utilities/SecCFWrappers.h>

#include <CoreFoundation/CFRuntime.h>
#include <CoreFoundation/CFString.h>

#include <Security/SecBase.h>
#include <Security/SecRandom.h>

#include <AssertMacros.h>

#include <corecrypto/cchmac.h>
#include <corecrypto/ccsha2.h>

#include <string.h>

static void SecOTRInitMyDHKeys(SecOTRSessionRef session)
{
    CFReleaseNull(session->_myKey);
    session->_myKey = SecOTRFullDHKCreate(kCFAllocatorDefault);
    CFReleaseNull(session->_myNextKey);
    session->_myNextKey = SecOTRFullDHKCreate(kCFAllocatorDefault);
    session->_keyID = 1;
    
    bzero(session->_keyCache, sizeof(session->_keyCache));
}

OSStatus SecOTRSAppendStartPacket(SecOTRSessionRef session, CFMutableDataRef appendPacket)
{
    __block OSStatus result = errSecSuccess;

    dispatch_sync(session->_queue, ^{
        session->_state = kAwaitingDHKey;

        // Generate r and x and calculate gx:
        SecOTRInitMyDHKeys(session);

        CFMutableDataRef destinationMessage = NULL;
        if (session->_textOutput) {
            destinationMessage = CFDataCreateMutable(kCFAllocatorDefault, 0);
        } else {
            destinationMessage = CFRetainSafe(appendPacket);
        }


        result = SecRandomCopyBytes(kSecRandomDefault, sizeof(session->_r), session->_r);
        if (result == errSecSuccess) {
            SecOTRAppendDHMessage(session, destinationMessage);
            if (session->_textOutput) {
                SecOTRPrepareOutgoingBytes(destinationMessage, appendPacket);
            }
        }
        CFReleaseSafe(destinationMessage);
    });

    return result;
}

OSStatus SecOTRSAppendRestartPacket(SecOTRSessionRef session, CFMutableDataRef appendPacket)
{
    __block OSStatus result = errSecSuccess;

    dispatch_sync(session->_queue, ^{
        if (!session->_myKey) {
            secerror("_myKey is NULL, avoiding crash");
            result = errSecDecode;
            return;
        }
        CFMutableDataRef destinationMessage;
        if (session->_textOutput) {
            destinationMessage = CFDataCreateMutable(kCFAllocatorDefault, 0);
        } else {
            destinationMessage = CFRetainSafe(appendPacket);
        }

        session->_state = kAwaitingDHKey;
        CFReleaseNull(session->_receivedDHMessage);
        CFReleaseNull(session->_receivedDHKeyMessage);
        
        SecOTRAppendDHMessage(session, destinationMessage);
        if (session->_textOutput) {
            SecOTRPrepareOutgoingBytes(destinationMessage, appendPacket);
        }
        CFReleaseSafe(destinationMessage);
    });

    return result;
}

static const uint8_t* FindGXHash(CFDataRef dhPacket)
{
    const uint8_t* messageBytes = CFDataGetBytePtr(dhPacket);
    size_t remainingBytes = (size_t)CFDataGetLength(dhPacket);
    
    OTRMessageType messageType;
    
    require_noerr(ReadHeader(&messageBytes, &remainingBytes, &messageType), fail);
    require(messageType == kDHMessage, fail);
    
    uint32_t egxiLength = 0;
    require_noerr(ReadLong(&messageBytes, &remainingBytes, & egxiLength), fail);
    require(egxiLength <= remainingBytes, fail);
    messageBytes += egxiLength;
    remainingBytes -= egxiLength;
    
    uint32_t dataLength = 0;
    require_noerr(ReadLong(&messageBytes, &remainingBytes, &dataLength), fail);
    require(dataLength <= remainingBytes, fail);
    require(dataLength == CCSHA256_OUTPUT_SIZE, fail);
    
    return messageBytes;
    
fail:
    return NULL;
}

static bool SecOTRMyGXHashIsBigger(SecOTRSessionRef session, CFDataRef dhCommitMessage)
{
    bool mineIsBigger = false;

    CFMutableDataRef myDHCommitMessage = CFDataCreateMutable(kCFAllocatorDefault, 0);
    
    SecOTRAppendDHMessage(session, myDHCommitMessage);
    
    const uint8_t* myHash = FindGXHash(myDHCommitMessage);
    const uint8_t* theirHash = FindGXHash(dhCommitMessage);
    
    require(myHash, fail);
    require(theirHash, fail);
    
    mineIsBigger = 0 < memcmp(myHash, theirHash, CCSHA256_OUTPUT_SIZE);
    
fail:
    CFReleaseNull(myDHCommitMessage);
    return mineIsBigger;
}

static OSStatus SecOTRSProcessDHMessage(SecOTRSessionRef session,
                                        CFDataRef incomingPacket,
                                        CFMutableDataRef negotiationResponse)
{
    OSStatus result = errSecParam;

    switch (session->_state) {
        case kAwaitingDHKey:
            // Compare hash values.
            if (SecOTRMyGXHashIsBigger(session, incomingPacket)) {
                // If we're bigger we resend to force them to deal.
                CFReleaseNull(session->_receivedDHMessage);
                SecOTRAppendDHMessage(session, negotiationResponse);
                result = errSecSuccess;
                break;
            } // Else intentionally fall through to idle
        case kAwaitingSignature:
        case kIdle:
        case kDone:
            // Generate a new X and GX..
            SecOTRInitMyDHKeys(session);
            // If we were already waiting on reveal, then just send the packet again
        case kAwaitingRevealSignature:
            SecOTRAppendDHKeyMessage(session, negotiationResponse);

            // Keep the packet for use later.
            CFReleaseNull(session->_receivedDHMessage);
            session->_receivedDHMessage = CFDataCreateCopy(kCFAllocatorDefault, incomingPacket);
            
            session->_state = kAwaitingRevealSignature;
            result = errSecSuccess;
            break;
        default:
            result = errSecInteractionNotAllowed;
            break;
    }

    return result;
}

static OSStatus SecOTRSetupTheirKeyFrom(SecOTRSessionRef session, const uint8_t**data, size_t*size)
{
    SecOTRPublicDHKeyRef tempKey = SecOTRPublicDHKCreateFromSerialization(kCFAllocatorDefault, data, size);
    require(tempKey != NULL, fail);
    
    return SecOTRSetupInitialRemoteKey(session, tempKey);

fail:
    return errSecDecode;
}

static OSStatus SecOTRSExtractTheirPublicDHKey(SecOTRSessionRef session, CFDataRef dhPacket)
{
    OSStatus result = errSecParam;

    const uint8_t *messageBytes = CFDataGetBytePtr(dhPacket);
    size_t messageSize = (size_t)CFDataGetLength(dhPacket);
    OTRMessageType messageType = kDHMessage; // Suppress warning.
    
    ReadHeader(&messageBytes, &messageSize, &messageType);
    require(messageType == kDHKeyMessage, exit);
    
    result = SecOTRSetupTheirKeyFrom(session, &messageBytes, &messageSize);

exit:
    return result;
}


static OSStatus SecOTRSProcessDHKeyMessage(SecOTRSessionRef session,
                                        CFDataRef incomingPacket,
                                        CFMutableDataRef negotiationResponse)
{
    OSStatus result = errSecUnimplemented;
    
    result = SecOTRSExtractTheirPublicDHKey(session, incomingPacket);
    require_noerr(result, exit);

    switch (session->_state) {
        case kAwaitingDHKey:
            CFReleaseNull(session->_receivedDHKeyMessage);
            SecOTRAppendRevealSignatureMessage(session, negotiationResponse);
            session->_state = kAwaitingSignature;
            session->_receivedDHKeyMessage = CFDataCreateCopy(kCFAllocatorDefault, incomingPacket);
            result = errSecSuccess;
            break;
        case kAwaitingSignature:
            if (CFEqualSafe(incomingPacket, session->_receivedDHKeyMessage))
                SecOTRAppendRevealSignatureMessage(session, negotiationResponse);
            result = errSecSuccess;
            break;
        case kIdle:
        case kDone:
        case kAwaitingRevealSignature:
            result = errSecSuccess;
            break;
        default:
            result = errSecInteractionNotAllowed;
            break;
    }

exit:
    return result;
}


static OSStatus SecOTRSExtractR(SecOTRSessionRef session,
                                const uint8_t **messageBytes,
                                size_t *messageSize)
{
    OSStatus result = errSecDecode;

    OTRMessageType messageType = kDHMessage; // Suppress warning
    
    ReadHeader(messageBytes, messageSize, &messageType);
    require(messageType == kRevealSignatureMessage, exit);
    
    {
        uint32_t rSize = 0;
        ReadLong(messageBytes, messageSize, &rSize);
        require(rSize == kOTRAuthKeyBytes, exit);
    }
    
    memcpy(session->_r, *messageBytes, kOTRAuthKeyBytes);
    
    *messageBytes += kOTRAuthKeyBytes;
    *messageSize -= kOTRAuthKeyBytes;
    
    result = errSecSuccess;
exit:
    return result;
}

static OSStatus FindEncGYInDHPacket(SecOTRSessionRef session,
                                      const uint8_t **dhMessageBytesPtr,
                                      size_t *messageSizePtr,
                                      size_t* encGYBufferSize)
{
    OSStatus result = errSecParam;
    require_action(*encGYBufferSize >= kExponentiationBytes + 4, exit, result = errSecParam);
    
    OTRMessageType messageType;
    result = ReadHeader(dhMessageBytesPtr, messageSizePtr, &messageType);
    require_noerr(result, exit);
    require_action(messageType == kDHMessage, exit, result = errSecDecode);
    
    uint32_t readEncSize;
    result = ReadLong(dhMessageBytesPtr, messageSizePtr, &readEncSize);
    require_noerr(result, exit);

    *encGYBufferSize = readEncSize;
exit:
    // Don't bother erasing the public gy decrypted, it's public after all.
    return result;
    
}

static OSStatus SecOTRSExtractRAndTheirDHKey(SecOTRSessionRef session,
                                     const uint8_t **messageBytes,
                                     size_t *messageSize)
{
    OSStatus result = errSecDecode;
    
    require(session->_receivedDHMessage != NULL, exit);
    result = SecOTRSExtractR(session, messageBytes, messageSize);
    require_noerr(result, exit);
    
    uint8_t gxiDecrypted[kExponentiationBytes + 4];
    const uint8_t *gxiDecryptedBuffer = gxiDecrypted;
    
    const uint8_t* dhMessageBytes = CFDataGetBytePtr(session->_receivedDHMessage);
    size_t dhMessageSize = (size_t)CFDataGetLength(session->_receivedDHMessage);
    
    size_t encGYSize = sizeof(gxiDecrypted);
    result = FindEncGYInDHPacket(session, &dhMessageBytes, &dhMessageSize, &encGYSize);
    require_noerr(result, exit);
    require_action(encGYSize <= kExponentiationBytes + 4, exit, result = errSecDecode);
    
    AES_CTR_IV0_Transform(sizeof(session->_r), session->_r, encGYSize, dhMessageBytes, gxiDecrypted);

    result = SecOTRSetupTheirKeyFrom(session, &gxiDecryptedBuffer, &encGYSize);

exit:
    // Don't bother erasing the public gy decrypted, it's public after all.
    return result;
}

static OSStatus SecVerifySignatureAndMac(SecOTRSessionRef session,
                                         bool usePrimes,
                                         const uint8_t **signatureAndMacBytes,
                                         size_t *signatureAndMacSize)
{
    OSStatus result = errSecDecode;
    
    uint8_t m1[kOTRAuthMACKeyBytes];
    uint8_t m2[kOTRAuthMACKeyBytes];
    uint8_t c[kOTRAuthKeyBytes];
    
    {
        cc_unit s[kExponentiationUnits];
        
        SecPDHKeyGenerateS(session->_myKey, session->_theirKey, s);
        // Derive M1, M2 and C, either prime or normal versions.
        DeriveOTR256BitsFromS(usePrimes ? kM1Prime : kM1,
                              kExponentiationUnits, s, sizeof(m1), m1);
        DeriveOTR256BitsFromS(usePrimes ? kM2Prime : kM2,
                              kExponentiationUnits, s, sizeof(m2), m2);
        DeriveOTR128BitPairFromS(kCs,
                                 kExponentiationUnits, s,
                                 sizeof(c),usePrimes ? NULL : c,
                                 sizeof(c), usePrimes ? c : NULL);
        bzero(s, sizeof(s));
    }
    
    cchmac_di_decl(ccsha256_di(), mBContext);

    cchmac_init(ccsha256_di(), mBContext, sizeof(m1), m1);

    {
        CFMutableDataRef toHash = CFDataCreateMutable(kCFAllocatorDefault, 0);
        
        SecPDHKAppendSerialization(session->_theirKey, toHash);
        SecFDHKAppendPublicSerialization(session->_myKey, toHash);
        
        cchmac_update(ccsha256_di(), mBContext, (size_t)CFDataGetLength(toHash), CFDataGetBytePtr(toHash));
        
        CFReleaseNull(toHash);
    }
    
    const uint8_t* encSigDataBlobStart = *signatureAndMacBytes;
    
    uint32_t xbSize = 0;
    result = ReadLong(signatureAndMacBytes, signatureAndMacSize, &xbSize);
    require_noerr(result, exit);
    require_action(xbSize > 4, exit, result = errSecDecode);
    require_action(xbSize <= *signatureAndMacSize, exit, result = errSecDecode);
    
    uint8_t signatureMac[CCSHA256_OUTPUT_SIZE];
    cchmac(ccsha256_di(), sizeof(m2), m2, xbSize + 4, encSigDataBlobStart, signatureMac);
    
    require(xbSize + kSHA256HMAC160Bytes <= *signatureAndMacSize, exit);
    const uint8_t *macStart = *signatureAndMacBytes + xbSize;

    // check the outer hmac
    require_action(0 == memcmp(macStart, signatureMac, kSHA256HMAC160Bytes), exit, result = errSecDecode);
           

    {
        uint8_t xb[xbSize];
        // Decrypt and copy the signature block
        AES_CTR_IV0_Transform(sizeof(c), c, xbSize, *signatureAndMacBytes, xb);

        const uint8_t* signaturePacket = xb;
        size_t signaturePacketSize = xbSize;
        
        uint16_t pubKeyType;
        result = ReadShort(&signaturePacket, &signaturePacketSize, &pubKeyType);
        require_noerr(result, exit);
        require_action(pubKeyType == 0xF000, exit, result = errSecUnimplemented);

        uint32_t pubKeySize;
        result = ReadLong(&signaturePacket, &signaturePacketSize, &pubKeySize);
        require_noerr(result, exit);
        require_action(pubKeySize <= signaturePacketSize, exit, result = errSecDecode);
        require(((CFIndex)pubKeySize) >= 0, exit);
        
        // Add the signature and keyid to the hash.
        // PUBKEY of our type is 2 bytes of type, 2 bytes of size and size bytes.
        // Key ID is 4 bytes.
        cchmac_update(ccsha256_di(), mBContext, 2 + 4 + pubKeySize + 4, xb);
        
        uint8_t mb[CCSHA256_OUTPUT_SIZE];
        cchmac_final(ccsha256_di(), mBContext, mb);

        // Make reference to the deflated key
        require_action(SecOTRPIEqualToBytes(session->_them, signaturePacket, (CFIndex)pubKeySize), exit, result = errSecAuthFailed);

        signaturePacket += pubKeySize;
        signaturePacketSize -= pubKeySize;
       
        result = ReadLong(&signaturePacket, &signaturePacketSize, &session->_theirKeyID);
        require_noerr(result, exit);

        uint32_t sigSize;
        result = ReadLong(&signaturePacket, &signaturePacketSize, &sigSize);
        require_noerr(result, exit);
        require_action(sigSize <= signaturePacketSize, exit, result = errSecDecode);
        
        bool bresult = SecOTRPIVerifySignature(session->_them, mb, sizeof(mb), signaturePacket, sigSize, NULL);
        result = bresult ? errSecSuccess : errSecDecode;
        require_noerr(result, exit);
        
    }

exit:
    bzero(m1, sizeof(m1));
    bzero(m2, sizeof(m2));
    bzero(c, sizeof(c));
    
    return result;
}

static OSStatus SecOTRSProcessRevealSignatureMessage(SecOTRSessionRef session,
                                        CFDataRef incomingPacket,
                                        CFMutableDataRef negotiationResponse)
{
    OSStatus result = errSecParam;
    
    require_action_quiet(session->_state == kAwaitingRevealSignature, exit, result = errSecSuccess);

    const uint8_t *messageBytes = CFDataGetBytePtr(incomingPacket);
    size_t messageSize = (size_t)CFDataGetLength(incomingPacket);

    result = SecOTRSExtractRAndTheirDHKey(session, &messageBytes, &messageSize);
    require_noerr(result, exit);

    result = SecVerifySignatureAndMac(session, false, &messageBytes, &messageSize);
    require_noerr(result, exit);

    SecOTRAppendSignatureMessage(session, negotiationResponse);

    session->_state = kDone;
    result = errSecSuccess;
exit:
    return result;
}

static OSStatus SecOTRSProcessSignatureMessage(SecOTRSessionRef session,
                                        CFDataRef incomingPacket,
                                        CFMutableDataRef negotiationResponse)
{
    OSStatus result = errSecParam;

    require_action_quiet(session->_state == kAwaitingSignature, exit, result = errSecSuccess);

    const uint8_t *messageBytes = CFDataGetBytePtr(incomingPacket);
    size_t messageSize = (size_t)CFDataGetLength(incomingPacket);
    
    OTRMessageType messageType;
    result = ReadHeader(&messageBytes, &messageSize, &messageType);
    require_noerr(result, exit);
    require_action(messageType == kSignatureMessage, exit, result = errSecDecode);
    
    result = SecVerifySignatureAndMac(session, true, &messageBytes, &messageSize);
    require_noerr(result, exit);

    CFReleaseNull(session->_receivedDHKeyMessage);
    session->_state = kDone;

    result = errSecSuccess;
exit:
    return result;
}

OSStatus SecOTRSProcessPacket(SecOTRSessionRef session,
                              CFDataRef incomingPacket,
                              CFMutableDataRef negotiationResponse)
{
    __block OSStatus result = errSecParam;

    require(CFDataGetLength(incomingPacket) > 0, fail);
    dispatch_sync(session->_queue, ^{
        CFDataRef decodedBytes = SecOTRCopyIncomingBytes(incomingPacket);

        const uint8_t* bytes = CFDataGetBytePtr(decodedBytes);
        size_t size = CFDataGetLength(decodedBytes);

        OTRMessageType packetType = kInvalidMessage;
        if (ReadHeader(&bytes, &size, &packetType))
            packetType = kInvalidMessage;

        CFMutableDataRef destinationMessage;
        if (session->_textOutput) {
            destinationMessage = CFDataCreateMutable(kCFAllocatorDefault, 0);
        } else {
            destinationMessage = CFRetainSafe(negotiationResponse);
        }

        switch (packetType) {
            case kDHMessage:
                result = SecOTRSProcessDHMessage(session, decodedBytes, destinationMessage);
                break;
            case kDHKeyMessage:
                result = SecOTRSProcessDHKeyMessage(session, decodedBytes, destinationMessage);
                break;
            case kRevealSignatureMessage:
                result = SecOTRSProcessRevealSignatureMessage(session, decodedBytes, destinationMessage);
                break;
            case kSignatureMessage:
                result = SecOTRSProcessSignatureMessage(session, decodedBytes, destinationMessage);
                break;
            default:
                result = errSecDecode;
                break;
        };
        
        if (result != errSecSuccess) {
            secnotice("session", "Error %d processing packet type %d, session state %d, keyid %d, myKey %p, myNextKey %p, theirKeyId %d, theirKey %p, theirPreviousKey %p, bytes %@", (int)result, packetType, session->_state, session->_keyID, session->_myKey, session->_myNextKey, session->_theirKeyID, session->_theirKey, session->_theirPreviousKey, decodedBytes);
        }
        
        if (session->_textOutput) {
            SecOTRPrepareOutgoingBytes(destinationMessage, negotiationResponse);
        }
        CFReleaseSafe(destinationMessage);
        CFReleaseSafe(decodedBytes);
    });
    
fail:
    return result;
}
