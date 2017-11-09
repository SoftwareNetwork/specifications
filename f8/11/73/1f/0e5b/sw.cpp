void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &bzip2 = s.addTarget<LibraryTarget>("bzip2", "1.0.6");
    bzip2.Source = RemoteFile("http://www.bzip.org/1.0.6/bzip2-1.0.6.tar.gz");
    bzip2 +=
        "blocksort.c",
        "bzlib.[hc]"_r,
        "bzlib_private.h",
        "compress.c",
        "crctable.c",
        "decompress.c",
        "huffman.c",
        "randtable.c"
        ;
    bzip2.Public += sw::Shared, "BZ_SHARED"_d;
    bzip2.Interface += sw::Shared, "BZ_IMPORT"_d;

    bzip2.replaceInFileOnce("bzlib.h",
        "#ifdef _WIN32",
        R"(
#ifdef _WIN32
#include <windows.h>
#ifdef small
#undef small
#endif
#endif
#if defined(_WIN32) && defined(BZ_SHARED)
)"
);
    bzip2.replaceInFileOnce("bzlib.h",
        "#   define BZ_API(func) WINAPI func\n#   define BZ_EXTERN extern",
        "#   define BZ_API(func) func\n#   define BZ_EXTERN __declspec(dllexport)"
    );
    bzip2.replaceInFileOnce("bzlib.h",
        "#   define BZ_API(func) (WINAPI * func)\n#   define BZ_EXTERN",
        "#   define BZ_API(func) func\n#   define BZ_EXTERN __declspec(dllimport)"
    );
}
