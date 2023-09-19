void build(Solution &s)
{
    auto &abseil = s.addStaticLibrary("google.abseil", "2023.1.25.3");
    abseil += Git("https://github.com/abseil/abseil-cpp", "{M}{m:02}{p:02}.{t}");

    {
        auto &t = abseil;
        t += cpp17;
        SwapAndRestore sr(t.SourceDir, t.SourceDir / "absl");
        t += ".*\\.cc"_rr;
        t += ".*\\.h"_rr;
        t += ".*\\.inc"_rr;
        //t -= ".*_benchmark.*cc"_rr;
        // this includes removal of nanobenchmark
        // nanobenchmark.c does not build in x86
        t -= ".*benchmark.*cc"_rr;
        t -= ".*_test.*cc"_rr;
        t -= ".*_mock.*cc"_rr;
        t -= ".*_generator.*cc"_rr;
        t -= ".*_gentables.*cc"_rr;
        //t -= ".*_nonprod.*cc"_rr;
        t -= ".*/test_.*cc"_rr;
        t -= "hash/internal/print_hash_of.cc";
        t -= "random/benchmarks.cc";
        //t -= "random/mocking_bit_gen.cc";
        //t -= "synchronization/internal/waiter.cc";
        t += "random/internal/distribution_test_util.cc";

        abseil.Public += "ABSL_LEGACY_THREAD_ANNOTATIONS"_def;

        if (abseil.getBuildSettings().TargetOS.is(OSType::Windows) || abseil.getBuildSettings().TargetOS.is(OSType::Mingw))
        {
            abseil.Public += "NOMINMAX"_def;
            abseil += "advapi32.lib"_slib;
            abseil += "Dbghelp.lib"_slib;
        }
        abseil += "CoreFoundation"_framework;
    }
}
