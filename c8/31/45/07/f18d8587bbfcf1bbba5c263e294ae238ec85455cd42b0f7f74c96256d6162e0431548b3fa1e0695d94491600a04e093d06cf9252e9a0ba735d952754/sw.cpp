void build(Solution &s)
{
    auto &t = s.addSharedLibrary("signal11.hidapi", "master");
    t += Git("https://github.com/signal11/hidapi");

    t += "hidapi/.*"_rr;
    t -= "windows/hid.c";
    t -= "libusb/hid.c";

    t.Public += "hidapi"_idir;

    t -= "org.sw.demo.libusb"_dep;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t += "windows/hid.c";
        t += "Setupapi.lib"_slib;
    }
    else
    {
        t.ExportAllSymbols = true;
        //t += "linux/hid.c";
        t += "libusb/hid.c";
        t += "org.sw.demo.libusb"_dep;
        t += "pthread"_slib;
    }
}
