void build(Solution &s)
{
    auto &t = s.addLibrary("konstanty.libmodplug", "master");
    t += Git("https://github.com/Konstanty/libmodplug");

    t += "src/.*"_rr;
    t += "src/libmodplug"_idir;

    t += "MODPLUG_BUILD"_def;
    t += sw::Shared, "DLL_EXPORT"_def;
    if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
        t.Public += sw::Shared, "SYM_VISIBILITY"_def;
    t.Public += sw::Static, "MODPLUG_STATIC"_def;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        t += "User32.lib"_slib;

    if (!t.DryRun)
    {
        auto dummy = t.BinaryDir / "private/sw_copy_headers.txt";
        if (!fs::exists(dummy))
        {
            auto dir2 = t.BinaryDir / "libmodplug";
            fs::create_directories(dir2);
            fs::copy_file(t.SourceDir / "src" / "modplug.h", dir2 / "modplug.h", fs::copy_options::overwrite_existing);
            write_file(dummy, "");
        }
    }
}
