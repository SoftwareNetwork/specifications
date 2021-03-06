void build(Solution &s)
{
    auto &p = s.addProject("facebook.zstd", "1.4.9");
    p += Git("https://github.com/facebook/zstd", "v{v}");

    auto &lib = p.addLibrary("zstd");
    lib +=
        "lib/common/.*"_rr,
        "lib/compress/.*"_rr,
        "lib/decompress/.*"_rr,
        "lib/deprecated/.*"_rr,
        "lib/zstd.h"
        ;
    lib +=
        "lib/common"_idir,
        "lib/compress"_idir,
        "lib/decompress"_idir,
        "lib/deprecated"_idir
        ;
    lib.Public += "lib"_idir;

    if (lib.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        lib += sw::Shared, "ZSTD_DLL_EXPORT"_def;
        lib.Interface += sw::Shared, "ZSTD_DLL_IMPORT"_def;
    }
}
