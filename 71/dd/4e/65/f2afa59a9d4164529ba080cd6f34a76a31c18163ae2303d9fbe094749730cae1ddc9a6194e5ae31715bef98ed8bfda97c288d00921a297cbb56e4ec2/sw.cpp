void build(Solution &s)
{
    auto &t = s.addStaticLibrary("Corvusoft.restbed", "4.7.0");
    t += Git("https://github.com/Corvusoft/restbed", "{M}.{m}");

    t.Public += "org.sw.demo.chriskohlhoff.asio-1.12"_dep;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        t += "ws2_32.lib"_slib, "Mswsock.lib"_slib;
}
