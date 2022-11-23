void build(Solution &s)
{
    auto &p = s.addProject("gnome.pango", "1.49.1");
    p += RemoteFile("https://ftp.gnome.org/pub/GNOME/sources/pango/{M}.{m}/pango-{v}.tar.xz");

    auto &pango = p.addTarget<LibraryTarget>("pango");
    {
        pango.ApiName = "_PANGO_EXTERN";
        pango.setChecks("pango");

        pango +=
            "pango/.*\\.meson"_rr,
            "pango/break.*"_rr,
            "pango/ellipsize.c",
            "pango/fonts.c",
            "pango/glyphstring.c",
            "pango/modules.c",
            "pango/pango-.*"_rr,
            "pango/pango.*\\.h"_rr,
            "pango/reorder-items.c",
            "pango/itemize.c",
            "pango/shape.c"
            ;

        pango -=
            "pango/pango-ot.*"_rr,
            "pango/emoji.*"_rr
            ;

        pango.Public +=
            "."_id,
            "pango"_id;

        pango.Public += "HAVE_CAIRO=1"_d;
        pango.Public += "HAVE_CAIRO_FREETYPE=1"_d;
        pango.Public += "HAVE_CAIRO_PDF=1"_d;
        pango.Public += "HAVE_CAIRO_PNG=1"_d;
        pango.Public += "HAVE_CAIRO_PS=1"_d;
        pango.Public += "HAVE_DIRENT_H=1"_d;
        pango.Public += "HAVE_FREETYPE=1"_d;
        pango.Public.Definitions["PANGO_BINARY_AGE"] = "0x"s +
            pango.Variables["PACKAGE_VERSION_MINOR_NUM"].toString() +
            pango.Variables["PACKAGE_VERSION_PATCH_NUM"].toString();
        pango.Public += "PANGO_ENABLE_BACKEND=1"_d;
        pango.Public += "PANGO_ENABLE_ENGINE=1"_d;
        if (pango.getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            pango.Private += "SYSCONFDIR=\"/usr/local/etc\""_d;
        }
        if (pango.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            pango.Private += "SYSCONFDIR=\"./pango\""_d;
            pango.Public += "HAVE_CAIRO_WIN32"_d;
            pango.Public += "HAVE_WIN32=1"_d;
        }

        pango.Public += "org.sw.demo.gnome.glib.gio"_dep;
        pango.Public += "org.sw.demo.gnu.fribidi.fribidi"_dep;
        pango.Public += "org.sw.demo.behdad.harfbuzz"_dep;

        pango.writeFileOnce(pango.BinaryPrivateDir / "config.h");
        pango.replaceInFileOnce("pango/pango-version-macros.h",
            "#define _PANGO_EXTERN extern", "#define _PANGO_EXTERN extern SW_PANGO_LIBRARY_API");

        pango.Variables["PANGO_VERSION_MAJOR"] = pango.Variables["PACKAGE_VERSION_MAJOR"];
        pango.Variables["PANGO_VERSION_MINOR"] = pango.Variables["PACKAGE_VERSION_MINOR"];
        pango.Variables["PANGO_VERSION_MICRO"] = pango.Variables["PACKAGE_VERSION_PATCH"];

        pango.configureFile("pango/pango-features.h.meson", "pango/pango-features.h");
        pango.patch("pango/pango-coverage.h", "#include <harfbuzz/hb.h>", "#include <hb.h>");
        pango.Protected += IncludeDirectory(pango.BinaryDir / "pango");

        for (auto ext : { "h", "c" })
        {
            // glib.mkenums
            auto c = pango.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << pango.getFile("org.sw.demo.gnome.glib.gobject"_dep, "gobject/glib-mkenums.in")
                << "--template"
                << cmd::in("pango/pango-enum-types."s + ext + ".template")
                << "--output"
                << cmd::out("pango/pango-enum-types."s + ext)
                ;
            for (auto &[p, f] : pango[".*\\.h"_rr])
            {
                if (!f->skip)
                    c << cmd::in(p);
            }
        }
    }

    //
    auto &pangoft2 = p.addTarget<LibraryTarget>("pangoft2");
    {
        pangoft2.ApiName = "SW_PANGOFT2_LIBRARY_API";
        pangoft2.setChecks("pango");

        pangoft2 +=
            "pango/pango-ot-buffer.c",
            "pango/pango-ot-info.c",
            "pango/pango-ot-ruleset.c",
            "pango/pango-ot-tag.c",
            "pango/pangofc-decoder.c",
            "pango/pangofc-font.c",
            "pango/pangofc-fontmap.c",
            "pango/pangoft2-fontmap.c",
            "pango/pangoft2-render.c",
            "pango/pangoft2.c"
            ;

        pangoft2.Public +=
            "."_id,
            "pango"_id;

        pangoft2.Public += pango;
        pangoft2.Public += "org.sw.demo.behdad.harfbuzz"_dep;
        pangoft2.Public += "org.sw.demo.freedesktop.fontconfig.fontconfig"_dep;

        pangoft2.writeFileOnce(pangoft2.BinaryPrivateDir / "config.h");
        pangoft2.replaceInFileOnce("pango/pango-version-macros.h",
            "#define _PANGO_EXTERN extern", "#define _PANGO_EXTERN extern SW_PANGOFT2_LIBRARY_API");

        pangoft2.patch("pango/pangofc-font.c", "#include <harfbuzz/hb-ot.h>", "#include <hb-ot.h>");
    }

    //
    auto &pangocairo = p.addTarget<LibraryTarget>("pangocairo");
    {
        pangocairo.ApiName = "SW_PANGOCAIRO_LIBRARY_API";
        pangocairo.setChecks("pango");

        pangocairo +=
            "pango/pangocairo-.*"_rr,
            "pango/pangocoretext.*"_rr,
            "pango/pangowin32-fontcache.c",
            "pango/pangowin32-fontmap.c",
            "pango/pangowin32.c"
            ;

        pangocairo.Public +=
            "."_id,
            "pango"_id;

        pangocairo.Public += "org.sw.demo.cairographics.cairo"_dep;
        pangocairo.Public += pangoft2;

        if (pangocairo.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            pangocairo -=
                "pango/pangocoretext.*"_rr,
                "pango/pangocairo-coretext.*"_rr;
            pangocairo +=
                "Usp10.lib"_slib;
        }
        else if (pangocairo.getBuildSettings().TargetOS.Type == OSType::Linux)
        {
            pangocairo -=
                "pango/pangocoretext.*"_rr,
                "pango/pangocairo-coretext.*"_rr;
            pangocairo -=
                "pango/pangocairo-win32.*"_rr,
                "pango/pangowin32.*"_rr;
        }
        else
        {
            pangocairo -=
                "pango/pangocairo-win32.*"_rr,
                "pango/pangowin32.*"_rr;

            pangocairo += "CoreFoundation"_framework;
            pangocairo += "CoreText"_framework;
        }

        pangocairo.writeFileOnce(pangocairo.BinaryPrivateDir / "config.h");
        pangocairo.replaceInFileOnce("pango/pango-version-macros.h",
            "#define _PANGO_EXTERN extern", "#define _PANGO_EXTERN extern SW_PANGOCAIRO_LIBRARY_API");
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("pango");
    s.checkFunctionExists("flockfile");
    s.checkFunctionExists("getpagesize");
    s.checkFunctionExists("strtok_r");
    s.checkFunctionExists("sysconf");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("ndir.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/dir.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/ndir.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
