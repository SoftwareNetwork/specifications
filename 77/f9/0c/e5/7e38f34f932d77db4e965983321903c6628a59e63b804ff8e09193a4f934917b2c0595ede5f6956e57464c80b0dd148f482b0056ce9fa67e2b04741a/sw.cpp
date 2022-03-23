void build(Solution &s)
{
    auto &harfbuzz = s.addTarget<LibraryTarget>("behdad.harfbuzz", "4.1.0");
    harfbuzz += Git("https://github.com/behdad/harfbuzz");

    harfbuzz += cpp11;

    harfbuzz -=
        "src/[^/]*\\.cc"_rr,
        "src/[^/]*\\.h"_rr,
        "src/[^/]*\\.hh"_rr,
        "src/[^/]*\\.rl"_rr,
        "src/[^/]*\\.tmpl"_rr,
        "src/hb-version.h.in";
    harfbuzz += "src/harfbuzz.cc";

    // fix msvc build issue
    harfbuzz.Private += "HAVE_ROUND"_d;

    harfbuzz.Private += "HAVE_CAIRO"_d;
    harfbuzz.Private += "HAVE_CAIRO_FT"_d;
    harfbuzz.Private += "HAVE_FALLBACK"_d;
    harfbuzz.Private += "HAVE_FREETYPE"_d;
    harfbuzz.Private += "HAVE_GLIB"_d;
    harfbuzz.Private += "HAVE_ICU"_d;
    harfbuzz.Private += "HAVE_OT"_d;
    harfbuzz.Interface += sw::Shared, "HD_EXTERN=SW_IMPORT"_d;
    harfbuzz.Private += sw::Shared, "HB_EXTERN=SW_EXPORT"_d;

    if (!harfbuzz.getBuildSettings().TargetOS.is(OSType::Windows))
        harfbuzz += "HAVE_PTHREAD"_def;
    if (harfbuzz.getBuildSettings().TargetOS.is(OSType::Macos))
    {
        harfbuzz += "HAVE_CORETEXT"_def;
        harfbuzz += "CoreFoundation"_framework;
        harfbuzz += "CoreGraphics"_framework;
        harfbuzz += "CoreText"_framework;
    }

    harfbuzz.Public += "org.sw.demo.cairographics.cairo"_dep;
    harfbuzz.Public += "org.sw.demo.freetype"_dep;
    harfbuzz.Public += "org.sw.demo.gnome.glib.glib"_dep;
    harfbuzz.Public += "org.sw.demo.unicode.icu.i18n"_dep;
    harfbuzz.Public += "org.sw.demo.grigorig.ucdn-master"_dep;

    harfbuzz.Variables["HB_VERSION_MAJOR"] = harfbuzz.Variables["PACKAGE_VERSION_MAJOR"];
    harfbuzz.Variables["HB_VERSION_MINOR"] = harfbuzz.Variables["PACKAGE_VERSION_MINOR"];
    harfbuzz.Variables["HB_VERSION_MICRO"] = harfbuzz.Variables["PACKAGE_VERSION_PATCH"];

    harfbuzz.configureFile("src/hb-version.h.in", "hb-version.h");
    harfbuzz.writeFileOnce(harfbuzz.BinaryPrivateDir / "config.h");

    auto ragel = [&harfbuzz](const std::string &f)
    {
        auto c = harfbuzz.addCommand();
        c
            << cmd::prog("org.sw.demo.ragel"_dep)
            << cmd::in(path("src") / (f + ".rl"))
            << "-o" << cmd::out(f + ".hh");
    };

    ragel("hb-buffer-deserialize-json");
    ragel("hb-buffer-deserialize-text");
    ragel("hb-ot-shape-complex-indic-machine");
    ragel("hb-ot-shape-complex-myanmar-machine");
    ragel("hb-ot-shape-complex-use-machine");
}
