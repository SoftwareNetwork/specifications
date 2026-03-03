void build(Solution &s)
{
    auto &abseil = s.addStaticLibrary("google.abseil", "2026.1.7.1");
    abseil += Git("https://github.com/abseil/abseil-cpp", "{M}{m:02}{p:02}.{t}");

    {
        auto &t = abseil;
        t.ExportAllSymbols = true;
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
        t -= "status/internal/status_matchers.cc";
        t -= "random/benchmarks.cc";
        //t -= "random/mocking_bit_gen.cc";
        //t -= "synchronization/internal/waiter.cc";
        t += "random/internal/distribution_test_util.cc";
        t -= "time/internal/cctz/src/time_zone_name_win.cc";

        t.Public += "ABSL_LEGACY_THREAD_ANNOTATIONS"_def;

        if (t.getBuildSettings().TargetOS.is(OSType::Windows) || t.getBuildSettings().TargetOS.is(OSType::Mingw))
        {
            t += "time/internal/cctz/src/time_zone_name_win.cc";
            t.Public += "NOMINMAX"_def;
            t += "advapi32.lib"_slib;
            t += "Dbghelp.lib"_slib;
            t += "com.Microsoft.Windows.SDK.winrt"_dep;
        }
        t += "CoreFoundation"_framework;
    }
}
