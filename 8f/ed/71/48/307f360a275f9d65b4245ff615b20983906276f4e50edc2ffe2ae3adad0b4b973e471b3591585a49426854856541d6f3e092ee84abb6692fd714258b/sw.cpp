void build(Solution &s)
{
    auto &t = s.addTarget<Library>("troldal.OpenXLSX", "master");
    t += Git("https://github.com/troldal/OpenXLSX", "", "{v}");

    t.CPPVersion = CPPLanguageStandard::CPP17;
    t.setRootDirectory("@library/@openxlsx");
    t += ".*"_rr;
    t.Public += "interfaces/c++/headers"_id;
    t += "implementation/headers"_id;

    t += "org.sw.demo.ctabin.libzippp"_dep;
    t += "org.sw.demo.zeux.pugixml"_dep;
}
