#ifndef CRYPTOPP_VALIDATE_H
#define CRYPTOPP_VALIDATE_H

#include "cryptlib.h"
#include "randpool.h"

bool ValidateAll(bool thorough);
bool TestSettings();
bool TestOS_RNG();

bool ValidateCRC32();
bool ValidateAdler32();
bool ValidateMD2();
bool ValidateMD4();
bool ValidateMD5();
bool ValidateSHA();
bool ValidateSHA2();
bool ValidateHAVAL();
bool ValidateTiger();
bool ValidateRIPEMD();
bool ValidatePanama();

bool ValidateMD5MAC();
bool ValidateHMAC();
bool ValidateXMACC();

bool ValidateCipherModes();
bool ValidatePBKDF();

bool ValidateDES();
bool ValidateIDEA();
bool ValidateSAFER();
bool ValidateRC2();
bool ValidateARC4();

bool ValidateRC5();
bool ValidateBlowfish();
bool ValidateDiamond2();
bool ValidateThreeWay();
bool ValidateGOST();
bool ValidateSHARK();
bool ValidateSEAL();
bool ValidateCAST();
bool ValidateSquare();
bool ValidateSKIPJACK();
bool ValidateRC6();
bool ValidateMARS();
bool ValidateRijndael();
bool ValidateTwofish();
bool ValidateSerpent();

bool ValidateBBS();
bool ValidateDH();
bool ValidateMQV();
bool ValidateRSA();
bool ValidateElGamal();
bool ValidateDLIES();
bool ValidateNR();
bool ValidateDSA(bool thorough);
bool ValidateLUC();
bool ValidateLUC_DL();
bool ValidateLUC_DH();
bool ValidateXTR_DH();
bool ValidateRabin();
bool ValidateRW();
//bool ValidateBlumGoldwasser();
bool ValidateECP();
bool ValidateEC2N();
bool ValidateECDSA();
bool ValidateESIGN();

CryptoPP::RandomPool & GlobalRNG();

#endif
