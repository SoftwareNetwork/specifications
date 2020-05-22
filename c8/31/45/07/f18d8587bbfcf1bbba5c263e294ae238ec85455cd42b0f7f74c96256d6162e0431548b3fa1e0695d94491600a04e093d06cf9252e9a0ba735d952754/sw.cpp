void build(Solution &s)
{
    auto &t = s.addSharedLibrary("signal11.hidapi", "master");
    t += Git("https://github.com/signal11/hidapi");

    t += "hidapi/.*"_rr;
    t -= "windows/hid.c";
    t.Public += "hidapi"_idir;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t += "windows/hid.c";
        t += "Setupapi.lib"_slib;
    }
}
