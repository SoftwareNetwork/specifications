void build(Solution &s)
{
    auto &t = s.addStaticLibrary("quickfix.quickfix", "1.15.1");
    t += Git("https://github.com/quickfix/quickfix", "v{v}");

    t += "src/config_windows.h";
    t.setRootDirectory("src/C++");
    t += ".*\\.cpp"_r;
    t += ".*\\.hp?p?"_r;
    t -= "double-conversion/.*"_r;
    t -= "fix.*"_rr;

    t.Public += "HAVE_STD_UNIQUE_PTR"_def;
    t.Public += "HAVE_STD_SHARED_PTR"_def;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        t += "ws2_32.lib"_slib;

    if (!fs::exists(t.SourceDir / "quickfix"))
        fs::create_directory_symlink(t.SourceDir / ".", t.SourceDir / "quickfix");
}
