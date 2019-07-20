void build(Solution &s)
{
    // shared only at the moment
    auto &t = s.addTarget<SharedLibrary>("microsoft.mimalloc", "1.0.5");
    t += Git("https://github.com/microsoft/mimalloc", "v{v}");

    t += "include/.*"_rr;
    t -= "src/.*"_rr;
    t += "src/static.c";

    t.Public += sw::Shared, "MI_SHARED_LIB"_def;
    t += sw::Shared, "MI_SHARED_LIB_EXPORT"_def;

    if (t.getSettings().TargetOS.is(OSType::Windows))
        t += "Advapi32.lib"_slib;
}
