void build(Solution &s)
{
    auto &t = s.addStaticLibrary("libusb", "0.1.7");
    t += Git("https://github.com/libusb/libusb-compat-0.1", "v{v}");

    //t.ApiName = "API_EXPORTED";

    t += "libusb/.*"_rr;
    t.Public += "libusb"_idir;

    t += "API_EXPORTED="_def;

    t.writeFileOnce("config.h");
    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t.patch("libusb/core.c", "fmt...", "fmt, ...");
        t.patch("libusb/core.c", "__attribute__ ((constructor))", "");
        t.patch("libusb/core.c", "__attribute__ ((destructor))", "");
        t.writeFileOnce("unistd.h");
    }

    t += "org.sw.demo.libusb"_dep;
    t += "org.sw.demo.tronkko.dirent-master"_dep;
}
