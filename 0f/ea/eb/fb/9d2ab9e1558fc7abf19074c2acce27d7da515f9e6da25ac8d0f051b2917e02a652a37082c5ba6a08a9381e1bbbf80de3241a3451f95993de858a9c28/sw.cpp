void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("libxmlplusplus", "5.4.0");
    t += Git("https://github.com/libxmlplusplus/libxmlplusplus");

    t += cpp17;
    t += "libxml\\+\\+/.*"_rr;
    t += "LIBXMLPP_BUILD"_def;
    t.Public += sw::Shared, "LIBXMLPP_DLL"_def;
    t += "org.sw.demo.xmlsoft.libxml2"_dep;
    t.configureFile("libxml++config.h.meson", "libxml++config.h");
}
