void build(Solution &s)
{
    auto &p = s.addProject("census.opencensus.cpp", "0.4.0");
    p += Git("https://github.com/census-instrumentation/opencensus-cpp", "v{v}");

    auto add_dir = [](auto &t, const path &dir)
    {
        t += FileRegex(path("opencensus") / dir, ".*\\.h", false);
        t += FileRegex(path("opencensus") / dir / "internal", ".*", true);
        t -= FileRegex(path("opencensus") / dir / "internal", ".*_test\\.cc", true);
        t -= FileRegex(path("opencensus") / dir / "internal", ".*_benchmark\\.cc", true);
    };

    auto &cpp = p.addStaticLibrary("cpp");
    add_dir(cpp, "stats");
    add_dir(cpp, "trace");
    cpp.Public += "org.sw.demo.google.abseil"_dep;
}
