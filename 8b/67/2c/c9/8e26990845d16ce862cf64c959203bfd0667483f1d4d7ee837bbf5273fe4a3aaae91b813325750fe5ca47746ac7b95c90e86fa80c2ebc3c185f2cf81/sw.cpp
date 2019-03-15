void build(Solution &s)
{
    auto &t = s.addTarget<Library>("ebassi.graphene", "1.8.6");
    t += Git("https://github.com/ebassi/graphene");

    t.ExportAllSymbols = true;
    t += "src/.*"_r;
    t += "GRAPHENE_COMPILATION"_def;
    t.writeFileOnce(t.BinaryPrivateDir / "config.h");
    t.configureFile("src/graphene-config.h.meson", "graphene-config.h");

    t.Variables["GRAPHENE_MAJOR_VERSION"] = t.Variables["PACKAGE_VERSION_MAJOR"];
    t.Variables["GRAPHENE_MINOR_VERSION"] = t.Variables["PACKAGE_VERSION_MINOR"];
    t.Variables["GRAPHENE_MICRO_VERSION"] = t.Variables["PACKAGE_VERSION_PATCH"];
    t.configureFile("src/graphene-version.h.in", "graphene-version.h");

    t.Public += "org.sw.demo.gnome.glib.gobject"_dep;
}
