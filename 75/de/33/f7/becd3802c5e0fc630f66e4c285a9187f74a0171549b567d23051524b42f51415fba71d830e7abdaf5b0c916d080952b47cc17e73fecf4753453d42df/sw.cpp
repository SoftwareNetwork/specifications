void build(Solution &s)
{
    auto &p = s.addTarget<Library>("Blosc.c_blosc", "1.16.3");
    p += Git("https://github.com/Blosc/c-blosc", "v{v}");

    p -= "blosc/.*"_rr;
    p += "blosc/.*"_r;

    p.CompileOptions.push_back("-arch:AVX2");

    p += "HAVE_LZ4"_def;
    p += "HAVE_SNAPPY"_def;
    p += "HAVE_ZLIB"_def;
    p += "HAVE_ZSTD"_def;

    p += "org.sw.demo.madler.zlib"_dep;
    p += "org.sw.demo.google.snappy"_dep;
    p += "org.sw.demo.facebook.zstd.zstd"_dep;
    p += "org.sw.demo.lz4"_dep;

    p.ApiName = "BLOSC_EXPORT";
    p.Public += "BLOSC_NO_EXPORT="_def;
    p.writeFileOnce("blosc/blosc-export.h", "");
}
