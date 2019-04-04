void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("ximion.appstream", "0.12.6");
    p += Git("https://github.com/ximion/appstream", "APPSTREAM_{M}_{m}_{p}");

    p += "src/.*"_rr;

    p += "AS_COMPILATION"_def;
    p.Public += "org.sw.demo.gnome.glib.gio"_dep;
    p.Public += "org.sw.demo.xmlsoft.libxml2"_dep;
    p.Public += "org.sw.demo.pyyaml.yaml"_dep;

    p.configureFile("config.h.in", p.BinaryPrivateDir / "config.h");

    p.addCommand()
        << cmd::prog("org.sw.demo.gnu.gperf-*"_dep)
        << cmd::in("src/as-tag-yaml.gperf")
        << "--output-file"
        << cmd::out("as-tag-yaml-private.h")
        ;
    p.addCommand()
        << cmd::prog("org.sw.demo.gnu.gperf-*"_dep)
        << cmd::in("src/as-tag-yaml.gperf")
        << "--output-file"
        << cmd::out("as-tag-yaml-private.h")
        ;
}
