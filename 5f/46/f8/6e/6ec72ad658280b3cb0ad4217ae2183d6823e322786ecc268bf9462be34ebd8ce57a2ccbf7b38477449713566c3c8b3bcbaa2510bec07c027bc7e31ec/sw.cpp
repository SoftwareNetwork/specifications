void build(Solution &s)
{
    auto &t = s.addExecutable("dmage.co2mon", "master");
    t += Git("https://github.com/dmage/co2mon");

    auto &lib = t.addLibrary("lib");
    {
        lib.ExportAllSymbols = true;
        lib.setRootDirectory("libco2mon");
        lib.Public += "org.sw.demo.libusb.hidapi"_dep;
    }

    t.setRootDirectory("co2mond");
    t += lib;
}
