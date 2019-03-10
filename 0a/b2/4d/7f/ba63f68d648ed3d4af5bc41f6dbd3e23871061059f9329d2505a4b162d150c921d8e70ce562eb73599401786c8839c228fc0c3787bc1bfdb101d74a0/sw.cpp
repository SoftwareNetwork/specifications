void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("taocpp.taopq", "master");
    t += Git("https://github.com/taocpp/taopq", "", "{v}");
    t.CPPVersion = CPPLanguageStandard::CPP17;
    t += "include/.*"_rr;
    t += "src/lib/.*"_rr;
    if (s.Settings.TargetOS.Type == OSType::Windows)
        t += "WIN32"_def;
    t += "org.sw.demo.find.libpq-master"_dep;
}
