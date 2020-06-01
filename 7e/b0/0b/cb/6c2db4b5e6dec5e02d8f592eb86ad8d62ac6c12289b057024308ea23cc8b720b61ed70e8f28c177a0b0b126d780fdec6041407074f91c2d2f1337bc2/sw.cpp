void build(Solution &s)
{
    auto &p = s.addProject("systemd", "245");
    p += Git("https://github.com/systemd/systemd", "v{M}");

    // not working
    auto &basic = p.addStaticLibrary("basic");
    {
        basic.setRootDirectory("src/basic");
        basic += ".*"_rr;
        basic += "RELATIVE_SOURCE_PATH=\"\""_def;
    }

    auto &udev = p.addLibrary("udev");
    {
        udev.setRootDirectory("src/libudev");
        udev += ".*"_rr;
        udev.Public += basic;
    }
}
