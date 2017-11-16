void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &lzma = s.addTarget<LibraryTarget>("xz_utils.lzma", "5.2.3");
    lzma.Source = RemoteFile("https://tukaani.org/xz/xz-{M}.{m}.{p}.tar.gz");

    lzma.setChecks("lzma");

    lzma +=
        "src/common/.*\\.c"_rr,
        "src/common/.*\\.h"_rr,
        "src/liblzma/.*\\.c"_rr,
        "src/liblzma/.*\\.h"_rr;

    lzma -=
        "src/liblzma/check/crc32_small.c",
        "src/liblzma/check/crc32_tablegen.c",
        "src/liblzma/check/crc64_small.c",
        "src/liblzma/check/crc64_tablegen.c",
        "src/liblzma/lzma/fastpos_tablegen.c",
        "src/liblzma/rangecoder/price_tablegen.c";

    lzma.Private +=
        "src/liblzma/lzma"_id,
        "src/liblzma/simple"_id,
        "src/liblzma/lz"_id,
        "src/liblzma/common"_id,
        "src/liblzma/check"_id,
        "src/liblzma/rangecoder"_id,
        "src/liblzma/delta"_id,
        "src/common"_id;

    lzma.Public +=
        "src/liblzma/api"_id;

    lzma.Private += "HAVE_CONFIG_H"_d;
    if (s.Settings.TargetOS.Type != OSType::Windows)
    {
        lzma.Private += "MYTHREAD_POSIX"_d;
    }
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        lzma.Private += "MYTHREAD_WIN95"_d;
    }
    lzma.Private += sw::Shared, "DLL_EXPORT"_d;
    lzma.Public += sw::Static, "LZMA_API_STATIC"_d;
    
    lzma.replaceInFileOnce("src/liblzma/check/check.h",
        "#ifndef LZMA_SHA256FUNC",
        "#undef LZMA_SHA256FUNC\n#ifndef LZMA_SHA256FUNC");
    lzma.fileWriteOnce("config.h", R"(
#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#endif

/* Prefix for symbols exported by tuklib_*.c files */
#define TUKLIB_SYMBOL_PREFIX lzma_

/* How many MiB of RAM to assume if the real amount cannot be determined. */
#define ASSUME_RAM 128

/* Define to 1 if crc32 integrity check is enabled. */
#define HAVE_CHECK_CRC32 1

/* Define to 1 if crc64 integrity check is enabled. */
#define HAVE_CHECK_CRC64 1

/* Define to 1 if sha256 integrity check is enabled. */
#define HAVE_CHECK_SHA256 1
#define HAVE_INTERNAL_SHA256 1

/* Define to 1 if arm decoder is enabled. */
#define HAVE_DECODER_ARM 1

/* Define to 1 if armthumb decoder is enabled. */
#define HAVE_DECODER_ARMTHUMB 1

/* Define to 1 if delta decoder is enabled. */
#define HAVE_DECODER_DELTA 1

/* Define to 1 if ia64 decoder is enabled. */
#define HAVE_DECODER_IA64 1

/* Define to 1 if lzma1 decoder is enabled. */
#define HAVE_DECODER_LZMA1 1

/* Define to 1 if lzma2 decoder is enabled. */
#define HAVE_DECODER_LZMA2 1

/* Define to 1 if powerpc decoder is enabled. */
#define HAVE_DECODER_POWERPC 1

/* Define to 1 if sparc decoder is enabled. */
#define HAVE_DECODER_SPARC 1

/* Define to 1 if x86 decoder is enabled. */
#define HAVE_DECODER_X86 1

/* Define to 1 if arm encoder is enabled. */
#define HAVE_ENCODER_ARM 1

/* Define to 1 if armthumb encoder is enabled. */
#define HAVE_ENCODER_ARMTHUMB 1

/* Define to 1 if delta encoder is enabled. */
#define HAVE_ENCODER_DELTA 1

/* Define to 1 if ia64 encoder is enabled. */
#define HAVE_ENCODER_IA64 1

/* Define to 1 if lzma1 encoder is enabled. */
#define HAVE_ENCODER_LZMA1 1

/* Define to 1 if lzma2 encoder is enabled. */
#define HAVE_ENCODER_LZMA2 1

/* Define to 1 if powerpc encoder is enabled. */
#define HAVE_ENCODER_POWERPC 1

/* Define to 1 if sparc encoder is enabled. */
#define HAVE_ENCODER_SPARC 1

/* Define to 1 if x86 encoder is enabled. */
#define HAVE_ENCODER_X86 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <limits.h> header file. */
#define HAVE_LIMITS_H 1

/* Define to 1 to enable bt2 match finder. */
#define HAVE_MF_BT2 1

/* Define to 1 to enable bt3 match finder. */
#define HAVE_MF_BT3 1

/* Define to 1 to enable bt4 match finder. */
#define HAVE_MF_BT4 1

/* Define to 1 to enable hc3 match finder. */
#define HAVE_MF_HC3 1

/* Define to 1 to enable hc4 match finder. */
#define HAVE_MF_HC4 1
)", true);
}

void check(Checker &c)
{
    auto &s = c.addSet("lzma");
    s.checkFunctionExists("CC_SHA256_Init");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("futimens");
    s.checkFunctionExists("futimes");
    s.checkFunctionExists("futimesat");
    s.checkFunctionExists("posix_fadvise");
    s.checkFunctionExists("pthread_condattr_setclock");
    s.checkFunctionExists("SHA256Init");
    s.checkFunctionExists("SHA256_Init");
    s.checkFunctionExists("utime");
    s.checkFunctionExists("utimes");
    s.checkIncludeExists("CommonCrypto/CommonDigest.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("immintrin.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("minix/sha2.h");
    s.checkIncludeExists("sha256.h");
    s.checkIncludeExists("sha2.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("CC_SHA256_CTX");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("SHA256_CTX");
    s.checkTypeSize("SHA2_CTX");
    s.checkTypeSize("size_t");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("uint8_t");
    s.checkTypeSize("uintptr_t");
    s.checkTypeSize("void *");
    s.checkTypeSize("_Bool");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkDeclarationExists("CLOCK_MONOTONIC");
    s.checkDeclarationExists("_mm_movemask_epi8");

        s.checkStructMemberExists("struct stat", "st_atimensec").Parameters.Includes.push_back("sys/stat.h");
        s.checkStructMemberExists("struct stat", "st_atimespec.tv_nsec").Parameters.Includes.push_back("sys/stat.h");
        s.checkStructMemberExists("struct stat", "st_atim.st__tim.tv_nsec").Parameters.Includes.push_back("sys/stat.h");
        s.checkStructMemberExists("struct stat", "st_atim.tv_nsec").Parameters.Includes.push_back("sys/stat.h");
        s.checkStructMemberExists("struct stat", "st_uatime").Parameters.Includes.push_back("sys/stat.h");
}
