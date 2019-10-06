void build(Solution &s)
{
    auto &t = s.addLibrary("sezero.mikmod", "3.3.11.1");
    t += Git("https://github.com/sezero/mikmod", "libmikmod-{v}");

    t.setRootDirectory("libmikmod");

    t += "include/.*"_rr;
    t += "depackers/.*"_rr;
    t += "drivers/.*"_r;
    t += "loaders/.*"_rr;
    t += "mmio/.*"_rr;
    t += "playercode/.*"_rr;
    t += "posix/strcasecmp.c";

    t += "MIKMOD_BUILD"_def;
    t += sw::Shared, "DLL_EXPORT"_def;
    if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
        t.Public += sw::Shared, "SYM_VISIBILITY"_def;
    t.Public += sw::Static, "MIKMOD_STATIC"_def;
}
