void build(Solution &s)
{
    auto &t = s.addLibrary("hunspell.hunspell", "1.7.2");
    t += Git("https://github.com/hunspell/hunspell", "v{v}");
    {
        t += cpp17;
        t += "src/hunspell/.*"_rr;
        t.Public += "src"_idir;
        t += "BUILDING_LIBHUNSPELL"_def;
        if (t.getBuildSettings().TargetOS.is(OSType::Windows))
            t += sw::Static, "HUNSPELL_STATIC"_def;
    }
}
