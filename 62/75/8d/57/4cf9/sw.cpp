void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &t = s.addTarget<LibraryTarget>("png", "1.6.33");
    t.Source = Git("https://github.com/glennrp/libpng", "v{M}.{m}.{p}");
    t +=
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
    if (t.Settings.TargetOS.Type == OSType::Windows)
        t.Public += sw::Shared, "_WINDLL"_d;
    t.configureFile("scripts/pnglibconf.h.prebuilt", "pnglibconf.h");
    t += "pub.cppan2.demo.madler.zlib"_dep;
}
