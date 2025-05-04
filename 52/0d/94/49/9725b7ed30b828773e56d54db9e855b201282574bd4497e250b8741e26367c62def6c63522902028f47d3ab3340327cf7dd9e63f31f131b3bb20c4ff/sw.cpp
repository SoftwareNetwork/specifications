void build(Solution &s)
{
    auto &png = s.addTarget<LibraryTarget>("glennrp.png", "1.6.48");
    png += Git("https://github.com/glennrp/libpng", "v{v}");

    png.ApiName = "PNG_IMPEXP";
    png -= "arm/.*"_rr;
    png +=
        "png.c",
        "png.h",
        "pngconf.h",
        "pngdebug.h",
        "pngerror.c",
        "pngget.c",
        "pnginfo.h",
        "pngmem.c",
        "pngpread.c",
        "pngpriv.h",
        "pngread.c",
        "pngrio.c",
        "pngrtran.c",
        "pngrutil.c",
        "pngset.c",
        "pngstruct.h",
        "pngtrans.c",
        "pngwio.c",
        "pngwrite.c",
        "pngwtran.c",
        "pngwutil.c",
        "scripts/pnglibconf.h.prebuilt";
    if (png.getBuildSettings().TargetOS.Arch == ArchType::aarch64) {
        png += "arm/.*"_rr;
    }

    png.Public += "org.sw.demo.madler.zlib"_dep;
    png.configureFile("scripts/pnglibconf.h.prebuilt", "pnglibconf.h");
}
