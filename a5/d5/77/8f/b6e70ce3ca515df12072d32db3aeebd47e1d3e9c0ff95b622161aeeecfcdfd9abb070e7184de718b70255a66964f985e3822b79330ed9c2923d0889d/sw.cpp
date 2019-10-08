void build(Solution &s)
{
    auto &t = s.addLibrary("sezero.libtimidity", "0.2.6");
    t += Git("https://github.com/sezero/libtimidity", "libtimidity-{v}");

    t += "TIMIDITY_BUILD"_def;
    t += sw::Shared, "DLL_EXPORT"_def;
    if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
        t.Public += sw::Shared, "SYM_VISIBILITY"_def;
    t.Public += sw::Static, "TIMIDITY_STATIC"_def;
}
