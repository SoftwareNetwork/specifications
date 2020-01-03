void build(Solution &s)
{
    auto &t = s.addLibrary("libusb", "1.0.23");
    t += Git("https://github.com/libusb/libusb", "v{v}");
    {
        t += "libusb/.*"_r;
        t -= "libusb/os/.*"_rr;
        t ^= "libusb/libusb-1.0.rc";
        t -= "msvc/.*\\.h"_rr;

        if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            t += "msvc"_idir;
            t += "libusb/os/.*windows.*"_rr;
            t += "Advapi32.lib"_slib;
            t.patch("libusb/libusb-1.0.def", "LIBRARY \"libusb-1.0.dll\"", "LIBRARY");
        }

        t.Public += "libusb"_idir;
    }
}
