void build(Solution &s)
{
    auto &t = s.addLibrary("swami.libinstpatch", "master");
    t += Git("https://github.com/swami/libinstpatch");

    t += "libinstpatch/.*"_rr;
    t -= "libinstpatch/gir_prog.c";
    t -= "libinstpatch/IpatchState.*"_rr;

    t.configureFile("config.h.cmake", t.BinaryPrivateDir / "config.h");
    t.configureFile("libinstpatch/libinstpatch.h.in", t.BinaryPrivateDir / "libinstpatch.h");
    t.configureFile("libinstpatch/libinstpatch.h.in", "libinstpatch/libinstpatch.h");

    t.Variables["IPATCH_VERSION"] = "\"" + t.Variables["PACKAGE_VERSION"].toString() + "\"";
    t.Variables["IPATCH_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
    t.Variables["IPATCH_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
    t.Variables["IPATCH_VERSION_MICRO"] = t.Variables["PACKAGE_VERSION_PATCH"];
    t.configureFile("libinstpatch/version.h.in", t.BinaryPrivateDir / "version.h");
    t.configureFile("libinstpatch/version.h.in", "libinstpatch/version.h");

    t += "org.sw.demo.gnome.glib.gobject"_dep;
    t.Public += "org.sw.demo.erikd.libsndfile-master"_dep;

    for (auto ext : { "h"s, "c"s })
    {
        // glib.genmarshal
        auto c = t.addCommand();
        c << cmd::prog("org.sw.demo.python.exe-3"_dep)
            << t.getFile("org.sw.demo.gnome.glib.gobject"_dep, "gobject/glib-genmarshal.in")
            << "--output" << cmd::out("marshals." + ext)
            << (ext == "h" ? "--header" : "--body")
            << cmd::in("libinstpatch/marshals.list")
            ;
    }
}
