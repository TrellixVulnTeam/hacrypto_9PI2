#define KeccakP1600_implementation_config "6 rounds unrolled"
#define KeccakP1600_unrolling 6

#define KangarooTwelve_Final pqcrypto_keccak_KangarooTwelve_Final
#define KangarooTwelve_Initialize pqcrypto_keccak_KangarooTwelve_Initialize
#define KangarooTwelve pqcrypto_keccak_KangarooTwelve
#define KangarooTwelve_Squeeze pqcrypto_keccak_KangarooTwelve_Squeeze
#define KangarooTwelve_Update pqcrypto_keccak_KangarooTwelve_Update
#define KeccakF1600_FastLoop_Absorb pqcrypto_keccak_KeccakF1600_FastLoop_Absorb
#define Keccak_HashFinal pqcrypto_keccak_Keccak_HashFinal
#define Keccak_HashInitialize pqcrypto_keccak_Keccak_HashInitialize
#define Keccak_HashSqueeze pqcrypto_keccak_Keccak_HashSqueeze
#define Keccak_HashUpdate pqcrypto_keccak_Keccak_HashUpdate
#define KeccakP1600_AddBytesInLane pqcrypto_keccak_KeccakP1600_AddBytesInLane
#define KeccakP1600_AddBytes pqcrypto_keccak_KeccakP1600_AddBytes
#define KeccakP1600_AddLanes pqcrypto_keccak_KeccakP1600_AddLanes
#define KeccakP1600_ExtractAndAddBytesInLane pqcrypto_keccak_KeccakP1600_ExtractAndAddBytesInLane
#define KeccakP1600_ExtractAndAddBytes pqcrypto_keccak_KeccakP1600_ExtractAndAddBytes
#define KeccakP1600_ExtractAndAddLanes pqcrypto_keccak_KeccakP1600_ExtractAndAddLanes
#define KeccakP1600_ExtractBytesInLane pqcrypto_keccak_KeccakP1600_ExtractBytesInLane
#define KeccakP1600_ExtractBytes pqcrypto_keccak_KeccakP1600_ExtractBytes
#define KeccakP1600_ExtractLanes pqcrypto_keccak_KeccakP1600_ExtractLanes
#define KeccakP1600_Initialize pqcrypto_keccak_KeccakP1600_Initialize
#define KeccakP1600_OverwriteBytesInLane pqcrypto_keccak_KeccakP1600_OverwriteBytesInLane
#define KeccakP1600_OverwriteBytes pqcrypto_keccak_KeccakP1600_OverwriteBytes
#define KeccakP1600_OverwriteLanes pqcrypto_keccak_KeccakP1600_OverwriteLanes
#define KeccakP1600_OverwriteWithZeroes pqcrypto_keccak_KeccakP1600_OverwriteWithZeroes
#define KeccakP1600_Permute_12rounds pqcrypto_keccak_KeccakP1600_Permute_12rounds
#define KeccakP1600_Permute_24rounds pqcrypto_keccak_KeccakP1600_Permute_24rounds
#define KeccakP1600_Permute_Nrounds pqcrypto_keccak_KeccakP1600_Permute_Nrounds
#define KeccakP1600times2_AddByte pqcrypto_keccak_KeccakP1600times2_AddByte
#define KeccakP1600times2_AddBytes pqcrypto_keccak_KeccakP1600times2_AddBytes
#define KeccakP1600times2_AddLanesAll pqcrypto_keccak_KeccakP1600times2_AddLanesAll
#define KeccakP1600times2_ExtractAndAddBytes pqcrypto_keccak_KeccakP1600times2_ExtractAndAddBytes
#define KeccakP1600times2_ExtractAndAddLanesAll pqcrypto_keccak_KeccakP1600times2_ExtractAndAddLanesAll
#define KeccakP1600times2_ExtractBytes pqcrypto_keccak_KeccakP1600times2_ExtractBytes
#define KeccakP1600times2_ExtractLanesAll pqcrypto_keccak_KeccakP1600times2_ExtractLanesAll
#define KeccakP1600times2_InitializeAll pqcrypto_keccak_KeccakP1600times2_InitializeAll
#define KeccakP1600times2_OverwriteBytes pqcrypto_keccak_KeccakP1600times2_OverwriteBytes
#define KeccakP1600times2_OverwriteLanesAll pqcrypto_keccak_KeccakP1600times2_OverwriteLanesAll
#define KeccakP1600times2_OverwriteWithZeroes pqcrypto_keccak_KeccakP1600times2_OverwriteWithZeroes
#define KeccakP1600times2_PermuteAll_12rounds pqcrypto_keccak_KeccakP1600times2_PermuteAll_12rounds
#define KeccakP1600times2_PermuteAll_24rounds pqcrypto_keccak_KeccakP1600times2_PermuteAll_24rounds
#define KeccakP1600times2_PermuteAll_4rounds pqcrypto_keccak_KeccakP1600times2_PermuteAll_4rounds
#define KeccakP1600times2_PermuteAll_6rounds pqcrypto_keccak_KeccakP1600times2_PermuteAll_6rounds
#define KeccakP1600times2_StaticInitialize pqcrypto_keccak_KeccakP1600times2_StaticInitialize
#define KeccakP1600times4_AddByte pqcrypto_keccak_KeccakP1600times4_AddByte
#define KeccakP1600times4_AddBytes pqcrypto_keccak_KeccakP1600times4_AddBytes
#define KeccakP1600times4_AddLanesAll pqcrypto_keccak_KeccakP1600times4_AddLanesAll
#define KeccakP1600times4_ExtractAndAddBytes pqcrypto_keccak_KeccakP1600times4_ExtractAndAddBytes
#define KeccakP1600times4_ExtractAndAddLanesAll pqcrypto_keccak_KeccakP1600times4_ExtractAndAddLanesAll
#define KeccakP1600times4_ExtractBytes pqcrypto_keccak_KeccakP1600times4_ExtractBytes
#define KeccakP1600times4_ExtractLanesAll pqcrypto_keccak_KeccakP1600times4_ExtractLanesAll
#define KeccakP1600times4_InitializeAll pqcrypto_keccak_KeccakP1600times4_InitializeAll
#define KeccakP1600times4_OverwriteBytes pqcrypto_keccak_KeccakP1600times4_OverwriteBytes
#define KeccakP1600times4_OverwriteLanesAll pqcrypto_keccak_KeccakP1600times4_OverwriteLanesAll
#define KeccakP1600times4_OverwriteWithZeroes pqcrypto_keccak_KeccakP1600times4_OverwriteWithZeroes
#define KeccakP1600times4_PermuteAll_12rounds pqcrypto_keccak_KeccakP1600times4_PermuteAll_12rounds
#define KeccakP1600times4_PermuteAll_24rounds pqcrypto_keccak_KeccakP1600times4_PermuteAll_24rounds
#define KeccakP1600times4_PermuteAll_4rounds pqcrypto_keccak_KeccakP1600times4_PermuteAll_4rounds
#define KeccakP1600times4_PermuteAll_6rounds pqcrypto_keccak_KeccakP1600times4_PermuteAll_6rounds
#define KeccakP1600times4_StaticInitialize pqcrypto_keccak_KeccakP1600times4_StaticInitialize
#define KeccakP1600times8_AddByte pqcrypto_keccak_KeccakP1600times8_AddByte
#define KeccakP1600times8_AddBytes pqcrypto_keccak_KeccakP1600times8_AddBytes
#define KeccakP1600times8_AddLanesAll pqcrypto_keccak_KeccakP1600times8_AddLanesAll
#define KeccakP1600times8_ExtractAndAddBytes pqcrypto_keccak_KeccakP1600times8_ExtractAndAddBytes
#define KeccakP1600times8_ExtractAndAddLanesAll pqcrypto_keccak_KeccakP1600times8_ExtractAndAddLanesAll
#define KeccakP1600times8_ExtractBytes pqcrypto_keccak_KeccakP1600times8_ExtractBytes
#define KeccakP1600times8_ExtractLanesAll pqcrypto_keccak_KeccakP1600times8_ExtractLanesAll
#define KeccakP1600times8_InitializeAll pqcrypto_keccak_KeccakP1600times8_InitializeAll
#define KeccakP1600times8_OverwriteBytes pqcrypto_keccak_KeccakP1600times8_OverwriteBytes
#define KeccakP1600times8_OverwriteLanesAll pqcrypto_keccak_KeccakP1600times8_OverwriteLanesAll
#define KeccakP1600times8_OverwriteWithZeroes pqcrypto_keccak_KeccakP1600times8_OverwriteWithZeroes
#define KeccakP1600times8_PermuteAll_12rounds pqcrypto_keccak_KeccakP1600times8_PermuteAll_12rounds
#define KeccakP1600times8_PermuteAll_24rounds pqcrypto_keccak_KeccakP1600times8_PermuteAll_24rounds
#define KeccakP1600times8_PermuteAll_4rounds pqcrypto_keccak_KeccakP1600times8_PermuteAll_4rounds
#define KeccakP1600times8_PermuteAll_6rounds pqcrypto_keccak_KeccakP1600times8_PermuteAll_6rounds
#define KeccakP1600times8_StaticInitialize pqcrypto_keccak_KeccakP1600times8_StaticInitialize
#define KeccakWidth1600_12rounds_SpongeAbsorbLastFewBits pqcrypto_keccak_KeccakWidth1600_12rounds_SpongeAbsorbLastFewBits
#define KeccakWidth1600_12rounds_SpongeAbsorb pqcrypto_keccak_KeccakWidth1600_12rounds_SpongeAbsorb
#define KeccakWidth1600_12rounds_SpongeInitialize pqcrypto_keccak_KeccakWidth1600_12rounds_SpongeInitialize
#define KeccakWidth1600_12rounds_Sponge pqcrypto_keccak_KeccakWidth1600_12rounds_Sponge
#define KeccakWidth1600_12rounds_SpongeSqueeze pqcrypto_keccak_KeccakWidth1600_12rounds_SpongeSqueeze
#define KeccakWidth1600_SpongeAbsorbLastFewBits pqcrypto_keccak_KeccakWidth1600_SpongeAbsorbLastFewBits
#define KeccakWidth1600_SpongeAbsorb pqcrypto_keccak_KeccakWidth1600_SpongeAbsorb
#define KeccakWidth1600_SpongeInitialize pqcrypto_keccak_KeccakWidth1600_SpongeInitialize
#define KeccakWidth1600_Sponge pqcrypto_keccak_KeccakWidth1600_Sponge
#define KeccakWidth1600_SpongeSqueeze pqcrypto_keccak_KeccakWidth1600_SpongeSqueeze
#define SHA3_224 pqcrypto_keccak_SHA3_224
#define SHA3_256 pqcrypto_keccak_SHA3_256
#define SHA3_384 pqcrypto_keccak_SHA3_384
#define SHA3_512 pqcrypto_keccak_SHA3_512
#define SHAKE128 pqcrypto_keccak_SHAKE128
#define SHAKE256 pqcrypto_keccak_SHAKE256
