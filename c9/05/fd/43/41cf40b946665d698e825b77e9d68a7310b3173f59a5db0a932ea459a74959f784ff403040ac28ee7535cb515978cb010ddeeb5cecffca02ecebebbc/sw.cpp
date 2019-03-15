void build(Solution &s)
{
    // Uncomment to make a project. Also replace s.addTarget(). with p.addTarget() below.
    auto &atk = s.addTarget<Library>("gnome.atk", "2.31.92");
    atk += RemoteFile("https://gitlab.gnome.org/GNOME/atk/-/archive/ATK_{M}_{m}_{p}/atk-ATK_{M}_{m}_{p}.tar.bz2");

    atk.ExportAllSymbols = true;

    atk += "atk/.*"_rr;
    atk.Public += "atk"_idir;
    atk += IncludeDirectory(atk.BinaryDir / "atk");

    atk += "ATK_LOCALEDIR=\"\""_def;
    atk += "VERSION=\"\""_def;

    atk.writeFileOnce(atk.BinaryPrivateDir / "config.h");

    atk.Variables["ATK_MAJOR_VERSION"] = atk.Variables["PACKAGE_VERSION_MAJOR"];
    atk.Variables["ATK_MINOR_VERSION"] = atk.Variables["PACKAGE_VERSION_MINOR"];
    atk.Variables["ATK_MICRO_VERSION"] = atk.Variables["PACKAGE_VERSION_PATCH"];
    atk.Variables["ATK_BINARY_AGE"] = 2;
    atk.Variables["ATK_INTERFACE_AGE"] = 2;

    atk.configureFile("atk/atkversion.h.in", "atk/atkversion.h");

    atk.Public += "org.sw.demo.gnome.glib.gobject"_dep;

    for (auto ext : {"h"s, "c"s})
    {
        // glib.genmarshal
        auto c = atk.addCommand();
        c << cmd::prog("org.sw.demo.python.exe-3"_dep)
            << atk.getFile("org.sw.demo.gnome.glib.gobject-2"_dep, "gobject/glib-genmarshal.in")
            << "--prefix" << "atk_marshal"
            << "--output" << cmd::out("atkmarshal." + ext)
            << (ext == "h" ? "--header" : "--body")
            << cmd::in("atk/atkmarshal.list")
            ;
    }

    for (auto ext : {"h", "c"})
    {
        // glib.mkenums
        auto c = atk.addCommand();
        c << cmd::prog("org.sw.demo.python.exe-3"_dep)
            << atk.getFile("org.sw.demo.gnome.glib.gobject-2"_dep, "gobject/glib-mkenums.in")
            << "--template"
            << cmd::in("atk/atk-enum-types."s + ext + ".template")
            << "--output"
            << cmd::out("atk/atk-enum-types."s + ext)
            ;
        for (auto &[p, f] : atk[".*\\.h"_rr])
        {
            if (!f->skip)
                c << cmd::in(p);
        }
    }
}
