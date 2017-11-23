
void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &jasper = s.addTarget<LibraryTarget>("mdadams.jasper", "2.0.14");
    jasper.Source = Git("https://github.com/mdadams/jasper", "version-{v}");
    jasper.setRootDirectory("src/libjasper");

    jasper +=
        ".*\\.c"_rr,
        ".*\\.h"_rr,
        ".*\\.in"_rr;

    jasper -=
        "jpg/jpg_dummy.c";

    jasper.Public +=
        "include"_id;

    jasper.Private += sw::Shared, "JAS_BUILDING_DLL"_d;
    jasper.Public += sw::Shared, "JAS_DLL"_d;
    jasper.Public += sw::Shared, "JAS_HAVE_VISIBILITY"_d;

    jasper.Public += "pub.cppan2.demo.jpeg-9"_dep;

    jasper.configureFile("include/jasper/jas_config.h.in", "jasper/jas_config.h");
}
