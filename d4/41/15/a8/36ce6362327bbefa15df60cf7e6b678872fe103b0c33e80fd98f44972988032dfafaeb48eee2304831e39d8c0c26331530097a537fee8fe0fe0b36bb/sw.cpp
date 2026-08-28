void build(Solution &s) {
    auto &p = s.addProject("google", "main");
    p += Git("https://github.com/google/skia");

    auto &skcms = p.addLibrary("modules.skcms");
    {
        auto &t = skcms;
        t += cpp20;
        t += "SKCMS_API"_api;
        t += "modules/.*\.cc"_rr;
        t += "modules/.*\.h"_rr;
    }

    auto &sk = p.addLibrary("skia");
    {
        auto &t = sk;
        t += cpp20;
        t += "SK_API"_api;

        t += "include/.*"_rr;
        t += "src/codec/.*"_rr;
        t += "src/core/.*"_rr;
        t += "src/effects/.*"_rr;
        t += "src/image/.*"_rr;
        t += "src/shaders/.*"_rr;
        t += "src/text/.*"_rr;
        t += "src/xml/.*"_rr;
        t += "src/pathops/.*"_rr;
        t += "src/sfnt/.*"_rr;
        t += "src/opts/.*"_rr;
        t += "src/lazy/.*"_rr;
        t += "src/partition_alloc/.*"_rr;
        t += "src/capture/.*"_rr; // -=?
        t -= "src/gpu/.*"_rr;

        t += "src/utils/.*"_r;
        t += "src/utils/win/.*"_r;
        t -= "src/utils/SkGetExecutablePath_linux.cpp";
        t -= "src/utils/SkGetExecutablePath_mac.cpp";
        //t -= "src/utils/SkGetExecutablePath_win.cpp";

        //
        t += "src/ports/.*"_rr;
        t -= "src/ports/SkOSFile_posix.cpp";

        t -= "src/ports/SkFontMgr_fontconfig.cpp";
        t -= "src/ports/SkFontMgr_FontConfigInterface.cpp";
        t -= "src/ports/SkFontConfigInterface.cpp";
        t -= "src/ports/SkFontConfigInterface_direct.cpp";
        t -= "src/ports/SkFontConfigInterface_direct_factory.cpp";
        //t.Public += "org.sw.demo.freedesktop.fontconfig.fontconfig"_dep;

        t -= "src/ports/SkFontHost_FreeType.cpp";
        t -= "src/ports/SkFontHost_FreeType_common.cpp";
        //t.Public += "org.sw.demo.freetype"_dep;

        t -= "src/ports/SkImageGeneratorCG.cpp"; // mac

        t -= "src/ports/SkFontMgr_android.cpp";
        t -= "src/ports/SkImageEncoder_NDK.cpp";
        t -= "src/ports/SkFontMgr_android_ndk.cpp";
        t -= "src/ports/SkNDKConversions.cpp";
        t -= "src/ports/SkImageGeneratorNDK.cpp";
        t -= "src/ports/SkFontMgr_android_parser.cpp";

        t -= "src/ports/SkFontMgr_fuchsia.cpp";

        t -= "src/ports/SkFontMgr_custom.*"_r;

        t -= "src/ports/SkMemory_mozalloc.cpp";

        // rust
        t -= "src/ports/SkFontScanner_fontations.cpp";
        t -= "src/ports/SkFontMgr_fontations_empty.cpp";
        t -= "src/ports/SkTypeface_fontations.cpp";

        t += "src/codec/.*"_rr;
        t -= "src/codec/SkBmpRustCodec.cpp";
        t -= "src/codec/SkPngRustCodec.cpp";
        t -= "src/codec/SkBmpRustDecoder.cpp";
        t -= "src/codec/SkPngRustDecoder.cpp";
        t -= "src/codec/SkCodecColorProfileRust.cpp";
        t -= "src/codec/SkWuffsCodec.cpp";
        t -= "src/codec/SkCrabbyAvifCodec.cpp";
        t -= "src/codec/SkAvifCodec.cpp";
        t -= "src/codec/SkRawCodec.cpp";
        t -= "src/codec/SkJpegCodec.cpp"; // jpeg turbo?

        //
        t += "src/sksl/.*"_rr;
        t -= "src/sksl/codegen/SkSLWGSLValidator.cpp";
        t -= "src/sksl/codegen/SkSLSPIRVtoHLSL.cpp";
        t -= "src/sksl/codegen/SkSLSPIRVValidator.cpp";
        t -= "src/sksl/codegen/SkSLHLSLCodeGenerator.cpp";
        t -= "src/sksl/SkSLModuleDataDefault.cpp";
        t -= "src/sksl/SkSLGraphiteModules.cpp";

        t += "SK_CODEC_DECODES_PNG_WITH_LIBPNG"_def;
        //t += "SK_CODEC_DECODES_JPEG"_def; // static only
        t += "SK_CODEC_DECODES_WEBP"_def;
        //t += "SK_CODEC_DECODES_GIF"_def;
        t += "SK_CODEC_DECODES_ICO"_def;
        t += "SK_CODEC_DECODES_BMP"_def;
        t += "SK_CODEC_DECODES_WBMP"_def;
        //t += "SK_CODEC_DECODES_AVIF"_def;
        //t += "SK_CODEC_DECODES_JPEGXL"_def;

        t.Public += "."_idir;
        //t.Public += "include"_idir;

        t.Public += skcms;

        t.Public += "org.sw.demo.webmproject.webp"_dep;
        t.Public += "org.sw.demo.jpeg"_dep;
        t.Public += "org.sw.demo.jxl"_dep;
        t.Public += "org.sw.demo.glennrp.png"_dep;
        t.Public += "org.sw.demo.expat"_dep;

        // win32
        t.Public += "WIN32_LEAN_AND_MEAN"_def;
        t.Public += "NOMINMAX"_def;

        t += "Gdi32.lib"_slib;
        t += "ole32.lib"_slib;
        t += "uuid.lib"_slib;
        t += "Usp10.lib"_slib;
        t += "User32.lib"_slib;

        // win32 only?
        t.patch("src/codec/SkWebpCodec.h", "extern \"C\" {", "#include \"webp/demux.h\"\nextern \"C\"  {");
        t.patch("src/codec/SkWebpCodec.h", "struct WebPDemuxer;", "");
        t.patch("src/codec/SkWebpCodec.h", "void WebPDemuxDelete(WebPDemuxer* dmux);", "");
    }
}
