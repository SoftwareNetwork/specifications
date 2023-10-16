void build(Solution &s)
{
    auto &t = s.addStaticLibrary("opentelemetry", "1.12.0");
    t += Git("https://github.com/open-telemetry/opentelemetry-cpp", "v{v}");

    t += cpp11;
    t += "api/include/.*"_rr;
    t += "sdk/include/.*"_rr;
    t += "sdk/src/.*"_rr;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows)) {
        t -= "sdk/src/common/platform/fork_unix.cc";
    } else {
        t -= "sdk/src/common/platform/fork_windows.cc";
    }

    t.Public += "api/include"_idir;
    t += "sdk/include"_idir;
    t += "sdk"_idir;
}
