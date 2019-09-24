#pragma sw require pub.egorpugin.primitives.http-master

#include <primitives/http.h>

void build(Solution &s)
{
    auto &vpx = s.addStaticLibrary("webmproject.vpx", "1.8.1");
    vpx += Git("https://github.com/webmproject/libvpx", "v{v}");

    vpx.setExtensionProgram(".asm", "org.sw.demo.yasm"_dep);

    //vpx.ExportAllSymbols = true;
    vpx.setChecks("vpx");

    vpx +=
        "third_party/x86inc/x86inc.asm",
        "vp8/.*"_rr,
        "vp9/.*"_rr,
        "vpx/.*"_rr,
        "vpx_.*"_rr;

    vpx -=
        ".*/arm/.*"_rr,
        ".*/mips/.*"_rr,
        ".*/ppc/.*"_rr,
        "vpx_ports/.*"_rr;

    vpx +=
        "vpx_ports/.*\\.asm"_rr;

    //vpx -= ".*_avx2.*"_rr;
    vpx -= "vpx_dsp/x86/highbd_.*"_rr;

    vpx.Public += "WIN32"_def;

    vpx.Variables["ARCH_X86"] = 0;
    vpx.Variables["ARCH_X86_64"] = 0;
    if (vpx.getBuildSettings().TargetOS.Arch != ArchType::x86_64)
    {
        vpx.Variables["ARCH_X86"] = 1;
        vpx.Variables["asm"] = "win32";
    }
    else
    {
        vpx.Variables["ARCH_X86_64"] = 1;
        vpx.Variables["asm"] = "win64";
    }

    vpx.Variables["CONFIG_GCC"] = 0;
    vpx.Variables["CONFIG_MSVS"] = 0;
    vpx.Variables["CONFIG_BIG_ENDIAN"] = vpx.Variables["WORDS_BIGENDIAN"];

    if (vpx.getCompilerType() == CompilerType::GNU)
        vpx.Variables["CONFIG_GCC"] = 1;

    if (vpx.getCompilerType() == CompilerType::MSVC)
        vpx.Variables["CONFIG_MSVS"] = 1;

    vpx.writeFileOnce("vpx_config.h.in", R"(
        #pragma once

        #define INLINE inline
        #define RESTRICT

        #cmakedefine01 ARCH_X86
        #cmakedefine01 ARCH_X86_64

        #define ARCH_ARM 0
        #define ARCH_MIPS 0
        #define ARCH_PPC 0
        #define HAVE_NEON 0
        #define HAVE_NEON_ASM 0
        #define HAVE_MIPS32 0
        #define HAVE_DSPR2 0
        #define HAVE_MSA 0
        #define HAVE_MIPS64 0
        #define HAVE_MMX 1
        #define HAVE_SSE 1
        #define HAVE_SSE2 1
        #define HAVE_SSE3 1
        #define HAVE_SSSE3 1
        #define HAVE_SSE4_1 1
        #define HAVE_AVX 1
        #define HAVE_AVX2 1
        #define HAVE_AVX512 0
        #define HAVE_VSX 0
        #define HAVE_MMI 0
        #define HAVE_VPX_PORTS 1

        #cmakedefine01 CONFIG_GCC
        #cmakedefine01 CONFIG_MSVS
        #cmakedefine01 CONFIG_BIG_ENDIAN

        #define CONFIG_DEPENDENCY_TRACKING 1
        #define CONFIG_EXTERNAL_BUILD 0
        #define CONFIG_INSTALL_DOCS 0
        #define CONFIG_INSTALL_BINS 1
        #define CONFIG_INSTALL_LIBS 1
        #define CONFIG_INSTALL_SRCS 0
        #define CONFIG_DEBUG 0
        #define CONFIG_GPROF 0
        #define CONFIG_GCOV 0
        #define CONFIG_RVCT 0
        #define CONFIG_PIC 0
        #define CONFIG_CODEC_SRCS 0
        #define CONFIG_DEBUG_LIBS 0
        #define CONFIG_DEQUANT_TOKENS 0
        #define CONFIG_DC_RECON 0
        #define CONFIG_RUNTIME_CPU_DETECT 1
        #define CONFIG_POSTPROC 1
        #define CONFIG_VP9_POSTPROC 1
        #define CONFIG_MULTITHREAD 1
        #define CONFIG_INTERNAL_STATS 1
        #define CONFIG_VP8_ENCODER 1
        #define CONFIG_VP8_DECODER 1
        #define CONFIG_VP9_ENCODER 1
        #define CONFIG_VP9_DECODER 1
        #define CONFIG_VP8 1
        #define CONFIG_VP9 1
        #define CONFIG_ENCODERS 1
        #define CONFIG_DECODERS 1
        #define CONFIG_STATIC_MSVCRT 0
        #define CONFIG_SPATIAL_RESAMPLING 1
        #define CONFIG_REALTIME_ONLY 0
        #define CONFIG_ONTHEFLY_BITPACKING 0
        #define CONFIG_ERROR_CONCEALMENT 1
        #define CONFIG_SMALL 0
        #define CONFIG_POSTPROC_VISUALIZER 1
        #define CONFIG_OS_SUPPORT 1
        #define CONFIG_UNIT_TESTS 1
        #define CONFIG_WEBM_IO 1
        #define CONFIG_LIBYUV 1
        #define CONFIG_DECODE_PERF_TESTS 0
        #define CONFIG_ENCODE_PERF_TESTS 0
        #define CONFIG_MULTI_RES_ENCODING 1
        #define CONFIG_TEMPORAL_DENOISING 1
        #define CONFIG_VP9_TEMPORAL_DENOISING 1
        #define CONFIG_CONSISTENT_RECODE 0
        #define CONFIG_COEFFICIENT_RANGE_CHECKING 0
        #define CONFIG_VP9_HIGHBITDEPTH 1
        #define CONFIG_BETTER_HW_COMPATIBILITY 0
        #define CONFIG_EXPERIMENTAL 0
        #define CONFIG_SIZE_LIMIT 0
        #define CONFIG_ALWAYS_ADJUST_BPM 0
        #define CONFIG_FP_MB_STATS 0
        #define CONFIG_EMULATE_HARDWARE 0
    )");
    vpx.configureFile("vpx_config.h.in", "vpx_config.h");

    vpx.writeFileOnce("vpx_config.asm.in", R"(
        ARCH_ARM equ 0
        ARCH_MIPS equ 0
        ARCH_X86 equ ${ARCH_X86}
        ARCH_PPC equ 0
        HAVE_NEON equ 0
        HAVE_NEON_ASM equ 0
        HAVE_MIPS32 equ 0
        HAVE_DSPR2 equ 0
        HAVE_MSA equ 0
        HAVE_MIPS64 equ 0
        HAVE_MMX equ 1
        HAVE_SSE equ 1
        HAVE_SSE2 equ 1
        HAVE_SSE3 equ 1
        HAVE_SSSE3 equ 1
        HAVE_SSE4_1 equ 1
        HAVE_AVX equ 1
        HAVE_AVX2 equ 1
        HAVE_AVX512 equ 0
        HAVE_VSX equ 0
        HAVE_MMI equ 0
        HAVE_VPX_PORTS equ 1
        HAVE_PTHREAD_H equ ${HAVE_PTHREAD_H}
        HAVE_UNISTD_H equ ${HAVE_UNISTD_H}
        CONFIG_DEPENDENCY_TRACKING equ 1
        CONFIG_EXTERNAL_BUILD equ 0
        CONFIG_INSTALL_DOCS equ 0
        CONFIG_INSTALL_BINS equ 1
        CONFIG_INSTALL_LIBS equ 1
        CONFIG_INSTALL_SRCS equ 0
        CONFIG_DEBUG equ 0
        CONFIG_GPROF equ 0
        CONFIG_GCOV equ 0
        CONFIG_RVCT equ 0
        CONFIG_GCC equ ${CONFIG_GCC}
        CONFIG_MSVS equ ${CONFIG_MSVS}
        CONFIG_PIC equ 0
        CONFIG_BIG_ENDIAN equ ${CONFIG_BIG_ENDIAN}
        CONFIG_CODEC_SRCS equ 0
        CONFIG_DEBUG_LIBS equ 0
        CONFIG_DEQUANT_TOKENS equ 0
        CONFIG_DC_RECON equ 0
        CONFIG_RUNTIME_CPU_DETECT equ 1
        CONFIG_POSTPROC equ 1
        CONFIG_VP9_POSTPROC equ 1
        CONFIG_MULTITHREAD equ 1
        CONFIG_INTERNAL_STATS equ 1
        CONFIG_VP8_ENCODER equ 1
        CONFIG_VP8_DECODER equ 1
        CONFIG_VP9_ENCODER equ 1
        CONFIG_VP9_DECODER equ 1
        CONFIG_VP8 equ 1
        CONFIG_VP9 equ 1
        CONFIG_ENCODERS equ 1
        CONFIG_DECODERS equ 1
        CONFIG_STATIC_MSVCRT equ 0
        CONFIG_SPATIAL_RESAMPLING equ 1
        CONFIG_REALTIME_ONLY equ 0
        CONFIG_ONTHEFLY_BITPACKING equ 0
        CONFIG_ERROR_CONCEALMENT equ 1
        CONFIG_SMALL equ 0
        CONFIG_POSTPROC_VISUALIZER equ 1
        CONFIG_OS_SUPPORT equ 1
        CONFIG_UNIT_TESTS equ 1
        CONFIG_WEBM_IO equ 1
        CONFIG_LIBYUV equ 1
        CONFIG_DECODE_PERF_TESTS equ 0
        CONFIG_ENCODE_PERF_TESTS equ 0
        CONFIG_MULTI_RES_ENCODING equ 1
        CONFIG_TEMPORAL_DENOISING equ 1
        CONFIG_VP9_TEMPORAL_DENOISING equ 1
        CONFIG_CONSISTENT_RECODE equ 0
        CONFIG_COEFFICIENT_RANGE_CHECKING equ 0
        CONFIG_VP9_HIGHBITDEPTH equ 1
        CONFIG_BETTER_HW_COMPATIBILITY equ 0
        CONFIG_EXPERIMENTAL equ 0
        CONFIG_SIZE_LIMIT equ 0
        CONFIG_ALWAYS_ADJUST_BPM equ 0
        CONFIG_FP_MB_STATS equ 0
        CONFIG_EMULATE_HARDWARE equ 0
    )");
    vpx.configureFile("vpx_config.asm.in", "vpx_config.asm");
    vpx -= "vpx_config.asm";

    if (vpx.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        if (vpx.getBuildSettings().TargetOS.Arch != ArchType::x86_64)
            vpx -= ".*_x86_64.asm"_rr;
    }

    vpx.writeFileOnce("vpx_version.h",
        "#define VERSION_MAJOR " + vpx.Variables["PACKAGE_VERSION_MAJOR"].toString() + "\n" +
        "#define VERSION_MINOR " + vpx.Variables["PACKAGE_VERSION_MINOR"].toString() + "\n" +
        "#define VERSION_PATCH " + vpx.Variables["PACKAGE_VERSION_PATCH"].toString() + "\n" +
        R"(
        // This file is generated. Do not edit.
        #define VERSION_EXTRA  ""
        #define VERSION_PACKED ((VERSION_MAJOR<<16)|(VERSION_MINOR<<8)|(VERSION_PATCH))
        #define VERSION_STRING_NOSP ""
        #define VERSION_STRING      ""
    )");

    // generated configs are at
    // https://github.com/chromium/chromium/tree/master/third_party/libvpx/source/config/

    for (auto f : {
        "vp8_rtcd.h",
        "vp9_rtcd.h",
        "vpx_dsp_rtcd.h",
        "vpx_scale_rtcd.h",
        })
    {
        download_file("https://raw.githubusercontent.com/chromium/chromium/master/third_party/libvpx/source/config/win/x64/"s + f, vpx.BinaryDir / f);
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("vpx");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
