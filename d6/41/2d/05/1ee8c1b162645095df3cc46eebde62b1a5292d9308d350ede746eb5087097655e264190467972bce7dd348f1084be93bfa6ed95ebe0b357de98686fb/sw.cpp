void build(Solution &s)
{
    auto &p = s.addTarget<Library>("Blosc.c_blosc2", "2.5.0");
    p += Git("https://github.com/Blosc/c-blosc2", "v{v}");

    p -= "blosc/.*"_rr;
    p += "blosc/.*"_r;
    p -= "blosc/.*altivec.*"_rr;
    p -= "blosc/.*neon.*"_rr;
    p += "include/.*"_rr;
    p.Public += "include"_idir;
    p += "include/blosc2"_idir;

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
    p.writeFileOnce("include/blosc2/blosc2-export.h", "");
    p.configureFile("blosc/config.h.in", p.BinaryPrivateDir / "config.h");
}
