# Microsoft Developer Studio Project File - Name="cryptlib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=cryptlib - Win32 FIPS 140 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cryptlib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cryptlib.mak" CFG="cryptlib - Win32 FIPS 140 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cryptlib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cryptlib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "cryptlib - Win32 FIPS 140 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "cryptlib - Win32 FIPS 140 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/cryptlib", BAAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cryptlib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cryptlib"
# PROP BASE Intermediate_Dir "cryptlib"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "release"
# PROP Intermediate_Dir "release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /GB /Gd /MT /W3 /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_PRECOMPILED_HEADERS" /Yu"pch.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cryptlib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cryptli0"
# PROP BASE Intermediate_Dir "cryptli0"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "debug"
# PROP Intermediate_Dir "debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /Z7 /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo /MTd /W3 /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_PRECOMPILED_HEADERS" /Yu"pch.h" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo

!ELSEIF  "$(CFG)" == "cryptlib - Win32 FIPS 140 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "cryptlib___Win32_FIPS_140_Release"
# PROP BASE Intermediate_Dir "cryptlib___Win32_FIPS_140_Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "FIPS_140_Release"
# PROP Intermediate_Dir "FIPS_140_Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /Gz /MT /W3 /GX /Zi /O2 /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_PRECOMPILED_HEADERS" /Yu"pch.h" /FD /c
# ADD CPP /nologo /G5 /Gz /MT /W3 /GX /Zi /O2 /D "NDEBUG" /D "_WINDOWS" /D "USE_PRECOMPILED_HEADERS" /D "WIN32" /D CRYPTOPP_ENABLE_COMPLIANCE_WITH_FIPS_140_2=1 /Yu"pch.h" /Fd"FIPS_140_Release/cryptopp" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"FIPS_140_Release\cryptopp.lib"

!ELSEIF  "$(CFG)" == "cryptlib - Win32 FIPS 140 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "cryptlib___Win32_FIPS_140_Debug"
# PROP BASE Intermediate_Dir "cryptlib___Win32_FIPS_140_Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "FIPS_140_Debug"
# PROP Intermediate_Dir "FIPS_140_Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /GX /ZI /Od /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "USE_PRECOMPILED_HEADERS" /Yu"pch.h" /FD /c
# ADD CPP /nologo /G5 /Gz /MTd /W3 /GX /ZI /Od /D "_DEBUG" /D "_WINDOWS" /D "USE_PRECOMPILED_HEADERS" /D "WIN32" /D CRYPTOPP_ENABLE_COMPLIANCE_WITH_FIPS_140_2=1 /Yu"pch.h" /Fd"FIPS_140_Debug/cryptopp" /FD /c
# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"FIPS_140_Debug\cryptopp.lib"

!ENDIF 

# Begin Target

# Name "cryptlib - Win32 Release"
# Name "cryptlib - Win32 Debug"
# Name "cryptlib - Win32 FIPS 140 Release"
# Name "cryptlib - Win32 FIPS 140 Debug"
# Begin Group "Source Files"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\3way.cpp
# End Source File
# Begin Source File

SOURCE=.\adler32.cpp
# End Source File
# Begin Source File

SOURCE=.\algebra.cpp
# End Source File
# Begin Source File

SOURCE=.\algparam.cpp
# End Source File
# Begin Source File

SOURCE=.\arc4.cpp
# End Source File
# Begin Source File

SOURCE=.\asn.cpp
# End Source File
# Begin Source File

SOURCE=.\base64.cpp
# End Source File
# Begin Source File

SOURCE=.\basecode.cpp
# End Source File
# Begin Source File

SOURCE=.\bfinit.cpp
# End Source File
# Begin Source File

SOURCE=.\blowfish.cpp
# End Source File
# Begin Source File

SOURCE=.\blumshub.cpp
# End Source File
# Begin Source File

SOURCE=.\cast.cpp
# End Source File
# Begin Source File

SOURCE=.\casts.cpp
# End Source File
# Begin Source File

SOURCE=.\channels.cpp
# End Source File
# Begin Source File

SOURCE=.\crc.cpp
# End Source File
# Begin Source File

SOURCE=.\cryptlib.cpp
# End Source File
# Begin Source File

SOURCE=.\default.cpp
# End Source File
# Begin Source File

SOURCE=.\des.cpp
# End Source File
# Begin Source File

SOURCE=.\dessp.cpp
# End Source File
# Begin Source File

SOURCE=.\dh.cpp
# End Source File
# Begin Source File

SOURCE=.\dh2.cpp
# End Source File
# Begin Source File

SOURCE=.\diamond.cpp
# End Source File
# Begin Source File

SOURCE=.\diamondt.cpp
# End Source File
# Begin Source File

SOURCE=.\dsa.cpp
# End Source File
# Begin Source File

SOURCE=.\ec2n.cpp
# End Source File
# Begin Source File

SOURCE=.\eccrypto.cpp
# End Source File
# Begin Source File

SOURCE=.\ecp.cpp
# End Source File
# Begin Source File

SOURCE=.\elgamal.cpp
# End Source File
# Begin Source File

SOURCE=.\eprecomp.cpp
# End Source File
# Begin Source File

SOURCE=.\esign.cpp
# End Source File
# Begin Source File

SOURCE=.\files.cpp
# End Source File
# Begin Source File

SOURCE=.\filters.cpp
# End Source File
# Begin Source File

SOURCE=.\fips140.cpp
# End Source File
# Begin Source File

SOURCE=.\fipstest.cpp
# End Source File
# Begin Source File

SOURCE=.\gf256.cpp
# End Source File
# Begin Source File

SOURCE=.\gf2_32.cpp
# End Source File
# Begin Source File

SOURCE=.\gf2n.cpp
# End Source File
# Begin Source File

SOURCE=.\gfpcrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\gost.cpp
# End Source File
# Begin Source File

SOURCE=.\gzip.cpp
# End Source File
# Begin Source File

SOURCE=.\haval.cpp
# End Source File
# Begin Source File

SOURCE=.\hex.cpp
# End Source File
# Begin Source File

SOURCE=.\hrtimer.cpp
# End Source File
# Begin Source File

SOURCE=.\ida.cpp
# End Source File
# Begin Source File

SOURCE=.\idea.cpp
# End Source File
# Begin Source File

SOURCE=.\integer.cpp
# End Source File
# Begin Source File

SOURCE=.\iterhash.cpp
# End Source File
# Begin Source File

SOURCE=.\luc.cpp
# End Source File
# Begin Source File

SOURCE=.\mars.cpp
# End Source File
# Begin Source File

SOURCE=.\marss.cpp
# End Source File
# Begin Source File

SOURCE=.\md2.cpp
# End Source File
# Begin Source File

SOURCE=.\md4.cpp
# End Source File
# Begin Source File

SOURCE=.\md5.cpp
# End Source File
# Begin Source File

SOURCE=.\md5mac.cpp
# End Source File
# Begin Source File

SOURCE=.\misc.cpp
# End Source File
# Begin Source File

SOURCE=.\modes.cpp
# End Source File
# Begin Source File

SOURCE=.\modexppc.cpp
# End Source File
# Begin Source File

SOURCE=.\mqueue.cpp
# End Source File
# Begin Source File

SOURCE=.\mqv.cpp
# End Source File
# Begin Source File

SOURCE=.\nbtheory.cpp
# End Source File
# Begin Source File

SOURCE=.\network.cpp
# End Source File
# Begin Source File

SOURCE=.\oaep.cpp
# End Source File
# Begin Source File

SOURCE=.\osrng.cpp
# End Source File
# Begin Source File

SOURCE=.\panama.cpp
# End Source File
# Begin Source File

SOURCE=.\pch.cpp
# ADD CPP /Yc"pch.h"
# End Source File
# Begin Source File

SOURCE=.\pkcspad.cpp
# End Source File
# Begin Source File

SOURCE=.\polynomi.cpp
# End Source File
# Begin Source File

SOURCE=.\pubkey.cpp
# End Source File
# Begin Source File

SOURCE=.\queue.cpp
# End Source File
# Begin Source File

SOURCE=.\rabin.cpp
# End Source File
# Begin Source File

SOURCE=.\randpool.cpp
# End Source File
# Begin Source File

SOURCE=.\rc2.cpp
# End Source File
# Begin Source File

SOURCE=.\rc5.cpp
# End Source File
# Begin Source File

SOURCE=.\rc6.cpp
# End Source File
# Begin Source File

SOURCE=.\rdtables.cpp
# End Source File
# Begin Source File

SOURCE=.\rijndael.cpp
# End Source File
# Begin Source File

SOURCE=.\ripemd.cpp
# End Source File
# Begin Source File

SOURCE=.\rng.cpp
# End Source File
# Begin Source File

SOURCE=.\rsa.cpp
# End Source File
# Begin Source File

SOURCE=.\rw.cpp
# End Source File
# Begin Source File

SOURCE=.\safer.cpp
# End Source File
# Begin Source File

SOURCE=.\sapphire.cpp
# End Source File
# Begin Source File

SOURCE=.\seal.cpp
# End Source File
# Begin Source File

SOURCE=.\serpent.cpp
# End Source File
# Begin Source File

SOURCE=.\sha.cpp
# End Source File
# Begin Source File

SOURCE=.\shark.cpp
# End Source File
# Begin Source File

SOURCE=.\sharkbox.cpp
# End Source File
# Begin Source File

SOURCE=.\simple.cpp
# End Source File
# Begin Source File

SOURCE=.\skipjack.cpp
# End Source File
# Begin Source File

SOURCE=.\socketft.cpp
# End Source File
# Begin Source File

SOURCE=.\square.cpp
# End Source File
# Begin Source File

SOURCE=.\squaretb.cpp
# End Source File
# Begin Source File

SOURCE=.\strciphr.cpp
# End Source File
# Begin Source File

SOURCE=.\tea.cpp
# End Source File
# Begin Source File

SOURCE=.\tftables.cpp
# End Source File
# Begin Source File

SOURCE=.\tiger.cpp
# End Source File
# Begin Source File

SOURCE=.\tigertab.cpp
# End Source File
# Begin Source File

SOURCE=.\trdlocal.cpp
# End Source File
# Begin Source File

SOURCE=.\twofish.cpp
# End Source File
# Begin Source File

SOURCE=.\wait.cpp
# End Source File
# Begin Source File

SOURCE=.\wake.cpp
# End Source File
# Begin Source File

SOURCE=.\winpipes.cpp
# End Source File
# Begin Source File

SOURCE=.\xtr.cpp
# End Source File
# Begin Source File

SOURCE=.\xtrcrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\zdeflate.cpp
# End Source File
# Begin Source File

SOURCE=.\zinflate.cpp
# End Source File
# Begin Source File

SOURCE=.\zlib.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ".;.h"
# Begin Source File

SOURCE=.\3way.h
# End Source File
# Begin Source File

SOURCE=.\adler32.h
# End Source File
# Begin Source File

SOURCE=.\aes.h
# End Source File
# Begin Source File

SOURCE=.\algebra.h
# End Source File
# Begin Source File

SOURCE=.\algparam.h
# End Source File
# Begin Source File

SOURCE=.\arc4.h
# End Source File
# Begin Source File

SOURCE=.\argnames.h
# End Source File
# Begin Source File

SOURCE=.\asn.h
# End Source File
# Begin Source File

SOURCE=.\base64.h
# End Source File
# Begin Source File

SOURCE=.\basecode.h
# End Source File
# Begin Source File

SOURCE=.\blowfish.h
# End Source File
# Begin Source File

SOURCE=.\blumshub.h
# End Source File
# Begin Source File

SOURCE=.\cast.h
# End Source File
# Begin Source File

SOURCE=.\cbcmac.h
# End Source File
# Begin Source File

SOURCE=.\channels.h
# End Source File
# Begin Source File

SOURCE=.\config.h
# End Source File
# Begin Source File

SOURCE=.\crc.h
# End Source File
# Begin Source File

SOURCE=.\cryptlib.h
# End Source File
# Begin Source File

SOURCE=.\default.h
# End Source File
# Begin Source File

SOURCE=.\des.h
# End Source File
# Begin Source File

SOURCE=.\dh.h
# End Source File
# Begin Source File

SOURCE=.\dh2.h
# End Source File
# Begin Source File

SOURCE=.\diamond.h
# End Source File
# Begin Source File

SOURCE=.\dmac.h
# End Source File
# Begin Source File

SOURCE=.\dsa.h
# End Source File
# Begin Source File

SOURCE=.\ec2n.h
# End Source File
# Begin Source File

SOURCE=.\eccrypto.h
# End Source File
# Begin Source File

SOURCE=.\ecp.h
# End Source File
# Begin Source File

SOURCE=.\elgamal.h
# End Source File
# Begin Source File

SOURCE=.\eprecomp.h
# End Source File
# Begin Source File

SOURCE=.\esign.h
# End Source File
# Begin Source File

SOURCE=.\files.h
# End Source File
# Begin Source File

SOURCE=.\filters.h
# End Source File
# Begin Source File

SOURCE=.\fips140.h
# End Source File
# Begin Source File

SOURCE=.\fltrimpl.h
# End Source File
# Begin Source File

SOURCE=.\gf256.h
# End Source File
# Begin Source File

SOURCE=.\gf2_32.h
# End Source File
# Begin Source File

SOURCE=.\gf2n.h
# End Source File
# Begin Source File

SOURCE=.\gfpcrypt.h
# End Source File
# Begin Source File

SOURCE=.\gost.h
# End Source File
# Begin Source File

SOURCE=.\gzip.h
# End Source File
# Begin Source File

SOURCE=.\haval.h
# End Source File
# Begin Source File

SOURCE=.\hex.h
# End Source File
# Begin Source File

SOURCE=.\hmac.h
# End Source File
# Begin Source File

SOURCE=.\hrtimer.h
# End Source File
# Begin Source File

SOURCE=.\ida.h
# End Source File
# Begin Source File

SOURCE=.\idea.h
# End Source File
# Begin Source File

SOURCE=.\integer.h
# End Source File
# Begin Source File

SOURCE=.\iterhash.h
# End Source File
# Begin Source File

SOURCE=.\lubyrack.h
# End Source File
# Begin Source File

SOURCE=.\luc.h
# End Source File
# Begin Source File

SOURCE=.\mars.h
# End Source File
# Begin Source File

SOURCE=.\md2.h
# End Source File
# Begin Source File

SOURCE=.\md4.h
# End Source File
# Begin Source File

SOURCE=.\md5.h
# End Source File
# Begin Source File

SOURCE=.\md5mac.h
# End Source File
# Begin Source File

SOURCE=.\mdc.h
# End Source File
# Begin Source File

SOURCE=.\misc.h
# End Source File
# Begin Source File

SOURCE=.\modarith.h
# End Source File
# Begin Source File

SOURCE=.\modes.h
# End Source File
# Begin Source File

SOURCE=.\modexppc.h
# End Source File
# Begin Source File

SOURCE=.\mqueue.h
# End Source File
# Begin Source File

SOURCE=.\mqv.h
# End Source File
# Begin Source File

SOURCE=.\nbtheory.h
# End Source File
# Begin Source File

SOURCE=.\network.h
# End Source File
# Begin Source File

SOURCE=.\nr.h
# End Source File
# Begin Source File

SOURCE=.\oaep.h
# End Source File
# Begin Source File

SOURCE=.\oids.h
# End Source File
# Begin Source File

SOURCE=.\osrng.h
# End Source File
# Begin Source File

SOURCE=.\panama.h
# End Source File
# Begin Source File

SOURCE=.\pch.h
# End Source File
# Begin Source File

SOURCE=.\pkcspad.h
# End Source File
# Begin Source File

SOURCE=.\polynomi.h
# End Source File
# Begin Source File

SOURCE=.\pssr.h
# End Source File
# Begin Source File

SOURCE=.\pubkey.h
# End Source File
# Begin Source File

SOURCE=.\pwdbased.h
# End Source File
# Begin Source File

SOURCE=.\queue.h
# End Source File
# Begin Source File

SOURCE=.\rabin.h
# End Source File
# Begin Source File

SOURCE=.\randpool.h
# End Source File
# Begin Source File

SOURCE=.\rc2.h
# End Source File
# Begin Source File

SOURCE=.\rc5.h
# End Source File
# Begin Source File

SOURCE=.\rc6.h
# End Source File
# Begin Source File

SOURCE=.\rijndael.h
# End Source File
# Begin Source File

SOURCE=.\ripemd.h
# End Source File
# Begin Source File

SOURCE=.\rng.h
# End Source File
# Begin Source File

SOURCE=.\rsa.h
# End Source File
# Begin Source File

SOURCE=.\rw.h
# End Source File
# Begin Source File

SOURCE=.\safer.h
# End Source File
# Begin Source File

SOURCE=.\sapphire.h
# End Source File
# Begin Source File

SOURCE=.\seal.h
# End Source File
# Begin Source File

SOURCE=.\secblock.h
# End Source File
# Begin Source File

SOURCE=.\seckey.h
# End Source File
# Begin Source File

SOURCE=.\serpent.h
# End Source File
# Begin Source File

SOURCE=.\sha.h
# End Source File
# Begin Source File

SOURCE=.\shark.h
# End Source File
# Begin Source File

SOURCE=.\simple.h
# End Source File
# Begin Source File

SOURCE=.\skipjack.h
# End Source File
# Begin Source File

SOURCE=.\smartptr.h
# End Source File
# Begin Source File

SOURCE=.\socketft.h
# End Source File
# Begin Source File

SOURCE=.\square.h
# End Source File
# Begin Source File

SOURCE=.\strciphr.h
# End Source File
# Begin Source File

SOURCE=.\tea.h
# End Source File
# Begin Source File

SOURCE=.\tiger.h
# End Source File
# Begin Source File

SOURCE=.\trdlocal.h
# End Source File
# Begin Source File

SOURCE=.\trunhash.h
# End Source File
# Begin Source File

SOURCE=.\twofish.h
# End Source File
# Begin Source File

SOURCE=.\wait.h
# End Source File
# Begin Source File

SOURCE=.\wake.h
# End Source File
# Begin Source File

SOURCE=.\winpipes.h
# End Source File
# Begin Source File

SOURCE=.\words.h
# End Source File
# Begin Source File

SOURCE=.\xormac.h
# End Source File
# Begin Source File

SOURCE=.\xtr.h
# End Source File
# Begin Source File

SOURCE=.\xtrcrypt.h
# End Source File
# Begin Source File

SOURCE=.\zdeflate.h
# End Source File
# Begin Source File

SOURCE=.\zinflate.h
# End Source File
# Begin Source File

SOURCE=.\zlib.h
# End Source File
# End Group
# Begin Group "Miscellaneous"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\Doxyfile
# End Source File
# Begin Source File

SOURCE=.\GNUmakefile
# End Source File
# Begin Source File

SOURCE=.\license.txt
# End Source File
# Begin Source File

SOURCE=.\readme.txt
# End Source File
# End Group
# End Target
# End Project
