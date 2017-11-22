void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &freetype = s.addTarget<LibraryTarget>("freetype", "2.8.1");
    std::string patch;
    if (freetype.getPackage().version.Patch.IsSet())
        patch = "-" + std::to_string(freetype.getPackage().version.Patch);
    freetype.Source = RemoteFile("http://git.savannah.gnu.org/cgit/freetype/freetype2.git/snapshot/VER-{M}-{m}" + patch + ".tar.gz");

    freetype.ApiName = "SW_FREETYPE_LIBRARY_API";
    freetype.setChecks("freetype");

    freetype +=
        "builds/.*\\.c"_rr,
        "builds/.*\\.in"_rr,
        "include/.*"_rr,
        "src/.*\\.c"_rr,
        "src/.*\\.cc"_rr,
        "src/.*\\.h"_rr;

    freetype.Private += "FT2_BUILD_LIBRARY"_d;
    freetype.Private += sw::Shared, "freetype_EXPORTS"_d;

    freetype.Public += "pub.cppan2.demo.bzip2-1"_dep;
    freetype.Public += "pub.cppan2.demo.png-1"_dep;

    freetype.pushBackToFileOnce("include/freetype/config/ftconfig.h", R"(
    #ifdef FT_BASE
    #undef FT_BASE
    #ifdef __cplusplus
    #define FT_BASE(x) extern "C" SW_FREETYPE_LIBRARY_API x
    #else
    #define FT_BASE(x) extern SW_FREETYPE_LIBRARY_API x
    #endif
    #endif

    #ifdef FT_EXPORT
    #undef FT_EXPORT
    #ifdef __cplusplus
    #define FT_EXPORT(x) extern "C" SW_FREETYPE_LIBRARY_API x
    #else
    #define FT_EXPORT(x) extern SW_FREETYPE_LIBRARY_API x
    #endif
    #endif

)");

    if (freetype.Settings.TargetOS.Type != OSType::Windows)
    {
        const auto cfg = freetype.SourceDir / "builds/unix/ftconfig.in";
        if (freetype.Variables["HAVE_UNISTD_H"] == "1")
            freetype.replaceInFileOnce(cfg, "#undef +(HAVE_UNISTD_H)", "#define \\1");
        if (freetype.Variables["HAVE_FCNTL_H"] == "1")
            freetype.replaceInFileOnce(cfg, "#undef +(HAVE_FCNTL_H)", "#define \\1");
        if (freetype.Variables["HAVE_STDINT_H"] == "1")
            freetype.replaceInFileOnce(cfg, "#undef +(HAVE_STDINT_H)", "#define \\1");
        freetype.replaceInFileOnce(cfg, "/undef ", "#undef ");
        freetype.configureFile(cfg, "include/freetype/config/ftconfig.h");
    }
    else
    {
        freetype.configureFile(freetype.SourceDir / "include/freetype/config/ftconfig.h", "include/freetype/config/ftconfig.h");
    }

    const auto ftoption = freetype.SourceDir / "include/freetype/config/ftoption.h";
    freetype.replaceInFileOnce(ftoption, "/\\* +(#define +FT_CONFIG_OPTION_SYSTEM_ZLIB) +\\*/", "\\1");
    freetype.replaceInFileOnce(ftoption, "/\\* +(#define +FT_CONFIG_OPTION_USE_BZIP2) +\\*/", "\\1");
    freetype.replaceInFileOnce(ftoption, "/\\* +(#define +FT_CONFIG_OPTION_USE_PNG) +\\*/", "\\1");
    freetype.configureFile(ftoption, "include/freetype/config/ftoption.h");

    freetype -= ".*"_rr;

    if (freetype.Settings.TargetOS.Type == OSType::Windows)
        freetype += "builds/windows/ftdebug.c";
    //else if (freetype.Settings.TargetOS.Type == OSType::WinCE)
    else
        freetype += "builds/base/ftdebug.c";

    freetype += "src/autofit/autofit.c",
        "src/base/ftbase.c",
        "src/base/ftbbox.c",
        "src/base/ftbdf.c",
        "src/base/ftbitmap.c",
        "src/base/ftcid.c",
        "src/base/ftfntfmt.c",
        "src/base/ftfstype.c",
        "src/base/ftgasp.c",
        "src/base/ftglyph.c",
        "src/base/ftgxval.c",
        "src/base/ftinit.c",
        "src/base/ftlcdfil.c",
        "src/base/ftmm.c",
        "src/base/ftotval.c",
        "src/base/ftpatent.c",
        "src/base/ftpfr.c",
        "src/base/ftstroke.c",
        "src/base/ftsynth.c",
        "src/base/ftsystem.c",
        "src/base/fttype1.c",
        "src/base/ftwinfnt.c",
        "src/bdf/bdf.c",
        "src/bzip2/ftbzip2.c",
        "src/cache/ftcache.c",
        "src/cff/cff.c",
        "src/cid/type1cid.c",
        "src/gzip/ftgzip.c",
        "src/lzw/ftlzw.c",
        "src/pcf/pcf.c",
        "src/pfr/pfr.c",
        "src/psaux/psaux.c",
        "src/pshinter/pshinter.c",
        "src/psnames/psnames.c",
        "src/raster/raster.c",
        "src/sfnt/sfnt.c",
        "src/smooth/smooth.c",
        "src/truetype/truetype.c",
        "src/type1/type1.c",
        "src/type42/type42.c",
        "src/winfonts/winfnt.c";
}

void check(Checker &c)
{
    auto &s = c.addSet("freetype");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
