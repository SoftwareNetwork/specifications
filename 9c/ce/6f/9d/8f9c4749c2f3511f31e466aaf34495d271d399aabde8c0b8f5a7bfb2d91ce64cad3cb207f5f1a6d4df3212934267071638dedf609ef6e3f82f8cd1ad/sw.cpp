void build(Solution &sln)
{
    auto &s = sln.addDirectory("gnome");
    auto &p = s.addProject("pango", "1.42.4");
    p += RemoteFile("https://ftp.gnome.org/pub/GNOME/sources/pango/{M}.{m}/pango-{v}.tar.xz");

    auto &pango = p.addTarget<LibraryTarget>("pango");
    pango.ApiName = "SW_PANGO_LIBRARY_API";
    pango.setChecks("pango");

    pango +=
        "config.h.in",
        "pango/break.*"_rr,
        "pango/ellipsize.c",
        "pango/fonts.c",
        "pango/glyphstring.c",
        "pango/mini-fribidi/.*\\.c"_rr,
        "pango/mini-fribidi/.*\\.h"_rr,
        "pango/mini-fribidi/.*\\.i"_rr,
        "pango/modules.c",
        "pango/pango-.*"_rr,
        "pango/pango.h",
        "pango/reorder-items.c",
        "pango/shape.c";

    pango -=
        "pango/pango-ot.*"_rr;

    pango.Public +=
        "pango"_id;

    pango.Public += "HAVE_CAIRO=1"_d;
    pango.Public += "HAVE_CAIRO_FREETYPE=1"_d;
    pango.Public += "HAVE_CAIRO_PDF=1"_d;
    pango.Public += "HAVE_CAIRO_PNG=1"_d;
    pango.Public += "HAVE_CAIRO_PS=1"_d;
    pango.Public += "HAVE_DIRENT_H=1"_d;
    pango.Public += "HAVE_FREETYPE=1"_d;
    pango.Public.Definitions["PANGO_BINARY_AGE"] = "0x"s +
        pango.Variables["PACKAGE_VERSION_MINOR_NUM"] +
        pango.Variables["PACKAGE_VERSION_PATCH_NUM"];
    pango.Public += "PANGO_ENABLE_BACKEND=1"_d;
    pango.Public += "PANGO_ENABLE_ENGINE=1"_d;
    if (s.Settings.TargetOS.Type != OSType::Windows)
    {
        pango.Private += "SYSCONFDIR=\"/usr/local/etc\""_d;
    }
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        pango.Private += "SYSCONFDIR=\"./pango\""_d;
        pango.Public += "HAVE_CAIRO_WIN32"_d;
        pango.Public += "HAVE_WIN32=1"_d;
    }

    pango.Public += "org.sw.demo.gnome.glib.gobject-2"_dep;
    pango.Public += "org.sw.demo.gnu.fribidi.fribidi-1"_dep;

    pango.writeFileOnce(pango.BinaryPrivateDir / "config.h");
    pango.replaceInFileOnce("pango/pango-version-macros.h",
        "#define _PANGO_EXTERN extern", "#define _PANGO_EXTERN extern SW_PANGO_LIBRARY_API");

    //
    auto &pangoft2 = p.addTarget<LibraryTarget>("pangoft2");
    pangoft2.ApiName = "SW_PANGOFT2_LIBRARY_API";
    pangoft2.setChecks("pango");

    pangoft2 +=
        "pango/pango-version-macros.h",
        "pango/pango-ot-buffer.c",
        "pango/pango-ot-info.c",
        "pango/pango-ot-private.h",
        "pango/pango-ot-ruleset.c",
        "pango/pango-ot-tag.c",
        "pango/pango-ot.h",
        "pango/pangofc-decoder.c",
        "pango/pangofc-decoder.h",
        "pango/pangofc-font.c",
        "pango/pangofc-font.h",
        "pango/pangofc-fontmap.c",
        "pango/pangofc-fontmap.h",
        "pango/pangofc-private.h",
        "pango/pangofc-shape.c",
        "pango/pangoft2-fontmap.c",
        "pango/pangoft2-private.h",
        "pango/pangoft2-render.c",
        "pango/pangoft2.c",
        "pango/pangoft2.h";

    pangoft2.Public +=
        "."_id,
        "pango"_id;

    pangoft2.Public += pango;
    pangoft2.Public += "org.sw.demo.behdad.harfbuzz-1"_dep;
    pangoft2.Public += "org.sw.demo.freedesktop.fontconfig.fontconfig-2"_dep;

    pangoft2.writeFileOnce(pangoft2.BinaryPrivateDir / "config.h");
    pangoft2.replaceInFileOnce("pango/pango-version-macros.h",
        "#define _PANGO_EXTERN extern", "#define _PANGO_EXTERN extern SW_PANGOFT2_LIBRARY_API");

    //
    auto &pangocairo = p.addTarget<LibraryTarget>("pangocairo");
    pangocairo.ApiName = "SW_PANGOCAIRO_LIBRARY_API";
    pangocairo.setChecks("pango");

    pangocairo +=
        "pango/pango-version-macros.h",
        "pango/pangocairo-.*"_rr,
        "pango/pangocairo.h",
        "pango/pangocoretext.*"_rr,
        "pango/pangowin32-fontcache.c",
        "pango/pangowin32-fontmap.c",
        "pango/pangowin32-private.h",
        "pango/pangowin32-shape.c",
        "pango/pangowin32.c",
        "pango/pangowin32.h";

    pangocairo.Public +=
        "pango"_id;

    pangocairo.Public += "org.sw.demo.cairographics.cairo-1"_dep;
    pangocairo.Public += pangoft2;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        pangocairo -=
            "pango/pangocoretext.*"_rr,
            "pango/pangocairo-coretext.*"_rr;
        pangocairo +=
            "Usp10.lib"_l;
    }
    else
        pangocairo -=
        "pango/pangocairo-win32.*"_rr,
        "pango/pangowin32.*"_rr;

    pangocairo.writeFileOnce(pangocairo.BinaryPrivateDir / "config.h");
    pangocairo.replaceInFileOnce("pango/pango-version-macros.h",
        "#define _PANGO_EXTERN extern", "#define _PANGO_EXTERN extern SW_PANGOCAIRO_LIBRARY_API");
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
