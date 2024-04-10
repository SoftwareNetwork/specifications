#include <primitives/overload.h>

void build(Solution &s)
{
    auto &vpx = s.addLibrary("webmproject.vpx", "1.14.0");
    vpx += Git("https://github.com/webmproject/libvpx", "v{M}.{m}.{p}");

#if SW_CPP_DRIVER_API_VERSION >= 2
    vpx.addRuleDependency("asm", "org.sw.demo.yasm"_dep);
#else
    vpx.setExtensionProgram(".asm", "org.sw.demo.yasm"_dep);
#endif

    vpx.ExportAllSymbols = true;
    vpx.setChecks("vpx");

    vpx += c99;

    vpx +=
        "third_party/x86inc/x86inc.asm",
        "vp8/.*"_rr,
        "vp9/.*"_rr,
        "vpx/.*"_rr,
        "vpx_.*"_rr;
    vpx -=
        ".*/arm/.*"_rr,
        ".*/loongarch/.*"_rr,
        ".*/mips/.*"_rr,
        ".*/ppc/.*"_rr,
        "vpx_ports/.*"_rr;
    vpx +=
        "vpx_ports/.*\\.asm"_rr;

    vpx += "org.sw.demo.chromium.libyuv-master"_dep;

    vpx -= ".*avx.*"_rr;
    vpx -= ".*sse.*"_rr;
    vpx -= ".*mmx.*"_rr;

    // CONFIG_INTERNAL_STATS
    vpx -= "vpx_dsp/.*ssim.*"_rr;
    vpx -= "vpx_dsp/psnrhvs.c";

    //
    //vpx -= "vp8/decoder/error_concealment.c";

    // CONFIG_VP9_HIGHBITDEPTH
    /*{
    //vpx -= "vpx_dsp/x86/highbd_.*"_rr;
    //vpx -= "vp9/encoder/x86/highbd_.*"_rr;
    //vpx -= ".*temporal_filter.c"_rr;
    vpx -= "vp9/common/x86/vp9_highbd_iht4x4_add_sse4.c";
    vpx -= "vp9/common/x86/vp9_highbd_iht8x8_add_sse4.c";
    vpx -= "vp9/common/x86/vp9_highbd_iht16x16_add_sse4.c";
    vpx -= "vp9/encoder/x86/highbd_temporal_filter_sse4.c";
    vpx -= "vpx_dsp/x86/highbd_convolve_avx2.c";
    vpx -= "vpx_dsp/x86/highbd_idct4x4_add_sse2.c";
    vpx -= "vpx_dsp/x86/highbd_idct8x8_add_sse2.c";
    vpx -= "vpx_dsp/x86/highbd_idct16x16_add_sse2.c";
    vpx -= "vpx_dsp/x86/highbd_idct32x32_add_sse2.c";
    vpx -= "vpx_dsp/x86/highbd_idct4x4_add_sse4.c";
    vpx -= "vpx_dsp/x86/highbd_idct8x8_add_sse4.c";
    vpx -= "vpx_dsp/x86/highbd_idct16x16_add_sse4.c";
    vpx -= "vpx_dsp/x86/highbd_idct32x32_add_sse4.c";
    }*/

    struct setting {
        String key;
        std::variant<int, String> value;
        StringSet regex_add;
        StringSet regex_remove;
    };

    std::vector<setting> settings {
        {{"VPX_ARCH_ARM"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::aarch64},
        {{"VPX_ARCH_MIPS"}, 0},
        {{"VPX_ARCH_PPC"}, 0},
        {{"HAVE_MMX"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::x86, {".*mmx.*"}},
        {{"VPX_ARCH_X86"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::x86},
        {{"VPX_ARCH_X86_64"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::x86_64,
            {"vpx_ports/emms_mmx.asm"},
            {"vpx_ports/emms_mmx.c","vp8/common/x86/vp8_asm_stubs.c"}
        },
        {{"HAVE_NEON"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::aarch64},
        {{"HAVE_NEON_ASM"}, 0},
        {{"HAVE_MIPS32"}, 0},
        {{"HAVE_DSPR2"}, 0},
        {{"HAVE_MSA"}, 0},
        {{"HAVE_MIPS64"}, 0},
        {{"HAVE_SSE"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::x86_64},
        {{"HAVE_SSE2"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::x86_64},
        {{"HAVE_SSE3"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::x86_64},
        {{"HAVE_SSSE3"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::x86_64},
        {{"HAVE_SSE4_1"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::x86_64, {".*sse.*"}},
        {{"HAVE_AVX"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::x86_64},
        {{"HAVE_AVX2"}, vpx.getBuildSettings().TargetOS.Arch == ArchType::x86_64, {".*avx[^5].*"}},
        {{"HAVE_AVX512"}, 0, {".*avx5.*"}},
        {{"HAVE_VSX"}, 0},
        {{"HAVE_MMI"}, 0},
        {{"HAVE_VPX_PORTS"}, 1},

        {{"HAVE_PTHREAD_H"}, vpx.getBuildSettings().TargetOS.Type != OSType::Windows},
        {{"HAVE_UNISTD_H"}, vpx.getBuildSettings().TargetOS.Type != OSType::Windows},

        {{"INLINE"}, "inline"},
        {{"RESTRICT"}, ""},

        {{"CONFIG_DEPENDENCY_TRACKING"}, 1},
        {{"CONFIG_GCC"}, vpx.getCompilerType() != CompilerType::MSVC},
        {{"CONFIG_MSVC"}, vpx.getCompilerType() == CompilerType::MSVC},
        {{"CONFIG_MULTITHREAD"}, 1},
        {{"CONFIG_VP8_ENCODER"}, 1},
        {{"CONFIG_VP8_DECODER"}, 1},
        {{"CONFIG_VP9_ENCODER"}, 1},
        {{"CONFIG_VP9_DECODER"}, 1},
        {{"CONFIG_VP8"}, 1},
        {{"CONFIG_VP9"}, 1},
        {{"CONFIG_ENCODERS"}, 1},
        {{"CONFIG_DECODERS"}, 1},
        {{"CONFIG_SPATIAL_RESAMPLING"}, 1},
        {{"CONFIG_OS_SUPPORT"}, 1},
        {{"CONFIG_TEMPORAL_DENOISING"}, 1},
        {{"CONFIG_STATIC"}, vpx.getBuildSettings().Native.LibrariesType == LibraryType::Static},
        {{"CONFIG_SHARED"}, vpx.getBuildSettings().Native.LibrariesType != LibraryType::Static},
        {{"CONFIG_REALTIME_ONLY"}, 0},
        {{"CONFIG_INTERNAL_STATS"}, 0},
        {{"CONFIG_POSTPROC"}, 1},
        {{"CONFIG_VP9_POSTPROC"}, 1},
        {{"CONFIG_POSTPROC_VISUALIZER"}, 1},
        {{"CONFIG_MULTI_RES_ENCODING"}, 1},
        {{"CONFIG_ERROR_CONCEALMENT"}, 1},
        {{"CONFIG_VP9_TEMPORAL_DENOISING"}, 1},
        {{"CONFIG_PIC"}, 0},
        {{"CONFIG_VP9_HIGHBITDEPTH"}, 0, {}, {".*highbd.*", "vpx_dsp/x86/highbd.*"}},
    };

    String config_h, config_txt, config_asm;
    config_h += "#pragma once\n\n";
    for (auto &&s : settings) {
        config_h += "#define " + s.key + " ";
        std::visit(overload(
            [&](int &v){config_h += std::to_string(v);
            if (v) {
                for (auto &&r : s.regex_add)
                    vpx += FileRegex{vpx.SourceDir, r, true};
                for (auto &&r : s.regex_remove)
                    vpx -= FileRegex{vpx.SourceDir, r, true};
            }
            },[&](std::string &v){config_h += v;}), s.value);
        config_h += "\n";

        config_txt += s.key + "=";
        std::visit(overload([&](int &v){config_txt += v ? "yes" : "no";},[&](std::string &v){config_txt += v;}), s.value);
        config_txt += "\n";

        std::visit(overload([&](int &v){config_asm += s.key + " equ " + std::to_string(v);},[&](std::string &v){}), s.value);
        config_asm += "\n";
    }
    vpx.writeFileOnce("vpx_config.h", config_h);
    vpx.writeFileOnce("vpx_config.asm", config_asm);
    vpx.writeFileOnce("config.txt", config_txt);

    vpx -= "vpx_dsp/x86/highbd.*"_rr;
    vpx -= "vpx_dsp/x86/.*highbd.*"_rr;
    vpx -= "vp9/.*highbd.*"_rr; //vp9_highbd_iht16x16_add_sse4.c

    // CONFIG_VP9_TEMPORAL_DENOISING
    //vpx -= "vp9/encoder/vp9_denoiser.c";
    //vpx -= "vp9/encoder/x86/vp9_denoiser_sse2.c";

    //if (vpx.getBuildSettings().TargetOS.Type == OSType::Windows)
        //vpx.Public += "WIN32"_def;

    String arch, os;

    vpx.Variables["ARCH_X86"] = 0;
    vpx.Variables["ARCH_X86_64"] = 0;
    if (vpx.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
    {
        vpx -= ".*/arm/.*"_rr; // some sse_neon files are enabled with sse
        vpx.Variables["ARCH_X86_64"] = 1;
        if (vpx.getBuildSettings().TargetOS.Type == OSType::Windows) {
            vpx.Variables["asm"] = "win64";
            //vpx.CompileOptions.push_back("/arch:);
        } else if (vpx.getBuildSettings().TargetOS.isApple()) {
            vpx.Variables["asm"] = "macho";
            for (auto &&[p,f] : vpx[".*\\.cc?"_rr]) {
                f->args.push_back("-mavx2");
            }
            vpx -= "vp8/common/x86/loopfilter_x86.c";
            vpx -= "vp9/encoder/x86/vp9_dct_intrin_sse2.c";
            vpx -= "vpx_dsp/x86/subtract_avx2.c";
            vpx -= "vpx_dsp/x86/avg_pred_avx2.c";
        } else {
            vpx.Variables["asm"] = "elf64";
            for (auto &&[p,f] : vpx[".*\\.cc?"_rr]) {
                f->args.push_back("-mavx2");
            }
            vpx -=
                "vp9/encoder/x86/vp9_dct_intrin_sse2.c",
                "vp8/common/x86/loopfilter_x86.c",
                "vpx_dsp/x86/subtract_avx2.c",
                "vpx_dsp/x86/avg_pred_avx2.c"
                ;
            vpx += "pthread"_slib;
        }
        arch = "x64";
    }
    else if (vpx.getBuildSettings().TargetOS.Arch == ArchType::aarch64)
    {
        vpx.Variables["ARCH_ARM"] = 1;
        vpx.Variables["asm"] = "arm64";
        arch = "arm64";
        vpx -= ".*avx.*"_rr;
        vpx -= ".*sse.*"_rr;
        vpx += ".*arm.*"_rr;
        vpx += ".*neon.*"_rr;
        vpx -= ".*asm.*"_rr;
        vpx -= ".*mmx.*"_rr;
        vpx -= ".*.highbd.*"_rr;

        vpx += "vpx_ports/aarch64_cpudetect.c";
        vpx += "vpx_dsp/sse.c";

        vpx -= "vp9/encoder/arm/neon/vp9_denoiser_neon.c";
        vpx -= "vp9/encoder/arm/neon/vp9_dct_neon.c";
        vpx -= "vpx_dsp/arm/vpx_convolve_neon.c";
        vpx -= "vpx_dsp/arm/vpx_convolve8_neon.c";
        vpx -= "vpx_dsp/arm/subpel_variance_neon.c";
        vpx -= "vpx_dsp/arm/vpx_convolve8_neon_i8mm.c";
        vpx -= "vpx_dsp/arm/vpx_convolve8_neon_dotprod.c";
        vpx -= "vpx_dsp/arm/vpx_convolve_neon_i8mm.c";
        vpx -= "vpx_dsp/arm/vpx_convolve_neon_dotprod.c";
        if (vpx.getBuildSettings().TargetOS.Type == OSType::Windows) {
            vpx.patch("vp9/encoder/arm/neon/vp9_diamond_search_sad_neon.c", "__attribute__((aligned(16)))", "");
        }
    }
    else
    {
        vpx.Variables["ARCH_X86"] = 1;
        vpx.Variables["asm"] = "win32";
        arch = "x86";
    }

    if (vpx.getBuildSettings().TargetOS.Type == OSType::Windows)
        os = "win";
    else if (vpx.getBuildSettings().TargetOS.Type == OSType::Macos)
    {
        os = "mac";
        if (vpx.getBuildSettings().TargetOS.Type == OSType::IOS || vpx.getBuildSettings().TargetOS.Arch == ArchType::aarch64)
            os = "ios";
    }
    else
    {
        os = "linux";
        for (auto &&[p,f] : vpx[".*\\.c"_rr]) {
            f->args.push_back("-march=native");
        }
    }

    // CONFIG_REALTIME_ONLY
    {
        vpx -= "vp9/encoder/x86/highbd_temporal_filter_sse4.c";
        vpx -= "vp9/simple_encode.cc";
    }

    vpx.Variables["CONFIG_GCC"] = 0;
    vpx.Variables["CONFIG_MSVS"] = 0;
    vpx.Variables["CONFIG_BIG_ENDIAN"] = vpx.Variables["WORDS_BIGENDIAN"];

    if (vpx.getCompilerType() == CompilerType::GNU)
        vpx.Variables["CONFIG_GCC"] = 1;
    if (vpx.getCompilerType() == CompilerType::MSVC)
        vpx.Variables["CONFIG_MSVS"] = 1;

    if (vpx.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        if (vpx.getBuildSettings().TargetOS.Arch != ArchType::x86_64)
            vpx -= ".*_x86_64.asm"_rr;
    }
    if (vpx.getBuildSettings().TargetOS.isApple())
    {
        vpx -= "vp8/vp8_ratectrl_rtc.cc";
        vpx -= "vp9/ratectrl_rtc.cc";
    }

    vpx -= "test/.*"_rr;
    vpx -= "third_party/.*"_rr;

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

    {
        auto run = [&](auto &&prog) {
            auto cmd = [&](auto &&sym, auto &&file, auto &&out) {
                auto arch = "--arch=X64_86";
                if (vpx.getBuildSettings().TargetOS.Arch == ArchType::aarch64)
                    arch = "--arch=ARM64";
                vpx.addCommand()
                    << cmd::prog(prog)
                    << cmd::in("build/make/rtcd.pl")
                    << arch
                    << "--sym" << sym
                    << "--config" << cmd::in("config.txt")
                    << cmd::in(file)
                    << cmd::std_out(out);
            };
            cmd("vp8_rtcd", "vp8/common/rtcd_defs.pl", "vp8_rtcd.h");
            cmd("vp9_rtcd", "vp9/common/vp9_rtcd_defs.pl", "vp9_rtcd.h");
            cmd("vpx_dsp_rtcd", "vpx_dsp/vpx_dsp_rtcd_defs.pl", "vpx_dsp_rtcd.h");
            cmd("vpx_scale_rtcd", "vpx_scale/vpx_scale_rtcd.pl", "vpx_scale_rtcd.h");
        };
        auto perl = "org.sw.demo.perl.perl"_dep;
        vpx -= perl;
        if (vpx.getBuildSettings().TargetOS.Type == OSType::Windows) {
            run(perl);
        } else {
            run("perl");
        }
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
