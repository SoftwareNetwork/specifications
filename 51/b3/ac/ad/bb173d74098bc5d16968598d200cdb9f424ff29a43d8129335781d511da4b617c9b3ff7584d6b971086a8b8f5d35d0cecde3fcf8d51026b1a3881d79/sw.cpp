#pragma sw require header org.sw.demo.lexxmark.winflexbison.bison-master

void build(Solution &s)
{
    auto &DataManager = s.addLibrary("Polygon4.DataManager", "master");
    DataManager += Git("https://github.com/aimrebirth/DataManager", "", "{v}");

    auto &memory = DataManager.addStaticLibrary("memory");
    memory.CPPVersion = CPPLanguageStandard::CPP17;
    memory += "include/Polygon4/Memory.h", "src/memory/Memory.cpp";

    auto &schema = DataManager.addLibrary("schema");
    {
        schema.CPPVersion = CPPLanguageStandard::CPP17;
        schema.ApiName = "SCHEMA_API";
        schema += "include/.*"_rr;
        schema += "src/schema/.*\.h"_rr;
        schema += "src/schema/.*\.cpp"_rr;

        schema.Public += "include"_idir;
        schema += "include/Polygon4/DataManager/Schema"_idir;
        schema += "src/schema"_idir;

        schema.Public += "pub.egorpugin.primitives.filesystem-master"_dep;
        schema.Public += "pub.egorpugin.primitives.templates-master"_dep;
        schema.Public += "pub.egorpugin.primitives.context-master"_dep;
        schema.Public += "org.sw.demo.boost.algorithm-1"_dep;
        schema.Public += "org.sw.demo.boost.variant-1"_dep;
        schema.Public += memory;
        gen_flex_bison_pair(schema, "LALR1_CPP_VARIANT_PARSER", "src/schema/schema");
    }

    auto &generator = DataManager.addExecutable("tools.generator");
    generator.CPPVersion = CPPLanguageStandard::CPP17;
    generator += "src/generator/main.cpp";
    generator += schema;
    generator += "pub.egorpugin.primitives.sw.main-master"_dep;

    // DataManager
    {
        DataManager.CPPVersion = CPPLanguageStandard::CPP17;
        DataManager.ApiName = "DATA_MANAGER_API";
        DataManager += "include/.*"_rr;
        DataManager += "src/manager/.*\.h"_rr;
        DataManager += "src/manager/.*\.cpp"_rr;

        DataManager.Public += "include"_idir;
        DataManager += "src/manager"_idir;

        DataManager.Public += schema;
        DataManager.Public += "org.sw.demo.boost.log-1"_dep;
        DataManager.Public += "org.sw.demo.sqlite3-3"_dep;

        {
            auto c = DataManager.addCommand();
            c << cmd::prog(generator)
                << cmd::in("data/schema.txt")
                << DataManager.BinaryDir
                << cmd::end()
                ;
            for (auto o : {
                "include/detail/ForwardDeclarations.h",
                "include/detail/Types.h",
                "src/detail/Types.cpp",
                "include/detail/Storage.h",
                "src/detail/Storage.cpp",
                "include/detail/StorageImpl.h",
                "src/detail/StorageImpl.cpp",
                "src/detail/schema/Tokens.cpp",
                "include/detail/ObjectInterfaces.h",
                "include/detail/Enums.h",
                "src/detail/Enums.cpp",
                })
            {
                c << cmd::out(o);
                DataManager[o].skip = true;
            }
            DataManager.Public += IncludeDirectory(DataManager.BinaryDir / "include");
            DataManager += IncludeDirectory(DataManager.BinaryDir / "src");
        }

        if (s.Settings.Native.CompilerType == CompilerType::MSVC)
            DataManager.CompileOptions.push_back("-bigobj");
    }
}
