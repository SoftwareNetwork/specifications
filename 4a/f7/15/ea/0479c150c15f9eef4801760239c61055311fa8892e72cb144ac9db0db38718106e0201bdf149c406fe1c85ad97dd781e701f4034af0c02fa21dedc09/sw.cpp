void build(Solution &s)
{
    auto &as = s.addLibrary("angelscript", "2.34.0");
    as += RemoteFile("http://www.angelcode.com/angelscript/sdk/files/angelscript_{v}.zip");

    {
        auto &t = as;
        t.setRootDirectory("angelscript");

        t += sw::Shared, "ANGELSCRIPT_EXPORT"_def;
        t.Interface += sw::Shared, "ANGELSCRIPT_DLL_LIBRARY_IMPORT"_def;

        t -= "source/as_callfunc_arm_msvc.asm";
    }

    auto &addons = as.addStaticLibrary("addons");
    addons.setRootDirectory("add_on");
    addons.Public += as;

    for (auto &d : fs::directory_iterator(addons.SourceDir))
        addons += FileRegex(d, ".*", false);
}
