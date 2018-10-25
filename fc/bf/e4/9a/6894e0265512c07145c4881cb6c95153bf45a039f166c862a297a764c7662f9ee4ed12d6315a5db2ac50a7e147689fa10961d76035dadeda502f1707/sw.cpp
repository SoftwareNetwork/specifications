void build(Solution &s)
{
    auto &pixman = s.addTarget<StaticLibraryTarget>("freedesktop.pixman", "0.34.0");
    pixman += RemoteFile("https://www.cairographics.org/releases/pixman-{v}.tar.gz");

    pixman.PackageDefinitions = true;
    pixman.setChecks("pixman");

    pixman +=
        "pixman/.*\\.c"_rr,
        "pixman/.*\\.h"_rr;

    pixman -=
        "pixman/pixman-arm-neon.c",
        "pixman/pixman-arm-simd.c",
        "pixman/pixman-mips-dspr2.c",
        "pixman/pixman-region.c",
        "pixman/pixman-vmx.c";

    pixman.Public +=
        "pixman"_id;

    if (pixman.Variables["USE_VMX"] == "1")
        pixman += "pixman/pixman-vmx.c";
    if (pixman.Variables["USE_ARM_NEON"] == "1")
        pixman += "pixman/pixman-arm-neon.c";
    if (pixman.Variables["USE_ARM_SIMD"] == "1")
        pixman += "pixman/pixman-arm-simd.c";
    if (pixman.Variables["USE_MIPS_DSPR2"] == "1")
        pixman += "pixman/pixman-mips-dspr2.c";
        
    if (s.Settings.TargetOS.Type != OSType::Windows)
        pixman.Public += "HAVE_PTHREADS"_d;

    if (s.Settings.Native.CompilerType == CompilerType::Clang ||
        s.Settings.Native.CompilerType == CompilerType::GNU)
    {
        pixman["pixman/pixman-ssse3.c"].args.push_back("-mssse3");
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("pixman");
    s.checkFunctionExists("alarm");
    s.checkFunctionExists("getisax");
    s.checkFunctionExists("getpagesize");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("mprotect");
    s.checkFunctionExists("posix_memalign");
    s.checkFunctionExists("sigaction");
    s.checkIncludeExists("fenv.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("long");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    s.checkLibraryFunctionExists("m", "feenableexcept");
    s.checkLibraryFunctionExists("pixman-1", "pixman_version_string");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkSourceCompiles("USE_ARM_IWMMXT", R"sw_xxx(
    #ifndef __arm__
    #error "IWMMXT is only available on ARM"
    #endif
    #ifndef __IWMMXT__
    #error "IWMMXT not enabled (with -march=iwmmxt)"
    #endif
    #if defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 8))
    #error "Need GCC >= 4.8 for IWMMXT intrinsics"
    #endif
    #include <mmintrin.h>
    int main () {
        union {
                __m64 v;
                char c[8];
        } a = { .c = {1, 2, 3, 4, 5, 6, 7, 8} };
        int b = 4;
        __m64 c = _mm_srli_si64 (a.v, b);
    })sw_xxx");
    s.checkSourceCompiles("USE_ARM_NEON", R"sw_xxx(
    .text
    .fpu neon
    .arch armv7a
    .object_arch armv4
    .eabi_attribute 10, 0
    .arm
    .altmacro
    #ifndef __ARM_EABI__
    #error EABI is required (to be sure that calling conventions are compatible)
    #endif
    pld [r0]
    vmovn.u16 d0, q0)sw_xxx");
    s.checkSourceCompiles("USE_ARM_SIMD", R"sw_xxx(
    .text
    .arch armv6
    .object_arch armv4
    .arm
    .altmacro
    #ifndef __ARM_EABI__
    #error EABI is required (to be sure that calling conventions are compatible)
    #endif
    pld [r0]
    uqadd8 r0, r0, r0)sw_xxx");
    s.checkSourceCompiles("USE_GCC_INLINE_ASM", R"sw_xxx(
    int main () {
        /* Most modern architectures have a NOP instruction, so this is a fairly generic test. */
        asm volatile ( "\tnop\n" : : : "cc", "memory" );
        return 0;
    })sw_xxx");
    s.checkSourceCompiles("USE_LOONGSON_MMI", R"sw_xxx(
    #ifndef __mips_loongson_vector_rev
    #error "Loongson Multimedia Instructions are only available on Loongson"
    #endif
    #if defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 4))
    #error "Need GCC >= 4.4 for Loongson MMI compilation"
    #endif
    #include "pixman/loongson-mmintrin.h"
    int main () {
        union {
            __m64 v;
            char c[8];
        } a = { .c = {1, 2, 3, 4, 5, 6, 7, 8} };
        int b = 4;
        __m64 c = _mm_srli_pi16 (a.v, b);
        return 0;
    })sw_xxx");
    s.checkSourceCompiles("USE_MIPS_DSPR2", R"sw_xxx(
    #if !(defined(__mips__) &&  __mips_isa_rev >= 2)
    #error MIPS DSPr2 is currently only available on MIPS32r2 platforms.
    #endif
    int
    main ()
    {
        int c = 0, a = 0, b = 0;
        __asm__ __volatile__ (
            "precr.qb.ph %[c], %[a], %[b]          \n\t"
            : [c] "=r" (c)
            : [a] "r" (a), [b] "r" (b)
        );
        return c;
    })sw_xxx");
    s.checkSourceCompiles("USE_SSE2", R"sw_xxx(
    #if defined(__GNUC__) && (__GNUC__ < 4 || (__GNUC__ == 4 && __GNUC_MINOR__ < 2))
    #   if !defined(__amd64__) && !defined(__x86_64__)
    #      error "Need GCC >= 4.2 for SSE2 intrinsics on x86"
    #   endif
    #endif
    #include <mmintrin.h>
    #include <xmmintrin.h>
    #include <emmintrin.h>
    int param;
    int main () {
        __m128i a = _mm_set1_epi32 (param), b = _mm_set1_epi32 (param + 1), c;
        c = _mm_xor_si128 (a, b);
        return _mm_cvtsi128_si32(c);
    })sw_xxx");
    s.checkSourceCompiles("USE_SSSE3", R"sw_xxx(
    #include <mmintrin.h>
    #include <xmmintrin.h>
    #include <emmintrin.h>
    #include <tmmintrin.h>
    int param;
    int main () {
        __m128i a = _mm_set1_epi32 (param), b = _mm_set1_epi32 (param + 1), c;
        c = _mm_maddubs_epi16 (a, b);
        return _mm_cvtsi128_si32(c);
    })sw_xxx");
    s.checkSourceCompiles("USE_VMX", R"sw_xxx(
    #if defined(__GNUC__) && (__GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 4))
    #error "Need GCC >= 3.4 for sane altivec support"
    #endif
    #include <altivec.h>
    int main () {
        vector unsigned int v = vec_splat_u32 (1);
        v = vec_sub (v, v);
        return 0;
    })sw_xxx");
    s.checkSourceCompiles("USE_X86_MMX", R"sw_xxx(
    #if defined(__GNUC__) && (__GNUC__ < 3 || (__GNUC__ == 3 && __GNUC_MINOR__ < 4))
    #error "Need GCC >= 3.4 for MMX intrinsics"
    #endif
    #include <mmintrin.h>
    #include <stdint.h>

    /* Check support for block expressions */
    #define _mm_shuffle_pi16(A, N)                                              \
        ({                                                                      \
        __m64 ret;                                                      \
                                                                        \
        /* Some versions of clang will choke on K */                    \
        asm ("pshufw %2, %1, %0\n\t"                                    \
             : "=y" (ret)                                               \
             : "y" (A), "K" ((const int8_t)N)                           \
        );                                                              \
                                                                        \
        ret;                                                            \
        })

    int main () {
        __m64 v = _mm_cvtsi32_si64 (1);
        __m64 w;

        w = _mm_shuffle_pi16(v, 5);

        /* Some versions of clang will choke on this */
        asm ("pmulhuw %1, %0\n\t"
        : "+y" (w)
        : "y" (v)
        );

        return _mm_cvtsi64_si32 (v);
    })sw_xxx");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
    s.checkDeclarationExists("decl");
} 
