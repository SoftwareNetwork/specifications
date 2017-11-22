void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &harfbuzz = s.addTarget<LibraryTarget>("behdad.harfbuzz", "1.7.1");
    harfbuzz.Source = Git("https://github.com/behdad/harfbuzz", "{v}");

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

    harfbuzz.Public += "pub.cppan2.demo.cairographics.cairo-1"_dep;
    harfbuzz.Public += "pub.cppan2.demo.freetype-2"_dep;
    harfbuzz.Public += "pub.cppan2.demo.gnome.glib.glib-2"_dep;
    harfbuzz.Public += "pub.cppan2.demo.unicode.icu.i18n"_dep;
    harfbuzz.Public += "pub.cppan2.demo.grigorig.ucdn-master"_dep;
    auto r = harfbuzz + "pub.cppan2.demo.ragel-6"_dep;
    r->Dummy = true;

    harfbuzz.Variables["HB_VERSION_MAJOR"] = harfbuzz.Variables["PACKAGE_VERSION_MAJOR"];
    harfbuzz.Variables["HB_VERSION_MINOR"] = harfbuzz.Variables["PACKAGE_VERSION_MINOR"];
    harfbuzz.Variables["HB_VERSION_MICRO"] = harfbuzz.Variables["PACKAGE_VERSION_PATCH"];

    harfbuzz.configureFile("src/hb-version.h.in", "hb-version.h");

    auto ragel = [&harfbuzz, &r](const std::string &f)
    {
        const auto i = harfbuzz.SourceDir / "src" / (f + ".rl");
        const auto o = harfbuzz.BinaryDir / (f + ".hh");
        auto c = std::make_shared<Command>();
        c->dependency = r;
        c->args = { i.string(), "-o", o.string() };
        c->addInput(i);
        c->addOutput(o);
        harfbuzz += o;
    };

    ragel("hb-buffer-deserialize-json");
    ragel("hb-buffer-deserialize-text");
    ragel("hb-ot-shape-complex-indic-machine");
    ragel("hb-ot-shape-complex-myanmar-machine");
    ragel("hb-ot-shape-complex-use-machine");
}
