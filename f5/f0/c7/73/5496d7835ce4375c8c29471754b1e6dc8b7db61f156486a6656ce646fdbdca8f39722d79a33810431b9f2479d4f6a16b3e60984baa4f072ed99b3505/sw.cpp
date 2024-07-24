void build(Solution &s)
{
    auto &lz4 = s.addTarget<LibraryTarget>("lz4", "1.10.0");
    lz4 += Git("https://github.com/lz4/lz4", "v{v}");

    lz4 +=
        "lib/lz4.c",
        "lib/lz4.h",
        "lib/lz4frame.c",
        "lib/lz4frame.h",
        "lib/lz4frame_static.h",
        "lib/lz4hc.c",
        "lib/lz4hc.h"
        ;
    lz4 ^=
        "lib/xxhash.c",
        "lib/xxhash.h"
        ;

    lz4.Public +=
        "lib"_id;
    lz4 += "org.sw.demo.Cyan4973.xxHash"_dep;

    if (lz4.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        lz4.Interface += sw::Shared, "LZ4_DLL_IMPORT"_d;
        lz4.Private += sw::Shared, "LZ4_DLL_EXPORT"_d;
    }
}
