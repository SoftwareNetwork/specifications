void build(Solution &s)
{
    auto &t = s.addStaticLibrary("google.cloud", "1.1.2");
    t += Git("https://github.com/google/crc32c");

    t += "include/.*"_rr;
    t += "src/.*"_rr;
    t -= "src/.*_benchmark.cc"_rr;
    t -= "src/.*_test_main.cc"_rr;
    t -= "src/.*_unittest.cc"_rr;
    t.writeFileOnce("crc32c/crc32c_config.h");
}
