void build(Solution &s)
{
    auto &p = s.addProject("facebook.zstd", "1.5.7");
    p += Git("https://github.com/facebook/zstd", "v{v}");

    auto &lib = p.addLibrary("zstd");
    lib +=
        "lib/common/.*"_rr,
        "lib/compress/.*"_rr,
        "lib/dictBuilder/.*"_rr,
        "lib/decompress/.*"_rr,
        "lib/deprecated/.*"_rr,
        "lib/.*\\.h"_r
        ;
    lib +=
        "lib/common"_idir,
        "lib/compress"_idir,
        "lib/dictBuilder"_idir,
        "lib/decompress"_idir,
        "lib/deprecated"_idir
        ;
    lib.Public += "lib"_idir;

    if (lib.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        // for clang on windows
        lib += "ZSTD_NO_INTRINSICS"_def;
        //lib += "ZSTD_ARCH_X86_SSE2"_def;
        //lib.CompileOptions.push_back("-msse2");
        //lib.CompileOptions.push_back("-march=native");

        lib += sw::Shared, "ZSTD_DLL_EXPORT"_def;
        lib.Interface += sw::Shared, "ZSTD_DLL_IMPORT"_def;
    }
}
