void build(Solution &s)
{
    // shared only at the moment
    auto &t = s.addTarget<SharedLibrary>("microsoft.mimalloc", "1.0.6");
    t += Git("https://github.com/microsoft/mimalloc", "v{v}");

    t += "include/.*"_rr;
    t -= "src/.*"_rr;
    t += "src/static.c";

    //if (t.getSettings().Native.LibrariesType == LibraryType::Shared)
    {
        t.Public += sw::Shared, "MI_SHARED_LIB"_def;
        t += sw::Shared, "MI_SHARED_LIB_EXPORT"_def;
        t += sw::Shared, "MI_MALLOC_OVERRIDE"_def;
        t.LinkOptions.push_back("-ENTRY:DllEntry");
        t += "src/alloc-override-win.c";
    }

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        t += "Advapi32.lib"_slib;
}
