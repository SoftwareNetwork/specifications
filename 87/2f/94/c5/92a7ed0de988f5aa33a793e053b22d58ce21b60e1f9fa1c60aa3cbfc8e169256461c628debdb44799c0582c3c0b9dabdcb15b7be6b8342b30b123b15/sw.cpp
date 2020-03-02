void build(Solution &s)
{
    auto &minizip = s.addTarget<StaticLibraryTarget>("madler.minizip", "1.2.11");
    minizip += Git("https://github.com/madler/zlib", "v{v}");

    minizip +=
        "contrib/minizip/.*\\.c"_rr,
        "contrib/minizip/.*\\.h"_rr;

    minizip -=
        "contrib/minizip/miniunz.c";

    minizip.Public +=
        "contrib"_id,
        "contrib/minizip"_id
        ;

    minizip.Public += "HAVE_BZIP2"_d;
    minizip.Private += sw::Shared, "ZLIB_INTERNAL"_d;
    minizip.Public += sw::Shared, "ZLIB_DLL"_d;

    minizip.Public += "org.sw.demo.bzip2"_dep;
    minizip.Public += "org.sw.demo.madler.zlib"_dep;
}
