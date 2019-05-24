void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("google.brotli", "1.0.7");
    t += Git("https://github.com/google/brotli", "v{v}");
    t.setRootDirectory("c");

    t +=
        "common/.*\\.[hc]"_rr,
        "dec/.*\\.[hc]"_rr,
        "enc/.*\\.[hc]"_rr,
        "include/.*\\.[hc]"_rr
        ;

    t +=
        "common/.*\\.[hc]"_rr,
        "dec/.*\\.[hc]"_rr,
        "enc/.*\\.[hc]"_rr,
        "include/.*"_rr
        ;

    t.Public += sw::Shared, "BROTLI_SHARED_COMPILATION"_def;
    t += sw::Shared, "BROTLICOMMON_SHARED_COMPILATION"_def;
    t += sw::Shared, "BROTLIDEC_SHARED_COMPILATION"_def;
    t += sw::Shared, "BROTLIENC_SHARED_COMPILATION"_def;
}
