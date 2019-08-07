void build(Solution &s)
{
    auto &openssl = s.addProject("openssl", "1.1.1.3");
    openssl += Git("https://github.com/openssl/openssl", "OpenSSL_{M}_{m}_{p}c");

    auto &crypto = openssl.addTarget<LibraryTarget>("crypto");
    {
        crypto +=
            "crypto/.*\\.c"_rr,
            "crypto/.*\\.h"_rr,
            "e_os.h",
            "engines/.*\\.c"_rr,
            "engines/.*\\.h"_rr,
            "include/.*"_rr;

        crypto -=
            "crypto/LPdir_.*"_rr,
            "crypto/aes/asm/.*"_rr,
            //"crypto/bf/.*"_rr,
            "crypto/bn/asm/.*"_rr,
            "crypto/des/ncbc_enc.c",
            "crypto/md2/.*"_rr,
            "crypto/rc5/.*"_rr,
            "engines/e_afalg.*"_rr;

        crypto.Private +=
            "crypto/include"_id,
            "."_id,
            "crypto/modes"_id,
            "crypto/asn1"_id,
            "crypto/dsa"_id,
            "crypto/evp"_id,
            "crypto/ec/curve448"_id,
            "crypto/ec/curve448/arch_32"_id;

        crypto.Public +=
            "include"_id;

        crypto.ExportAllSymbols = true;

        crypto.Private += "NO_WINDOWS_BRAINDEATH"_d;
        crypto.Private += "OPENSSL_NO_DYNAMIC_ENGINE"_d;
        crypto.Private += "OPENSSL_RAND_SEED_OS"_d;
        crypto.Public += "OPENSSL_NO_ASM"_d;
        if (crypto.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            crypto.Private += "DSO_WIN32"_d;
            crypto.Public += "Crypt32.lib"_slib;
            crypto.Public += "bCrypt.lib"_slib;
            crypto.Public += "WIN32_LEAN_AND_MEAN"_d;
        }
        else
        {
            crypto.Private += "HAVE_DLFCN_H"_d;
            crypto.Private += "DSO_DLFCN"_d;
            //crypto.Private += "DSO_DL"_d;
        }

        if (crypto.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            crypto.Public += "ws2_32.lib"_slib, "advapi32.lib"_slib, "User32.lib"_slib;
        }

        if (crypto.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            crypto.Public.Definitions["OPENSSLDIR"] = "\"C:/Program Files/Common Files/SSL/\"";
            crypto.Public.Definitions["ENGINESDIR"] = "\"C:/Program Files/OpenSSL/lib/engines/\"";
        }
        else
        {
            crypto.Public.Definitions["OPENSSLDIR"] = "\"/usr/local/ssl\"";
            crypto.Public.Definitions["ENGINESDIR"] = "\"/usr/local/ssl/lib/engines\"";
        }

        crypto -=
            // arch stuff
            "crypto/armcap.c",
            "crypto/s390xcap.c",
            "crypto/sparcv9cap.c",
            "crypto/ppccap.c",
            "crypto/aes/aes_x86core.c",

            "crypto/ec/ecp_nistz256.c",
            "crypto/ec/ecp_nistz256_table.c"
            ;

        if (crypto.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            crypto -= "crypto/poly1305/poly1305_ieee754.c";
        }

        crypto -= "crypto/poly1305/poly1305_base2_44.c";
        crypto -= "crypto/engine/eng_devcrypto.c";

        crypto.Variables["OPENSSL_SYS"] = "UNIX";
        if (crypto.getBuildSettings().TargetOS.Type == OSType::Linux)
            crypto.Variables["OPENSSL_SYS"] = "LINUX";
        else if (crypto.getBuildSettings().TargetOS.Type == OSType::Cygwin)
            crypto.Variables["OPENSSL_SYS"] = "CYGWIN";
        else if (crypto.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            if (crypto.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
                crypto.Variables["OPENSSL_SYS"] = "WIN64A";
            else
                crypto.Variables["OPENSSL_SYS"] = "WIN32";
        }
        else if (crypto.getBuildSettings().TargetOS.Type == OSType::Macos)
            crypto.Variables["OPENSSL_SYS"] = "MACOSX";

        /*
        if (MINGW)
        if (CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(OPENSSL_SYS MINGW64)
        else()
        set(OPENSSL_SYS MINGW32)
        endif()
        endif()*/

        if (crypto.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
        {
            crypto.Variables["SIXTY_FOUR_BIT"] = "define";
            crypto.Variables["THIRTY_TWO_BIT"] = "undef";
        }
        else
        {
            crypto.Variables["SIXTY_FOUR_BIT"] = "undef";
            crypto.Variables["THIRTY_TWO_BIT"] = "define";
        }

        if (crypto.getBuildSettings().TargetOS.Type == OSType::Windows)
            crypto.Variables["CPPAN_SHARED_LIBRARY_SUFFIX"] = ".dll";
        else if (crypto.getBuildSettings().TargetOS.Type == OSType::Macos)
            crypto.Variables["CPPAN_SHARED_LIBRARY_SUFFIX"] = ".dylib";
        else
            crypto.Variables["CPPAN_SHARED_LIBRARY_SUFFIX"] = ".so";

        crypto.writeFileOnce(crypto.BinaryPrivateDir / "buildinf.h", R"xxx(
#define DATE ""
#define compiler_flags ""
#define PLATFORM ""
    )xxx");

        crypto.writeFileOnce("openssl/opensslconf.h.in", R"xxx(
/*
     *
     * Copyright 2016 The OpenSSL Project Authors. All Rights Reserved.
     *
     * Licensed under the OpenSSL license (the "License").  You may not use
     * this file except in compliance with the License.  You can obtain a copy
     * in the file LICENSE in the source distribution or at
     * https://www.openssl.org/source/license.html
     */

    #ifdef  __cplusplus
    extern "C" {
    #endif

    #ifdef OPENSSL_ALGORITHM_DEFINES
    # error OPENSSL_ALGORITHM_DEFINES no longer supported
    #endif

    /*
     * OpenSSL was configured with the following options:
     */

    #ifndef OPENSSL_SYS_${OPENSSL_SYS}
    # define OPENSSL_SYS_${OPENSSL_SYS} 1
    #endif
    #ifndef OPENSSL_NO_MD2
    # define OPENSSL_NO_MD2
    #endif
    #ifndef OPENSSL_NO_RC5
    # define OPENSSL_NO_RC5
    #endif
    #ifndef OPENSSL_THREADS
    # define OPENSSL_THREADS
    #endif
    #ifndef OPENSSL_NO_ASAN
    # define OPENSSL_NO_ASAN
    #endif
    #ifndef OPENSSL_NO_CRYPTO_MDEBUG
    # define OPENSSL_NO_CRYPTO_MDEBUG
    #endif
    #ifndef OPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE
    # define OPENSSL_NO_CRYPTO_MDEBUG_BACKTRACE
    #endif
    #ifndef OPENSSL_NO_EC_NISTP_64_GCC_128
    # define OPENSSL_NO_EC_NISTP_64_GCC_128
    #endif
    //#ifndef OPENSSL_NO_EGD
    //# define OPENSSL_NO_EGD
    //#endif
    #ifndef OPENSSL_NO_FUZZ_AFL
    # define OPENSSL_NO_FUZZ_AFL
    #endif
    #ifndef OPENSSL_NO_FUZZ_LIBFUZZER
    # define OPENSSL_NO_FUZZ_LIBFUZZER
    #endif
    #ifndef OPENSSL_NO_HEARTBEATS
    # define OPENSSL_NO_HEARTBEATS
    #endif
    #ifndef OPENSSL_NO_MSAN
    # define OPENSSL_NO_MSAN
    #endif
    #ifndef OPENSSL_NO_SCTP
    # define OPENSSL_NO_SCTP
    #endif
    #ifndef OPENSSL_NO_SSL_TRACE
    # define OPENSSL_NO_SSL_TRACE
    #endif
    #ifndef OPENSSL_NO_SSL3
    # define OPENSSL_NO_SSL3
    #endif
    #ifndef OPENSSL_NO_SSL3_METHOD
    # define OPENSSL_NO_SSL3_METHOD
    #endif
    #ifndef OPENSSL_NO_UBSAN
    # define OPENSSL_NO_UBSAN
    #endif
    #ifndef OPENSSL_NO_UNIT_TEST
    # define OPENSSL_NO_UNIT_TEST
    #endif
    #ifndef OPENSSL_NO_WEAK_SSL_CIPHERS
    # define OPENSSL_NO_WEAK_SSL_CIPHERS
    #endif
    #ifndef OPENSSL_NO_AFALGENG
    # define OPENSSL_NO_AFALGENG
    #endif

    #define OPENSSL_NO_DEVCRYPTOENG

    /*
     * Sometimes OPENSSSL_NO_xxx ends up with an empty file and some compilers
     * don't like that.  This will hopefully silence them.
     */
    #define NON_EMPTY_TRANSLATION_UNIT static void *dummy = &dummy;

    /*
     * Applications should use -DOPENSSL_API_COMPAT=<version> to suppress the
     * declarations of functions deprecated in or before <version>. Otherwise, they
     * still won't see them if the library has been built to disable deprecated
     * functions.
     */
    #if defined(OPENSSL_NO_DEPRECATED)
    # define DECLARE_DEPRECATED(f)
    #elif __GNUC__ > 3 || (__GNUC__ == 3 && __GNUC_MINOR__ > 0)
    # define DECLARE_DEPRECATED(f)    f __attribute__ ((deprecated));
    #else
    # define DECLARE_DEPRECATED(f)   f;
    #endif

    #ifndef OPENSSL_FILE
    # ifdef OPENSSL_NO_FILENAMES
    #  define OPENSSL_FILE ""
    #  define OPENSSL_LINE 0
    # else
    #  define OPENSSL_FILE __FILE__
    #  define OPENSSL_LINE __LINE__
    # endif
    #endif

    #ifndef OPENSSL_MIN_API
    # define OPENSSL_MIN_API 0
    #endif

    #if !defined(OPENSSL_API_COMPAT) || OPENSSL_API_COMPAT < OPENSSL_MIN_API
    # undef OPENSSL_API_COMPAT
    # define OPENSSL_API_COMPAT OPENSSL_MIN_API
    #endif

    #if OPENSSL_VERSION_NUMBER < 0x10200000L
    # define DEPRECATEDIN_1_2_0(f)   f;
    #elif OPENSSL_API_COMPAT < 0x10200000L
    # define DEPRECATEDIN_1_2_0(f)   DECLARE_DEPRECATED(f)
    #else
    # define DEPRECATEDIN_1_2_0(f)
    #endif

    #if OPENSSL_API_COMPAT < 0x10100000L
    # define DEPRECATEDIN_1_1_0(f)   DECLARE_DEPRECATED(f)
    #else
    # define DEPRECATEDIN_1_1_0(f)
    #endif

    #if OPENSSL_API_COMPAT < 0x10000000L
    # define DEPRECATEDIN_1_0_0(f)   DECLARE_DEPRECATED(f)
    #else
    # define DEPRECATEDIN_1_0_0(f)
    #endif

    #if OPENSSL_API_COMPAT < 0x00908000L
    # define DEPRECATEDIN_0_9_8(f)   DECLARE_DEPRECATED(f)
    #else
    # define DEPRECATEDIN_0_9_8(f)
    #endif

    //#define OPENSSL_CPUID_OBJ

    /* Generate 80386 code? */
    #undef I386_ONLY

    #undef OPENSSL_UNISTD
    #define OPENSSL_UNISTD <unistd.h>

    #define OPENSSL_EXPORT_VAR_AS_FUNCTION

    /*
     * The following are cipher-specific, but are part of the public API.
     */
    #if !defined(OPENSSL_SYS_UEFI)
    # undef BN_LLONG
    /* Only one for the following should be defined */
    # undef SIXTY_FOUR_BIT_LONG
    # ${SIXTY_FOUR_BIT} SIXTY_FOUR_BIT
    # ${THIRTY_TWO_BIT} THIRTY_TWO_BIT
    #endif

    #define RC4_INT unsigned int

    #ifdef  __cplusplus
    }
    #endif
)xxx");

        crypto.writeFileOnce("internal/bn_conf.h.in", R"xxx(
    /*
     * Copyright 2016 The OpenSSL Project Authors. All Rights Reserved.
     *
     * Licensed under the OpenSSL license (the "License").  You may not use
     * this file except in compliance with the License.  You can obtain a copy
     * in the file LICENSE in the source distribution or at
     * https://www.openssl.org/source/license.html
     */

    #ifndef HEADER_BN_CONF_H
    # define HEADER_BN_CONF_H

    /*
     * The contents of this file are not used in the UEFI build, as
     * both 32-bit and 64-bit builds are supported from a single run
     * of the Configure script.
     */

    /* Should we define BN_DIV2W here? */

    /* Only one for the following should be defined */
    # undef SIXTY_FOUR_BIT_LONG
    # ${SIXTY_FOUR_BIT} SIXTY_FOUR_BIT
    # ${THIRTY_TWO_BIT} THIRTY_TWO_BIT

    #endif
)xxx");

        crypto.writeFileOnce("internal/dso_conf.h.in", R"xxx(
    /*
     * Copyright 2016 The OpenSSL Project Authors. All Rights Reserved.
     *
     * Licensed under the OpenSSL license (the "License").  You may not use
     * this file except in compliance with the License.  You can obtain a copy
     * in the file LICENSE in the source distribution or at
     * https://www.openssl.org/source/license.html
     */

    #ifndef HEADER_DSO_CONF_H
    # define HEADER_DSO_CONF_H

    # define DSO_EXTENSION "${CPPAN_SHARED_LIBRARY_SUFFIX}"
    #endif
)xxx");

        crypto.configureFile(crypto.BinaryDir / "openssl/opensslconf.h.in", "openssl/opensslconf.h");
        crypto.configureFile(crypto.BinaryDir / "internal/bn_conf.h.in", "internal/bn_conf.h");
        crypto.configureFile(crypto.BinaryDir / "internal/dso_conf.h.in", "internal/dso_conf.h");
    }

    auto &ssl = openssl.addTarget<LibraryTarget>("ssl");
    {
        ssl.ExportAllSymbols = true;
        ssl +=
            "e_os.h",
            "ssl/.*\\.c"_rr,
            "ssl/.*\\.h"_rr;

        ssl.Public += crypto;
    }
}
