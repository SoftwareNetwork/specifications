void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &lz4 = s.addTarget<LibraryTarget>("lz4", "1.8.0");
    lz4.Source = Git("https://github.com/Cyan4973/lz4", "v{v}");

    lz4 +=
        "lib/lz4.c",
        "lib/lz4.h",
        "lib/lz4frame.c",
        "lib/lz4frame.h",
        "lib/lz4frame_static.h",
        "lib/lz4hc.c",
        "lib/lz4hc.h",
        "lib/lz4opt.h",
        "lib/xxhash.c",
        "lib/xxhash.h";

    lz4.Public +=
        "lib"_id;

    lz4.Interface += sw::Shared, "LZ4_DLL_IMPORT"_d;
    lz4.Private += sw::Shared, "LZ4_DLL_EXPORT"_d;
}
