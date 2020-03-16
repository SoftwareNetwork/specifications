void build(Solution &s)
{
    auto &p = s.addProject("gnome.gtk.gdk", "2.38.1");
    p += RemoteFile("https://ftp.gnome.org/pub/GNOME/sources/gdk-pixbuf/{M}.{m}/gdk-pixbuf-{v}.tar.xz");

    auto &gdk = p.addTarget<Library>("pixbuf");

    gdk.ExportAllSymbols = true;

    gdk -= "gdk-pixbuf/.*"_rr;
    gdk.Public += "gdk-pixbuf"_id;
    gdk.Public += "."_id;
    gdk += IncludeDirectory(gdk.BinaryDir / "gdk-pixbuf");

    gdk += "gdk-pixbuf/.*\\.[hc]"_r;
    gdk += "gdk-pixbuf/pixops/pixops.*\\.[hc]"_r;
    gdk -= "gdk-pixbuf/gdk-pixbuf-pixdata.c";
    gdk -= "gdk-pixbuf/gdk-pixbuf-csource.c";
    gdk -= "gdk-pixbuf/gdk-pixbuf-private.h";
    gdk -= "gdk-pixbuf/make.*\\.[c]"_r;
    gdk -= "gdk-pixbuf/test.*\\.[c]"_r;
    gdk -= "gdk-pixbuf/query.*\\.[c]"_r;

    gdk += "GDK_PIXBUF_COMPILATION"_def;
    gdk += "GDK_PIXBUF_ENABLE_BACKEND"_def;
    gdk += "USE_GMODULE"_def;

    for (auto m : {
        "png",
        "bmp",
        "gif",
        "ico",
        "ani",
        "jpeg",
        "pnm",
        "tiff",
        "xpm",
        "xbm",
        "tga",
        "icns",
        "jasper",
        "qtif",
        "gdiplus",
        "gdip_png",
        })
    {
        gdk += Definition("INCLUDE_"s + m);
    }

    gdk += "GDK_PIXBUF_LOCALEDIR=\"\""_def;
    gdk += "GDK_PIXBUF_LIBDIR=\"\""_def;
    gdk += Definition("GDK_PIXBUF_BINARY_VERSION=" + gdk.Variables["PACKAGE_VERSION_NUM"].toString());

    gdk.Public += "org.sw.demo.gnome.glib.gio"_dep;
    gdk.Public += "org.sw.demo.mdadams.jasper"_dep;
    gdk.Public += "org.sw.demo.glennrp.png"_dep;
    gdk.Public += "org.sw.demo.tiff"_dep;

    if (gdk.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        gdk += "Gdiplus.lib"_slib;
        gdk += "Ole32.lib"_slib;
    }

    gdk.writeFileOnce(gdk.BinaryPrivateDir / "config.h");

    gdk.Variables["GDK_PIXBUF_MAJOR"] = gdk.Variables["PACKAGE_VERSION_MAJOR"];
    gdk.Variables["GDK_PIXBUF_MINOR"] = gdk.Variables["PACKAGE_VERSION_MINOR"];
    gdk.Variables["GDK_PIXBUF_MICRO"] = gdk.Variables["PACKAGE_VERSION_PATCH"];
    gdk.Variables["GDK_PIXBUF_VERSION"] = gdk.Variables["PACKAGE_VERSION"];
    gdk.configureFile("gdk-pixbuf/gdk-pixbuf-features.h.in", "gdk-pixbuf/gdk-pixbuf-features.h");

    gdk.pushFrontToFileOnce("gdk-pixbuf/io-gdip-png.c", R"(#include "config.h"
#include <glib/gi18n-lib.h>)");

    for (auto ext : {"h"s, "c"s})
    {
        // glib.genmarshal
        auto c = gdk.addCommand();
        c << cmd::prog("org.sw.demo.python.exe-3"_dep)
            << gdk.getFile("org.sw.demo.gnome.glib.gobject-2"_dep, "gobject/glib-genmarshal.in")
            << "--prefix" << "_gdk_pixbuf_marshal"
            << "--output" << cmd::out("gdk-pixbuf-marshal." + ext)
            << (ext == "h" ? "--header" : "--body")
            << cmd::in("gdk-pixbuf/gdk-pixbuf-marshal.list")
            ;
    }

    for (auto ext : {"h", "c"})
    {
        // glib.mkenums
        auto c = gdk.addCommand();
        c << cmd::prog("org.sw.demo.python.exe-3"_dep)
            << gdk.getFile("org.sw.demo.gnome.glib.gobject-2"_dep, "gobject/glib-mkenums.in")
            << "--template"
            << cmd::in("gdk-pixbuf/gdk-pixbuf-enum-types."s + ext + ".template")
            << "--output"
            << cmd::out("gdk-pixbuf/gdk-pixbuf-enum-types."s + ext)
            ;
        for (auto &[p, f] : gdk["gdk-pixbuf/gdk-pixbuf-.*\\.h"_rr])
        {
            if (!f->skip)
                c << cmd::in(p);
        }
    }
}
