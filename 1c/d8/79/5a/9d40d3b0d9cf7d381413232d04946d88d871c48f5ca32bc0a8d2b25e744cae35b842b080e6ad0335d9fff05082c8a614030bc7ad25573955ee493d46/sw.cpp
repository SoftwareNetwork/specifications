void build(Solution &s)
{
    auto &abseil = s.addStaticLibrary("google.abseil", "master");
    abseil += Git("https://github.com/abseil/abseil-cpp", "", "{v}");

    {
        auto &t = abseil;
        SwapAndRestore sr(t.SourceDir, t.SourceDir / "absl");
        t += ".*\\.cc"_rr;
        t += ".*\\.h"_rr;
        t += ".*\\.inc"_rr;
        t -= ".*_benchmark.*cc"_rr;
        t -= ".*_test.*cc"_rr;
        t -= ".*_nonprod.*cc"_rr;
        t -= ".*/test_.*cc"_rr;
        t -= "synchronization/internal/waiter.cc";
    }

    abseil.Public += "NOMINMAX"_d;
}
