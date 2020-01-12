void build(Solution &s)
{
    auto &t = s.addLibrary("angelscript", "2.34.0");
    t += RemoteFile("http://www.angelcode.com/angelscript/sdk/files/angelscript_{v}.zip");

    t.setRootDirectory("angelscript");

    t += sw::Shared, "ANGELSCRIPT_EXPORT"_def;
    t.Interface += sw::Shared, "ANGELSCRIPT_DLL_LIBRARY_IMPORT"_def;

    t -= "source/as_callfunc_arm_msvc.asm";
}
