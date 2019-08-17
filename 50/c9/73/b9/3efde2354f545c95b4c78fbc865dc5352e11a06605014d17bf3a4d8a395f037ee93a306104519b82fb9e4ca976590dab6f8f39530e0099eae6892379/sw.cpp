void build(Solution &s)
{
    auto &abseil = s.addStaticLibrary("google.abseil", "2019.08.08");
    abseil += Git("https://github.com/abseil/abseil-cpp", "{M}{m:02}{p:02}");

    {
        auto &t = abseil;
        SwapAndRestore sr(t.SourceDir, t.SourceDir / "absl");
        t += ".*\\.cc"_rr;
        t += ".*\\.h"_rr;
        t += ".*\\.inc"_rr;
        t -= ".*_benchmark.*cc"_rr;
        t -= ".*_test.*cc"_rr;
        t -= ".*_generator.*cc"_rr;
        t -= ".*_gentables.*cc"_rr;
        t -= ".*_nonprod.*cc"_rr;
        t -= ".*/test_.*cc"_rr;
        t -= "hash/internal/print_hash_of.cc";
        t -= "random/benchmarks.cc";
        t -= "random/internal/named_generator.cc";
        t -= "synchronization/internal/waiter.cc";
    }

    abseil.Public += "NOMINMAX"_d;
}
