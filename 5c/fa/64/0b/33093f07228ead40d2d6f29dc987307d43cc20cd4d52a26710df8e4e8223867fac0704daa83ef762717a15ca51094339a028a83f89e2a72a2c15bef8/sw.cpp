void build(Solution &s)
{
    auto &t = s.addStaticLibrary("opentelemetry", "1.24.0");
    t += Git("https://github.com/open-telemetry/opentelemetry-cpp", "v{v}");

    t += cpp17;
    t += "api/include/.*"_rr;
    t += "sdk/include/.*"_rr;
    t += "sdk/src/.*"_rr;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows)) {
        t -= "sdk/src/common/platform/fork_unix.cc";
    } else {
        t -= "sdk/src/common/platform/fork_windows.cc";
    }

    t.Public += "api/include"_idir;
    t.Public += "sdk/include"_idir;
    t += "sdk"_idir;

    t.Public += "org.sw.demo.biojppm.rapidyaml"_dep;

    t.patch("sdk/src/configuration/ryml_document.cc",
        "auto ryml_loc = parser_->location(node);",
        "auto ryml_loc = node.location(*parser_);"
    );

    auto &e = t.addStaticLibrary("exporters.ostream");
    e.setRootDirectory("exporters/ostream");
    e.Public += t;
}
