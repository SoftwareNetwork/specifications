void build(Solution &s)
{
    auto &harfbuzz = s.addTarget<LibraryTarget>("behdad.harfbuzz", "2.3.1");
    harfbuzz += Git("https://github.com/behdad/harfbuzz", "{v}");

    harfbuzz +=
        "src/[^/]*\\.c"_rr,
        "src/[^/]*\\.cc"_rr,
        "src/[^/]*\\.h"_rr,
        "src/[^/]*\\.hh"_rr,
        "src/[^/]*\\.rl"_rr,
        "src/[^/]*\\.tmpl"_rr,
        "src/hb-version.h.in";

    harfbuzz -=
        "src/hb-coretext.cc",
        "src/hb-directwrite.cc",
        "src/hb-gobject-enums.cc",
        "src/hb-gobject-structs.cc",
        "src/hb-graphite2.cc",
        "src/hb-uniscribe.cc";

    harfbuzz -=
        "src/main.cc",
        "src/dump.*"_rr,
        "src/test.*"_rr;

    harfbuzz.Public +=
        "src"_id;

    harfbuzz.Private += "HAVE_CAIRO"_d;
    harfbuzz.Private += "HAVE_CAIRO_FT"_d;
    harfbuzz.Private += "HAVE_FALLBACK"_d;
    harfbuzz.Private += "HAVE_FREETYPE"_d;
    harfbuzz.Private += "HAVE_GLIB"_d;
    harfbuzz.Private += "HAVE_ICU"_d;
    harfbuzz.Private += "HAVE_OT"_d;
    harfbuzz.Interface += sw::Shared, "HD_EXTERN=SW_IMPORT"_d;
    harfbuzz.Private += sw::Shared, "HB_EXTERN=SW_EXPORT"_d;

    harfbuzz.Public += "org.sw.demo.cairographics.cairo-1"_dep;
    harfbuzz.Public += "org.sw.demo.freetype-2"_dep;
    harfbuzz.Public += "org.sw.demo.gnome.glib.glib-2"_dep;
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
            << cmd::prog("org.sw.demo.ragel-6"_dep)
            << cmd::in(path("src") / (f + ".rl"))
            << "-o" << cmd::out(f + ".hh");
    };

    ragel("hb-buffer-deserialize-json");
    ragel("hb-buffer-deserialize-text");
    ragel("hb-ot-shape-complex-indic-machine");
    ragel("hb-ot-shape-complex-myanmar-machine");
    ragel("hb-ot-shape-complex-use-machine");
}
