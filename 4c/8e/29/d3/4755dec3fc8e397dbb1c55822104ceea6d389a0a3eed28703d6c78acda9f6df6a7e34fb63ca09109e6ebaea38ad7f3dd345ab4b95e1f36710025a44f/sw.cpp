void build(Solution &s)
{
    auto &p = s.addProject("imagemagick", "7.1.0.0");
    p += Git("https://github.com/ImageMagick/ImageMagick", "{M}.{m}.{p}-{t}");

    auto &magick = p.addTarget<LibraryTarget>("magick");
    {
        magick.PackageDefinitions = true;
        magick += cpp17;

        magick.setChecks("magick", true);

        magick +=
            "coders/.*\\.[hc]"_rr,
            "filters/.*\\.[hc]"_rr,
            "MagickCore/.*\\.[hc]"_rr,
            "MagickCore/.*\\.in"_rr;

        magick.Public += "MAGICKCORE_QUANTUM_DEPTH=16"_d;
        magick.Public += "MAGICKCORE_HDRI_ENABLE"_d;

        magick += "_MAGICKLIB_"_d;
        magick.Public += "_MAGICKDLL_"_d;

        if (magick.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            magick.Protected += "magick_restrict="_d;
            magick.Public += "WIN32"_d;
            //magick += "UNICODE"_d;
            if (magick.getBuildSettings().TargetOS.is(ArchType::x86_64))
                magick.Public += "WIN64"_d;
            magick +=
                "User32.lib"_slib,
                "Advapi32.lib"_slib,
                "Gdi32.lib"_slib,
                "OleAut32.lib"_slib,
                "Urlmon.lib"_slib,
                "ws2_32.lib"_slib
                ;
        }

        magick.writeFileOnce("MagickCore/magick-baseconfig.h", R"(
#if defined(_WIN64)
typedef long long ssize_t;
#elif defined(_WIN32)
typedef int ssize_t;
#endif

#define MAGICKCORE_PACKAGE_NAME PACKAGE_NAME
)");
        magick.Variables["PACKAGE_LIB_VERSION"] = 7;
        magick.Variables["MAGICK_LIB_VERSION_NUMBER"] = 7;
        magick.Variables["MAGICK_LIBRARY_CURRENT"] = 7;
        magick.Variables["MAGICK_LIBRARY_CURRENT_MIN"] = 7;
        magick.Variables["MAGICKPP_LIBRARY_VERSION_INFO"] = 7;
        magick.Variables["MAGICKPP_LIBRARY_CURRENT"] = 7;
        magick.Variables["MAGICKPP_LIBRARY_CURRENT_MIN"] = 7;
        magick.Variables["MAGICK_LIB_VERSION_TEXT"] = magick.getPackage().getVersion().toString();

        magick.configureFile("MagickCore/version.h.in", "MagickCore/version.h");
    }

    auto &magickwand = p.addTarget<LibraryTarget>("wand");
    {
        magickwand += "MagickWand/.*\\.[hc]"_r;

        magickwand += "_MAGICKLIB_"_d;
        magickwand.Public += "_MAGICKDLL_"_d;

        magickwand.Public += magick;
    }

    auto &magickpp = p.addTarget<LibraryTarget>("magickpp");
    {
        magickpp.setRootDirectory("Magick++");
        magickpp.Public += magickwand;
        magickpp += "MAGICKCORE_IMPLEMENTATION"_def;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("magick");
    s.checkTypeSize("void *");

    for (auto &check : s.all)
        check->Prefixes.insert("MAGICKCORE_");
}
