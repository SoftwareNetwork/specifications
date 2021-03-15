void build(Solution &s)
{
    auto &t = s.addStaticLibrary("quickfix.quickfix", "1.15.1");
    t += Git("https://github.com/quickfix/quickfix", "v{v}");

    t.setRootDirectory("src/C++");
    t += ".*\\.cpp"_r;
    t += ".*\\.hp?p?"_r;
    t -= "double-conversion/.*"_r;
    t -= "fix.*"_rr;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        t += "ws2_32.lib"_slib;
}
