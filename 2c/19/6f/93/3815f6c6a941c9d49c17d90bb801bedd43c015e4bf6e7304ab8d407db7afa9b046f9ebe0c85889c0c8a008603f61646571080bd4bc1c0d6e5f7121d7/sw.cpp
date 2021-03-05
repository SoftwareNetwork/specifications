void build(Solution &s)
{
    auto &jasper = s.addTarget<LibraryTarget>("mdadams.jasper", "2.0.26");
    jasper += Git("https://github.com/mdadams/jasper", "version-{v}");

    jasper.SourceDir /= "src/libjasper";
    jasper.setChecks("jas");
    jasper +=
        ".*\\.c"_rr,
        ".*\\.h"_rr,
        ".*\\.in"_rr;

    if (jasper.getBuildSettings().TargetOS.Type == OSType::Windows ||
        jasper.getBuildSettings().TargetOS.Type == OSType::Mingw)
        jasper += "WIN32"_def;

    jasper += sw::Shared, "JAS_BUILDING_DLL"_d;
    jasper.Public += sw::Shared, "JAS_DLL"_d;
    jasper.Public += sw::Shared, "JAS_HAVE_VISIBILITY"_d;

    jasper.Public += "org.sw.demo.jpeg"_dep;

    jasper.configureFile("include/jasper/jas_config.h.in", "jasper/jas_config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("jas");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("fcntl.h");

    for (auto &check : s.all)
        check->Prefixes.insert("JAS_");
}
