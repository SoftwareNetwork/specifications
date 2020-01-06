void build(Solution &s)
{
    auto &p = s.addProject("DaanDeMeyer", "10.0.3");
    p += Git("https://github.com/DaanDeMeyer/reproc", "v{v}");

    auto &t = p.addLibrary("reproc");
    t.ApiName = "REPROC_EXPORT";
    t.setRootDirectory("reproc");
    t.writeFileOnce("reproc/export.h");
    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        t -= ".*posix.*"_rr;
    else
        t -= ".*windows.*"_rr;
    t += "HAVE_ATTRIBUTE_LIST"_def;

    auto &tpp = p.addLibrary("reprocpp");
    tpp.ApiName = "REPROCXX_EXPORT";
    tpp.setRootDirectory("reprocxx");
    tpp.writeFileOnce("reprocxx/export.hpp");
    tpp.Public += t;
}
