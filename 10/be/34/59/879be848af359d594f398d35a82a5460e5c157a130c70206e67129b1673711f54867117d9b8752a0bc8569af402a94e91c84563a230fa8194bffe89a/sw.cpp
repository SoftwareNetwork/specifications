void build(Solution &s)
{
    auto &abseil = s.addProject("google.abseil", "master");
    abseil += Git("https://github.com/abseil/abseil-cpp", "", "{v}");

    auto add_target = [&abseil](const String &name, bool header_only = false) -> decltype(auto)
    {
        auto &t = abseil.addStaticLibrary(name);
        SwapAndRestore sr(t.SourceDir, t.SourceDir / "absl" / name);
        if (!header_only)
            t += ".*\\.cc"_rr;
        t += ".*\\.h"_rr;
        t += ".*\\.inc"_rr;
        if (!header_only)
        {
            t -= ".*_benchmark.*cc"_rr;
            t -= ".*_test.*cc"_rr;
            t -= ".*_nonprod.*cc"_rr;
            t -= ".*/test_.*cc"_rr;
        }
        return t;
    };

    auto &base = add_target("base");
    base.Public.IncludeDirectories.push_back(abseil.SourceDir);
    base.Public += "NOMINMAX"_d;

    auto &strings = add_target("strings");
    strings.Public += base;

    auto &algorithm = add_target("algorithm");
    algorithm.Public += base;

    auto &container = add_target("container");
    container.Public += algorithm;

    auto &numeric = add_target("numeric");
    numeric.Public += base;

    auto &synchronization = add_target("synchronization");
    synchronization -= "absl/synchronization/internal/waiter.cc";
    synchronization.Public += base;

    auto &time = add_target("time");
    time.Public += base;

    auto &types = add_target("types");
    types.Public += base;

    auto &utility = add_target("utility", true);
    utility.Public += base;
}
