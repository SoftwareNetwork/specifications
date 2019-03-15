void build(Solution &s)
{
    auto &jasper = s.addTarget<LibraryTarget>("mdadams.jasper", "2.0.16");
    jasper += Git("https://github.com/mdadams/jasper", "version-{v}");

    jasper.SourceDir /= "src/libjasper";
    jasper +=
        ".*\\.c"_rr,
        ".*\\.h"_rr,
        ".*\\.in"_rr;

    jasper -=
        "jpg/jpg_dummy.c";

    jasper.Private += sw::Shared, "JAS_BUILDING_DLL"_d;
    jasper.Public += sw::Shared, "JAS_DLL"_d;
    jasper.Public += sw::Shared, "JAS_HAVE_VISIBILITY"_d;

    jasper.Public += "org.sw.demo.jpeg-9"_dep;

    jasper.configureFile("include/jasper/jas_config.h.in", "jasper/jas_config.h");
}
