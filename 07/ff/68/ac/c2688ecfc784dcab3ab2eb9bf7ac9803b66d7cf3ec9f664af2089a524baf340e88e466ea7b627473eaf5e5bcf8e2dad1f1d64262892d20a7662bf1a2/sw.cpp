void build(Solution &s)
{
    auto &t = s.addTarget<Library>("nanomsg.nng", "1.1.1");
    t += Git("https://github.com/nanomsg/nng", "v{v}");

    t.ApiName = "NNG_DECL";

    t -= "src/.*zerotier.*"_rr;
    t -= "src/.*stub.*"_rr;
    t -= "src/.*none.*"_rr;

    if (s.Settings.TargetOS.is(OSType::Windows))
    {
        t -= "src/.*posix.*"_rr;
        t.Public += "NNG_PLATFORM_WINDOWS"_def;
        t += "ws2_32.lib"_slib;
        t += "Advapi32.lib"_slib;
    }
    else
    {
        t -= "src/.*windows.*"_rr;
        t.Public += "NNG_PLATFORM_POSIX"_def;
    }

    t += "org.sw.demo.mbed.tls"_dep;
}
