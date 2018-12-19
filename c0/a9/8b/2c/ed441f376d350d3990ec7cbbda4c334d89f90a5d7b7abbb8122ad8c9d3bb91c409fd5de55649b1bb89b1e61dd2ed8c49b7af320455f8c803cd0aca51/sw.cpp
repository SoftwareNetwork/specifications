void build(Solution &s)
{
    auto &png = s.addTarget<LibraryTarget>("glennrp.png", "1.6.36");
    png += Git("https://github.com/glennrp/libpng", "v{v}");

    png.ApiName = "PNG_IMPEXP";
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

    png.Public += "org.sw.demo.madler.zlib-1"_dep;
    png.configureFile("scripts/pnglibconf.h.prebuilt", "pnglibconf.h");
}
