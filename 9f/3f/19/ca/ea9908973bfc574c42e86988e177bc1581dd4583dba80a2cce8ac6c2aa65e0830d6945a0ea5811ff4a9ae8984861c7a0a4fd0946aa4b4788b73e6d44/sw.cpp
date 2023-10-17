void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("madler.zlib", "1.3.0");
    t += Git("https://github.com/madler/zlib", "v{M}.{m}{po}");

    //t += c89;
    t.setChecks("zlib", true);

    t += "[^/]*\\.[hc]"_rr;
    t += sw::Shared, "ZLIB_DLL"_def;

    if (!t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t.ExportAllSymbols = true;
        t += "HAVE_HIDDEN"_def;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("zlib");
    s.checkIncludeExists("unistd.h");
}
