void build(Solution &s)
{
    auto &png = s.addTarget<LibraryTarget>("glennrp.png", "1.0.5");
    png += Git("https://github.com/glennrp/libpng", "v{v}");

    png +=
        "png.c",
        "png.h",
        "pngasmrd.h",
        "pngconf.h",
        "pngerror.c",
        "pngget.c",
        "pngmem.c",
        "pngpread.c",
        "pngread.c",
        "pngrio.c",
        "pngrtran.c",
        "pngrutil.c",
        "pngset.c",
        "pngtrans.c",
        "pngwio.c",
        "pngwrite.c",
        "pngwtran.c",
        "pngwutil.c"
        ;

    png.Public += "org.sw.demo.madler.zlib-1"_dep;
    png += sw::Shared, "__WIN32DLL__"_def;
}
