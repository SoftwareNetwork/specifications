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

    auto add_addon = [&as](const String &name)
    {
        auto &t = as.addStaticLibrary(name);
        t.setRootDirectory("add_on/" + name);
        t += ".*"_r;
        t.Public += as;
    };

    for (auto &t : {
             "autowrapper",
             "contextmgr",
             "datetime",
             "debugger",
             "scriptany",
             "scriptarray",
             "scriptbuilder",
             "scriptdictionary",
             "scriptfile",
             "scriptgrid",
             "scripthandle",
             "scripthelper",
             "scriptmath",
             "scriptstdstring",
             "serializer",
             "weakref",
         })
    {
        add_addon(t);
    }
}
