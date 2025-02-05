// Copyright 2009 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

package main

import (
	"bytes"
	"crypto"
	"crypto/ecdsa"
	"crypto/elliptic"
	"crypto/rsa"
	"crypto/subtle"
	"crypto/x509"
	"encoding/asn1"
	"errors"
	"fmt"
	"io"
	"math/big"
)

// serverHandshakeState contains details of a server handshake in progress.
// It's discarded once the handshake has completed.
type serverHandshakeState struct {
	c               *Conn
	clientHello     *clientHelloMsg
	hello           *serverHelloMsg
	suite           *cipherSuite
	ellipticOk      bool
	ecdsaOk         bool
	sessionState    *sessionState
	finishedHash    finishedHash
	masterSecret    []byte
	certsFromClient [][]byte
	cert            *Certificate
	finishedBytes   []byte
}

// serverHandshake performs a TLS handshake as a server.
func (c *Conn) serverHandshake() error {
	config := c.config

	// If this is the first server handshake, we generate a random key to
	// encrypt the tickets with.
	config.serverInitOnce.Do(config.serverInit)

	c.sendHandshakeSeq = 0
	c.recvHandshakeSeq = 0

	hs := serverHandshakeState{
		c: c,
	}
	isResume, err := hs.readClientHello()
	if err != nil {
		return err
	}

	// For an overview of TLS handshaking, see https://tools.ietf.org/html/rfc5246#section-7.3
	if isResume {
		// The client has included a session ticket and so we do an abbreviated handshake.
		if err := hs.doResumeHandshake(); err != nil {
			return err
		}
		if err := hs.establishKeys(); err != nil {
			return err
		}
		if c.config.Bugs.RenewTicketOnResume {
			if err := hs.sendSessionTicket(); err != nil {
				return err
			}
		}
		if err := hs.sendFinished(); err != nil {
			return err
		}
		// Most retransmits are triggered by a timeout, but the final
		// leg of the handshake is retransmited upon re-receiving a
		// Finished.
		if err := c.simulatePacketLoss(func() {
			c.writeRecord(recordTypeHandshake, hs.finishedBytes)
			c.dtlsFlushHandshake()
		}); err != nil {
			return err
		}
		if err := hs.readFinished(isResume); err != nil {
			return err
		}
		c.didResume = true
	} else {
		// The client didn't include a session ticket, or it wasn't
		// valid so we do a full handshake.
		if err := hs.doFullHandshake(); err != nil {
			return err
		}
		if err := hs.establishKeys(); err != nil {
			return err
		}
		if err := hs.readFinished(isResume); err != nil {
			return err
		}
		if c.config.Bugs.ExpectFalseStart {
			if err := c.readRecord(recordTypeApplicationData); err != nil {
				return err
			}
		}
		if err := hs.sendSessionTicket(); err != nil {
			return err
		}
		if err := hs.sendFinished(); err != nil {
			return err
		}
	}
	c.handshakeComplete = true

	return nil
}

// readClientHello reads a ClientHello message from the client and decides
// whether we will perform session resumption.
func (hs *serverHandshakeState) readClientHello() (isResume bool, err error) {
	config := hs.c.config
	c := hs.c

	if err := c.simulatePacketLoss(nil); err != nil {
		return false, err
	}
	msg, err := c.readHandshake()
	if err != nil {
		return false, err
	}
	var ok bool
	hs.clientHello, ok = msg.(*clientHelloMsg)
	if !ok {
		c.sendAlert(alertUnexpectedMessage)
		return false, unexpectedMessageError(hs.clientHello, msg)
	}
	if config.Bugs.RequireFastradioPadding && len(hs.clientHello.raw) < 1000 {
		return false, errors.New("tls: ClientHello record size should be larger than 1000 bytes when padding enabled.")
	}

	if c.isDTLS && !config.Bugs.SkipHelloVerifyRequest {
		// Per RFC 6347, the version field in HelloVerifyRequest SHOULD
		// be always DTLS 1.0
		helloVerifyRequest := &helloVerifyRequestMsg{
			vers:   VersionTLS10,
			cookie: make([]byte, 32),
		}
		if _, err := io.ReadFull(c.config.rand(), helloVerifyRequest.cookie); err != nil {
			c.sendAlert(alertInternalError)
			return false, errors.New("dtls: short read from Rand: " + err.Error())
		}
		c.writeRecord(recordTypeHandshake, helloVerifyRequest.marshal())
		c.dtlsFlushHandshake()

		if err := c.simulatePacketLoss(nil); err != nil {
			return false, err
		}
		msg, err := c.readHandshake()
		if err != nil {
			return false, err
		}
		newClientHello, ok := msg.(*clientHelloMsg)
		if !ok {
			c.sendAlert(alertUnexpectedMessage)
			return false, unexpectedMessageError(hs.clientHello, msg)
		}
		if !bytes.Equal(newClientHello.cookie, helloVerifyRequest.cookie) {
			return false, errors.New("dtls: invalid cookie")
		}

		// Apart from the cookie, the two ClientHellos must
		// match. Note that clientHello.equal compares the
		// serialization, so we make a copy.
		oldClientHelloCopy := *hs.clientHello
		oldClientHelloCopy.raw = nil
		oldClientHelloCopy.cookie = nil
		newClientHelloCopy := *newClientHello
		newClientHelloCopy.raw = nil
		newClientHelloCopy.cookie = nil
		if !oldClientHelloCopy.equal(&newClientHelloCopy) {
			return false, errors.New("dtls: retransmitted ClientHello does not match")
		}
		hs.clientHello = newClientHello
	}

	if config.Bugs.RequireSameRenegoClientVersion && c.clientVersion != 0 {
		if c.clientVersion != hs.clientHello.vers {
			return false, fmt.Errorf("tls: client offered different version on renego")
		}
	}
	c.clientVersion = hs.clientHello.vers

	// Reject < 1.2 ClientHellos with signature_algorithms.
	if c.clientVersion < VersionTLS12 && len(hs.clientHello.signatureAndHashes) > 0 {
		return false, fmt.Errorf("tls: client included signature_algorithms before TLS 1.2")
	}

	c.vers, ok = config.mutualVersion(hs.clientHello.vers)
	if !ok {
		c.sendAlert(alertProtocolVersion)
		return false, fmt.Errorf("tls: client offered an unsupported, maximum protocol version of %x", hs.clientHello.vers)
	}
	c.haveVers = true

	hs.hello = new(serverHelloMsg)
	hs.hello.isDTLS = c.isDTLS

	supportedCurve := false
	preferredCurves := config.curvePreferences()
Curves:
	for _, curve := range hs.clientHello.supportedCurves {
		for _, supported := range preferredCurves {
			if supported == curve {
				supportedCurve = true
				break Curves
			}
		}
	}

	supportedPointFormat := false
	for _, pointFormat := range hs.clientHello.supportedPoints {
		if pointFormat == pointFormatUncompressed {
			supportedPointFormat = true
			break
		}
	}
	hs.ellipticOk = supportedCurve && supportedPointFormat

	foundCompression := false
	// We only support null compression, so check that the client offered it.
	for _, compression := range hs.clientHello.compressionMethods {
		if compression == compressionNone {
			foundCompression = true
			break
		}
	}

	if !foundCompression {
		c.sendAlert(alertHandshakeFailure)
		return false, errors.New("tls: client does not support uncompressed connections")
	}

	hs.hello.vers = c.vers
	hs.hello.random = make([]byte, 32)
	_, err = io.ReadFull(config.rand(), hs.hello.random)
	if err != nil {
		c.sendAlert(alertInternalError)
		return false, err
	}

	if !bytes.Equal(c.clientVerify, hs.clientHello.secureRenegotiation) {
		c.sendAlert(alertHandshakeFailure)
		return false, errors.New("tls: renegotiation mismatch")
	}

	if len(c.clientVerify) > 0 && !c.config.Bugs.EmptyRenegotiationInfo {
		hs.hello.secureRenegotiation = append(hs.hello.secureRenegotiation, c.clientVerify...)
		hs.hello.secureRenegotiation = append(hs.hello.secureRenegotiation, c.serverVerify...)
		if c.config.Bugs.BadRenegotiationInfo {
			hs.hello.secureRenegotiation[0] ^= 0x80
		}
	} else {
		hs.hello.secureRenegotiation = hs.clientHello.secureRenegotiation
	}

	hs.hello.compressionMethod = compressionNone
	hs.hello.duplicateExtension = c.config.Bugs.DuplicateExtension
	if len(hs.clientHello.serverName) > 0 {
		c.serverName = hs.clientHello.serverName
	}

	if len(hs.clientHello.alpnProtocols) > 0 {
		if selectedProto, fallback := mutualProtocol(hs.clientHello.alpnProtocols, c.config.NextProtos); !fallback {
			hs.hello.alpnProtocol = selectedProto
			c.clientProtocol = selectedProto
			c.usedALPN = true
		}
	} else {
		// Although sending an empty NPN extension is reasonable, Firefox has
		// had a bug around this. Best to send nothing at all if
		// config.NextProtos is empty. See
		// https://code.google.com/p/go/issues/detail?id=5445.
		if hs.clientHello.nextProtoNeg && len(config.NextProtos) > 0 {
			hs.hello.nextProtoNeg = true
			hs.hello.nextProtos = config.NextProtos
		}
	}
	hs.hello.extendedMasterSecret = c.vers >= VersionTLS10 && hs.clientHello.extendedMasterSecret && !c.config.Bugs.NoExtendedMasterSecret

	if len(config.Certificates) == 0 {
		c.sendAlert(alertInternalError)
		return false, errors.New("tls: no certificates configured")
	}
	hs.cert = &config.Certificates[0]
	if len(hs.clientHello.serverName) > 0 {
		hs.cert = config.getCertificateForName(hs.clientHello.serverName)
	}
	if expected := c.config.Bugs.ExpectServerName; expected != "" && expected != hs.clientHello.serverName {
		return false, errors.New("tls: unexpected server name")
	}

	if hs.clientHello.channelIDSupported && config.RequestChannelID {
		hs.hello.channelIDRequested = true
	}

	if hs.clientHello.srtpProtectionProfiles != nil {
	SRTPLoop:
		for _, p1 := range c.config.SRTPProtectionProfiles {
			for _, p2 := range hs.clientHello.srtpProtectionProfiles {
				if p1 == p2 {
					hs.hello.srtpProtectionProfile = p1
					c.srtpProtectionProfile = p1
					break SRTPLoop
				}
			}
		}
	}

	if c.config.Bugs.SendSRTPProtectionProfile != 0 {
		hs.hello.srtpProtectionProfile = c.config.Bugs.SendSRTPProtectionProfile
	}

	_, hs.ecdsaOk = hs.cert.PrivateKey.(*ecdsa.PrivateKey)

	if hs.checkForResumption() {
		return true, nil
	}

	var scsvFound bool

	for _, cipherSuite := range hs.clientHello.cipherSuites {
		if cipherSuite == fallbackSCSV {
			scsvFound = true
			break
		}
	}

	if !scsvFound && config.Bugs.FailIfNotFallbackSCSV {
		return false, errors.New("tls: no fallback SCSV found when expected")
	} else if scsvFound && !config.Bugs.FailIfNotFallbackSCSV {
		return false, errors.New("tls: fallback SCSV found when not expected")
	}

	var preferenceList, supportedList []uint16
	if c.config.PreferServerCipherSuites {
		preferenceList = c.config.cipherSuites()
		supportedList = hs.clientHello.cipherSuites
	} else {
		preferenceList = hs.clientHello.cipherSuites
		supportedList = c.config.cipherSuites()
	}

	for _, id := range preferenceList {
		if hs.suite = c.tryCipherSuite(id, supportedList, c.vers, hs.ellipticOk, hs.ecdsaOk); hs.suite != nil {
			break
		}
	}

	if hs.suite == nil {
		c.sendAlert(alertHandshakeFailure)
		return false, errors.New("tls: no cipher suite supported by both client and server")
	}

	return false, nil
}

// checkForResumption returns true if we should perform resumption on this connection.
func (hs *serverHandshakeState) checkForResumption() bool {
	c := hs.c

	if len(hs.clientHello.sessionTicket) > 0 {
		if c.config.SessionTicketsDisabled {
			return false
		}

		var ok bool
		if hs.sessionState, ok = c.decryptTicket(hs.clientHello.sessionTicket); !ok {
			return false
		}
	} else {
		if c.config.ServerSessionCache == nil {
			return false
		}

		var ok bool
		sessionId := string(hs.clientHello.sessionId)
		if hs.sessionState, ok = c.config.ServerSessionCache.Get(sessionId); !ok {
			return false
		}
	}

	// Never resume a session for a different SSL version.
	if !c.config.Bugs.AllowSessionVersionMismatch && c.vers != hs.sessionState.vers {
		return false
	}

	cipherSuiteOk := false
	// Check that the client is still offering the ciphersuite in the session.
	for _, id := range hs.clientHello.cipherSuites {
		if id == hs.sessionState.cipherSuite {
			cipherSuiteOk = true
			break
		}
	}
	if !cipherSuiteOk {
		return false
	}

	// Check that we also support the ciphersuite from the session.
	hs.suite = c.tryCipherSuite(hs.sessionState.cipherSuite, c.config.cipherSuites(), hs.sessionState.vers, hs.ellipticOk, hs.ecdsaOk)
	if hs.suite == nil {
		return false
	}

	sessionHasClientCerts := len(hs.sessionState.certificates) != 0
	needClientCerts := c.config.ClientAuth == RequireAnyClientCert || c.config.ClientAuth == RequireAndVerifyClientCert
	if needClientCerts && !sessionHasClientCerts {
		return false
	}
	if sessionHasClientCerts && c.config.ClientAuth == NoClientCert {
		return false
	}

	return true
}

func (hs *serverHandshakeState) doResumeHandshake() error {
	c := hs.c

	hs.hello.cipherSuite = hs.suite.id
	// We echo the client's session ID in the ServerHello to let it know
	// that we're doing a resumption.
	hs.hello.sessionId = hs.clientHello.sessionId
	hs.hello.ticketSupported = c.config.Bugs.RenewTicketOnResume

	hs.finishedHash = newFinishedHash(c.vers, hs.suite)
	hs.finishedHash.discardHandshakeBuffer()
	hs.writeClientHash(hs.clientHello.marshal())
	hs.writeServerHash(hs.hello.marshal())

	c.writeRecord(recordTypeHandshake, hs.hello.marshal())

	if len(hs.sessionState.certificates) > 0 {
		if _, err := hs.processCertsFromClient(hs.sessionState.certificates); err != nil {
			return err
		}
	}

	hs.masterSecret = hs.sessionState.masterSecret
	c.extendedMasterSecret = hs.sessionState.extendedMasterSecret

	return nil
}

func (hs *serverHandshakeState) doFullHandshake() error {
	config := hs.c.config
	c := hs.c

	isPSK := hs.suite.flags&suitePSK != 0
	if !isPSK && hs.clientHello.ocspStapling && len(hs.cert.OCSPStaple) > 0 {
		hs.hello.ocspStapling = true
	}

	if hs.clientHello.sctListSupported && len(hs.cert.SignedCertificateTimestampList) > 0 {
		hs.hello.sctList = hs.cert.SignedCertificateTimestampList
	}

	hs.hello.ticketSupported = hs.clientHello.ticketSupported && !config.SessionTicketsDisabled && c.vers > VersionSSL30
	hs.hello.cipherSuite = hs.suite.id
	if config.Bugs.SendCipherSuite != 0 {
		hs.hello.cipherSuite = config.Bugs.SendCipherSuite
	}
	c.extendedMasterSecret = hs.hello.extendedMasterSecret

	// Generate a session ID if we're to save the session.
	if !hs.hello.ticketSupported && config.ServerSessionCache != nil {
		hs.hello.sessionId = make([]byte, 32)
		if _, err := io.ReadFull(config.rand(), hs.hello.sessionId); err != nil {
			c.sendAlert(alertInternalError)
			return errors.New("tls: short read from Rand: " + err.Error())
		}
	}

	hs.finishedHash = newFinishedHash(c.vers, hs.suite)
	hs.writeClientHash(hs.clientHello.marshal())
	hs.writeServerHash(hs.hello.marshal())

	c.writeRecord(recordTypeHandshake, hs.hello.marshal())

	if !isPSK {
		certMsg := new(certificateMsg)
		certMsg.certificates = hs.cert.Certificate
		if !config.Bugs.UnauthenticatedECDH {
			certMsgBytes := certMsg.marshal()
			if config.Bugs.WrongCertificateMessageType {
				certMsgBytes[0] += 42
			}
			hs.writeServerHash(certMsgBytes)
			c.writeRecord(recordTypeHandshake, certMsgBytes)
		}
	}

	if hs.hello.ocspStapling {
		certStatus := new(certificateStatusMsg)
		certStatus.statusType = statusTypeOCSP
		certStatus.response = hs.cert.OCSPStaple
		hs.writeServerHash(certStatus.marshal())
		c.writeRecord(recordTypeHandshake, certStatus.marshal())
	}

	keyAgreement := hs.suite.ka(c.vers)
	skx, err := keyAgreement.generateServerKeyExchange(config, hs.cert, hs.clientHello, hs.hello)
	if err != nil {
		c.sendAlert(alertHandshakeFailure)
		return err
	}
	if skx != nil && !config.Bugs.SkipServerKeyExchange {
		hs.writeServerHash(skx.marshal())
		c.writeRecord(recordTypeHandshake, skx.marshal())
	}

	if config.ClientAuth >= RequestClientCert {
		// Request a client certificate
		certReq := &certificateRequestMsg{
			certificateTypes: config.ClientCertificateTypes,
		}
		if certReq.certificateTypes == nil {
			certReq.certificateTypes = []byte{
				byte(CertTypeRSASign),
				byte(CertTypeECDSASign),
			}
		}
		if c.vers >= VersionTLS12 {
			certReq.hasSignatureAndHash = true
			if !config.Bugs.NoSignatureAndHashes {
				certReq.signatureAndHashes = config.signatureAndHashesForServer()
			}
		}

		// An empty list of certificateAuthorities signals to
		// the client that it may send any certificate in response
		// to our request. When we know the CAs we trust, then
		// we can send them down, so that the client can choose
		// an appropriate certificate to give to us.
		if config.ClientCAs != nil {
			certReq.certificateAuthorities = config.ClientCAs.Subjects()
		}
		hs.writeServerHash(certReq.marshal())
		c.writeRecord(recordTypeHandshake, certReq.marshal())
	}

	helloDone := new(serverHelloDoneMsg)
	hs.writeServerHash(helloDone.marshal())
	c.writeRecord(recordTypeHandshake, helloDone.marshal())
	c.dtlsFlushHandshake()

	var pub crypto.PublicKey // public key for client auth, if any

	if err := c.simulatePacketLoss(nil); err != nil {
		return err
	}
	msg, err := c.readHandshake()
	if err != nil {
		return err
	}

	var ok bool
	// If we requested a client certificate, then the client must send a
	// certificate message, even if it's empty.
	if config.ClientAuth >= RequestClientCert {
		var certMsg *certificateMsg
		if certMsg, ok = msg.(*certificateMsg); !ok {
			c.sendAlert(alertUnexpectedMessage)
			return unexpectedMessageError(certMsg, msg)
		}
		hs.writeClientHash(certMsg.marshal())

		if len(certMsg.certificates) == 0 {
			// The client didn't actually send a certificate
			switch config.ClientAuth {
			case RequireAnyClientCert, RequireAndVerifyClientCert:
				c.sendAlert(alertBadCertificate)
				return errors.New("tls: client didn't provide a certificate")
			}
		}

		pub, err = hs.processCertsFromClient(certMsg.certificates)
		if err != nil {
			return err
		}

		msg, err = c.readHandshake()
		if err != nil {
			return err
		}
	}

	// Get client key exchange
	ckx, ok := msg.(*clientKeyExchangeMsg)
	if !ok {
		c.sendAlert(alertUnexpectedMessage)
		return unexpectedMessageError(ckx, msg)
	}
	hs.writeClientHash(ckx.marshal())

	preMasterSecret, err := keyAgreement.processClientKeyExchange(config, hs.cert, ckx, c.vers)
	if err != nil {
		c.sendAlert(alertHandshakeFailure)
		return err
	}
	if c.extendedMasterSecret {
		hs.masterSecret = extendedMasterFromPreMasterSecret(c.vers, hs.suite, preMasterSecret, hs.finishedHash)
	} else {
		if c.config.Bugs.RequireExtendedMasterSecret {
			return errors.New("tls: extended master secret required but not supported by peer")
		}
		hs.masterSecret = masterFromPreMasterSecret(c.vers, hs.suite, preMasterSecret, hs.clientHello.random, hs.hello.random)
	}

	// If we received a client cert in response to our certificate request message,
	// the client will send us a certificateVerifyMsg immediately after the
	// clientKeyExchangeMsg.  This message is a digest of all preceding
	// handshake-layer messages that is signed using the private key corresponding
	// to the client's certificate. This allows us to verify that the client is in
	// possession of the private key of the certificate.
	if len(c.peerCertificates) > 0 {
		msg, err = c.readHandshake()
		if err != nil {
			return err
		}
		certVerify, ok := msg.(*certificateVerifyMsg)
		if !ok {
			c.sendAlert(alertUnexpectedMessage)
			return unexpectedMessageError(certVerify, msg)
		}

		// Determine the signature type.
		var signatureAndHash signatureAndHash
		if certVerify.hasSignatureAndHash {
			signatureAndHash = certVerify.signatureAndHash
			if !isSupportedSignatureAndHash(signatureAndHash, config.signatureAndHashesForServer()) {
				return errors.New("tls: unsupported hash function for client certificate")
			}
		} else {
			// Before TLS 1.2 the signature algorithm was implicit
			// from the key type, and only one hash per signature
			// algorithm was possible. Leave the hash as zero.
			switch pub.(type) {
			case *ecdsa.PublicKey:
				signatureAndHash.signature = signatureECDSA
			case *rsa.PublicKey:
				signatureAndHash.signature = signatureRSA
			}
		}

		switch key := pub.(type) {
		case *ecdsa.PublicKey:
			if signatureAndHash.signature != signatureECDSA {
				err = errors.New("tls: bad signature type for client's ECDSA certificate")
				break
			}
			ecdsaSig := new(ecdsaSignature)
			if _, err = asn1.Unmarshal(certVerify.signature, ecdsaSig); err != nil {
				break
			}
			if ecdsaSig.R.Sign() <= 0 || ecdsaSig.S.Sign() <= 0 {
				err = errors.New("ECDSA signature contained zero or negative values")
				break
			}
			var digest []byte
			digest, _, err = hs.finishedHash.hashForClientCertificate(signatureAndHash, hs.masterSecret)
			if err != nil {
				break
			}
			if !ecdsa.Verify(key, digest, ecdsaSig.R, ecdsaSig.S) {
				err = errors.New("ECDSA verification failure")
				break
			}
		case *rsa.PublicKey:
			if signatureAndHash.signature != signatureRSA {
				err = errors.New("tls: bad signature type for client's RSA certificate")
				break
			}
			var digest []byte
			var hashFunc crypto.Hash
			digest, hashFunc, err = hs.finishedHash.hashForClientCertificate(signatureAndHash, hs.masterSecret)
			if err != nil {
				break
			}
			err = rsa.VerifyPKCS1v15(key, hashFunc, digest, certVerify.signature)
		}
		if err != nil {
			c.sendAlert(alertBadCertificate)
			return errors.New("could not validate signature of connection nonces: " + err.Error())
		}

		hs.writeClientHash(certVerify.marshal())
	}

	hs.finishedHash.discardHandshakeBuffer()

	return nil
}

func (hs *serverHandshakeState) establishKeys() error {
	c := hs.c

	clientMAC, serverMAC, clientKey, serverKey, clientIV, serverIV :=
		keysFromMasterSecret(c.vers, hs.suite, hs.masterSecret, hs.clientHello.random, hs.hello.random, hs.suite.macLen, hs.suite.keyLen, hs.suite.ivLen)

	var clientCipher, serverCipher interface{}
	var clientHash, serverHash macFunction

	if hs.suite.aead == nil {
		clientCipher = hs.suite.cipher(clientKey, clientIV, true /* for reading */)
		clientHash = hs.suite.mac(c.vers, clientMAC)
		serverCipher = hs.suite.cipher(serverKey, serverIV, false /* not for reading */)
		serverHash = hs.suite.mac(c.vers, serverMAC)
	} else {
		clientCipher = hs.suite.aead(clientKey, clientIV)
		serverCipher = hs.suite.aead(serverKey, serverIV)
	}

	c.in.prepareCipherSpec(c.vers, clientCipher, clientHash)
	c.out.prepareCipherSpec(c.vers, serverCipher, serverHash)

	return nil
}

func (hs *serverHandshakeState) readFinished(isResume bool) error {
	c := hs.c

	c.readRecord(recordTypeChangeCipherSpec)
	if err := c.in.error(); err != nil {
		return err
	}

	if hs.hello.nextProtoNeg {
		msg, err := c.readHandshake()
		if err != nil {
			return err
		}
		nextProto, ok := msg.(*nextProtoMsg)
		if !ok {
			c.sendAlert(alertUnexpectedMessage)
			return unexpectedMessageError(nextProto, msg)
		}
		hs.writeClientHash(nextProto.marshal())
		c.clientProtocol = nextProto.proto
	}

	if hs.hello.channelIDRequested {
		msg, err := c.readHandshake()
		if err != nil {
			return err
		}
		encryptedExtensions, ok := msg.(*encryptedExtensionsMsg)
		if !ok {
			c.sendAlert(alertUnexpectedMessage)
			return unexpectedMessageError(encryptedExtensions, msg)
		}
		x := new(big.Int).SetBytes(encryptedExtensions.channelID[0:32])
		y := new(big.Int).SetBytes(encryptedExtensions.channelID[32:64])
		r := new(big.Int).SetBytes(encryptedExtensions.channelID[64:96])
		s := new(big.Int).SetBytes(encryptedExtensions.channelID[96:128])
		if !elliptic.P256().IsOnCurve(x, y) {
			return errors.New("tls: invalid channel ID public key")
		}
		channelID := &ecdsa.PublicKey{elliptic.P256(), x, y}
		var resumeHash []byte
		if isResume {
			resumeHash = hs.sessionState.handshakeHash
		}
		if !ecdsa.Verify(channelID, hs.finishedHash.hashForChannelID(resumeHash), r, s) {
			return errors.New("tls: invalid channel ID signature")
		}
		c.channelID = channelID

		hs.writeClientHash(encryptedExtensions.marshal())
	}

	msg, err := c.readHandshake()
	if err != nil {
		return err
	}
	clientFinished, ok := msg.(*finishedMsg)
	if !ok {
		c.sendAlert(alertUnexpectedMessage)
		return unexpectedMessageError(clientFinished, msg)
	}

	verify := hs.finishedHash.clientSum(hs.masterSecret)
	if len(verify) != len(clientFinished.verifyData) ||
		subtle.ConstantTimeCompare(verify, clientFinished.verifyData) != 1 {
		c.sendAlert(alertHandshakeFailure)
		return errors.New("tls: client's Finished message is incorrect")
	}
	c.clientVerify = append(c.clientVerify[:0], clientFinished.verifyData...)

	hs.writeClientHash(clientFinished.marshal())
	return nil
}

func (hs *serverHandshakeState) sendSessionTicket() error {
	c := hs.c
	state := sessionState{
		vers:          c.vers,
		cipherSuite:   hs.suite.id,
		masterSecret:  hs.masterSecret,
		certificates:  hs.certsFromClient,
		handshakeHash: hs.finishedHash.server.Sum(nil),
	}

	if !hs.hello.ticketSupported || hs.c.config.Bugs.SkipNewSessionTicket {
		if c.config.ServerSessionCache != nil && len(hs.hello.sessionId) != 0 {
			c.config.ServerSessionCache.Put(string(hs.hello.sessionId), &state)
		}
		return nil
	}

	m := new(newSessionTicketMsg)

	var err error
	m.ticket, err = c.encryptTicket(&state)
	if err != nil {
		return err
	}

	hs.writeServerHash(m.marshal())
	c.writeRecord(recordTypeHandshake, m.marshal())

	return nil
}

func (hs *serverHandshakeState) sendFinished() error {
	c := hs.c

	finished := new(finishedMsg)
	finished.verifyData = hs.finishedHash.serverSum(hs.masterSecret)
	c.serverVerify = append(c.serverVerify[:0], finished.verifyData...)
	hs.finishedBytes = finished.marshal()
	hs.writeServerHash(hs.finishedBytes)
	postCCSBytes := hs.finishedBytes

	if c.config.Bugs.FragmentAcrossChangeCipherSpec {
		c.writeRecord(recordTypeHandshake, postCCSBytes[:5])
		postCCSBytes = postCCSBytes[5:]
	}
	c.dtlsFlushHandshake()

	if !c.config.Bugs.SkipChangeCipherSpec {
		c.writeRecord(recordTypeChangeCipherSpec, []byte{1})
	}

	if c.config.Bugs.AppDataAfterChangeCipherSpec != nil {
		c.writeRecord(recordTypeApplicationData, c.config.Bugs.AppDataAfterChangeCipherSpec)
	}
	if c.config.Bugs.AlertAfterChangeCipherSpec != 0 {
		c.sendAlert(c.config.Bugs.AlertAfterChangeCipherSpec)
		return errors.New("tls: simulating post-CCS alert")
	}

	if !c.config.Bugs.SkipFinished {
		c.writeRecord(recordTypeHandshake, postCCSBytes)
		c.dtlsFlushHandshake()
	}

	c.cipherSuite = hs.suite.id

	return nil
}

// processCertsFromClient takes a chain of client certificates either from a
// Certificates message or from a sessionState and verifies them. It returns
// the public key of the leaf certificate.
func (hs *serverHandshakeState) processCertsFromClient(certificates [][]byte) (crypto.PublicKey, error) {
	c := hs.c

	hs.certsFromClient = certificates
	certs := make([]*x509.Certificate, len(certificates))
	var err error
	for i, asn1Data := range certificates {
		if certs[i], err = x509.ParseCertificate(asn1Data); err != nil {
			c.sendAlert(alertBadCertificate)
			return nil, errors.New("tls: failed to parse client certificate: " + err.Error())
		}
	}

	if c.config.ClientAuth >= VerifyClientCertIfGiven && len(certs) > 0 {
		opts := x509.VerifyOptions{
			Roots:         c.config.ClientCAs,
			CurrentTime:   c.config.time(),
			Intermediates: x509.NewCertPool(),
			KeyUsages:     []x509.ExtKeyUsage{x509.ExtKeyUsageClientAuth},
		}

		for _, cert := range certs[1:] {
			opts.Intermediates.AddCert(cert)
		}

		chains, err := certs[0].Verify(opts)
		if err != nil {
			c.sendAlert(alertBadCertificate)
			return nil, errors.New("tls: failed to verify client's certificate: " + err.Error())
		}

		ok := false
		for _, ku := range certs[0].ExtKeyUsage {
			if ku == x509.ExtKeyUsageClientAuth {
				ok = true
				break
			}
		}
		if !ok {
			c.sendAlert(alertHandshakeFailure)
			return nil, errors.New("tls: client's certificate's extended key usage doesn't permit it to be used for client authentication")
		}

		c.verifiedChains = chains
	}

	if len(certs) > 0 {
		var pub crypto.PublicKey
		switch key := certs[0].PublicKey.(type) {
		case *ecdsa.PublicKey, *rsa.PublicKey:
			pub = key
		default:
			c.sendAlert(alertUnsupportedCertificate)
			return nil, fmt.Errorf("tls: client's certificate contains an unsupported public key of type %T", certs[0].PublicKey)
		}
		c.peerCertificates = certs
		return pub, nil
	}

	return nil, nil
}

func (hs *serverHandshakeState) writeServerHash(msg []byte) {
	// writeServerHash is called before writeRecord.
	hs.writeHash(msg, hs.c.sendHandshakeSeq)
}

func (hs *serverHandshakeState) writeClientHash(msg []byte) {
	// writeClientHash is called after readHandshake.
	hs.writeHash(msg, hs.c.recvHandshakeSeq-1)
}

func (hs *serverHandshakeState) writeHash(msg []byte, seqno uint16) {
	if hs.c.isDTLS {
		// This is somewhat hacky. DTLS hashes a slightly different format.
		// First, the TLS header.
		hs.finishedHash.Write(msg[:4])
		// Then the sequence number and reassembled fragment offset (always 0).
		hs.finishedHash.Write([]byte{byte(seqno >> 8), byte(seqno), 0, 0, 0})
		// Then the reassembled fragment (always equal to the message length).
		hs.finishedHash.Write(msg[1:4])
		// And then the message body.
		hs.finishedHash.Write(msg[4:])
	} else {
		hs.finishedHash.Write(msg)
	}
}

// tryCipherSuite returns a cipherSuite with the given id if that cipher suite
// is acceptable to use.
func (c *Conn) tryCipherSuite(id uint16, supportedCipherSuites []uint16, version uint16, ellipticOk, ecdsaOk bool) *cipherSuite {
	for _, supported := range supportedCipherSuites {
		if id == supported {
			var candidate *cipherSuite

			for _, s := range cipherSuites {
				if s.id == id {
					candidate = s
					break
				}
			}
			if candidate == nil {
				continue
			}
			// Don't select a ciphersuite which we can't
			// support for this client.
			if (candidate.flags&suiteECDHE != 0) && !ellipticOk {
				continue
			}
			if (candidate.flags&suiteECDSA != 0) != ecdsaOk {
				continue
			}
			if !c.config.Bugs.SkipCipherVersionCheck && version < VersionTLS12 && candidate.flags&suiteTLS12 != 0 {
				continue
			}
			if c.isDTLS && candidate.flags&suiteNoDTLS != 0 {
				continue
			}
			return candidate
		}
	}

	return nil
}
