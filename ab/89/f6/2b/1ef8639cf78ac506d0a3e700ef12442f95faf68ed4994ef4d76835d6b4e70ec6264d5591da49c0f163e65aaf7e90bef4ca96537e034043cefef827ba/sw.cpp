void build(Solution &s)
{
    auto &p = s.addProject("census.opencensus", "0.4.0");
    p += Git("https://github.com/census-instrumentation/opencensus-cpp", "v{v}");

    auto add_dir = [](auto &t, const path &dir)
    {
        t += FileRegex(path("opencensus") / dir, ".*\\.h", false);
        t.AllowEmptyRegexes = true;
        t += FileRegex(path("opencensus") / dir / "internal", ".*", false);
        t -= FileRegex(path("opencensus") / dir / "internal", ".*_test\\.cc", false);
        t -= FileRegex(path("opencensus") / dir / "internal", ".*_benchmark\\.cc", false);
        t -= FileRegex(path("opencensus") / dir / "internal", ".*_fuzzer\\.cc", false);
        t.AllowEmptyRegexes = false;
    };

    auto &cpp = p.addStaticLibrary("cpp");
    cpp += cpp17;
    add_dir(cpp, "common");
    add_dir(cpp, "context");
    add_dir(cpp, "stats");
    add_dir(cpp, "tags");
    add_dir(cpp, "trace");
    add_dir(cpp, "trace/exporter");
    add_dir(cpp, "trace/propagation");
    cpp.writeFileOnce("sw_opencensus_fi.h", R"(
#define ACQUIRED_AFTER(...)
#define GUARDED_BY(...)
#define LOCKS_EXCLUDED(...)
#define EXCLUSIVE_LOCKS_REQUIRED(...)
)");
#if SW_MODULE_ABI_VERSION >= 34
    cpp.Public += ForceIncludeFile{"sw_opencensus_fi.h"s};
#else
    if (cpp.getCompilerType() == CompilerType::MSVC) {
        cpp.Public.CompileOptions.push_back("-FIsw_opencensus_fi.h");
    } else {
        cpp.Public.CompileOptions.push_back("-includesw_opencensus_fi.h");
    }
#endif
    cpp.Public += "org.sw.demo.google.abseil"_dep;
}
