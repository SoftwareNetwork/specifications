#pragma sw require header org.sw.demo.lexxmark.winflexbison.bison

void build(Solution &s)
{
    auto &DataManager = s.addLibrary("Polygon4.DataManager", "master");
    DataManager += Git("https://github.com/aimrebirth/DataManager", "", "{v}");

    auto &memory = DataManager.addStaticLibrary("memory");
    {
        memory += cpp20;
        memory += "include/Polygon4/Memory.h", "src/memory/Memory.cpp";
        if (memory.getOptions()["alligned-allocator"] == "1")
        {
            memory.getOptions()["alligned-allocator"].use();
            memory += "DATA_MANAGER_ALIGNED_ALLOCATOR"_def;
        }
    }

    auto &schema = DataManager.addLibrary("schema");
    {
        schema += cpp20;
        schema.ApiName = "SCHEMA_API";
        schema += "include/.*"_rr;
        schema += "src/schema/.*\\.h"_rr;
        schema += "src/schema/.*\\.cpp"_rr;

        schema.Public += "include"_idir;
        schema += "include/Polygon4/DataManager/Schema"_idir;
        schema += "src/schema"_idir;

        schema.Public += "pub.egorpugin.primitives.filesystem-master"_dep;
        schema.Public += "pub.egorpugin.primitives.templates-master"_dep;
        schema.Public += "pub.egorpugin.primitives.emitter-master"_dep;
        schema.Public += "org.sw.demo.boost.algorithm"_dep;
        schema.Public += "org.sw.demo.boost.variant"_dep;
        schema.Public += memory;
        gen_flex_bison_pair("org.sw.demo.lexxmark.winflexbison"_dep, schema, "LALR1_CPP_VARIANT_PARSER", "src/schema/schema");
    }

    auto &generator = DataManager.addExecutable("tools.generator");
    generator += cpp20;
    generator += "src/generator/main.cpp";
    generator += schema;
    generator += "pub.egorpugin.primitives.sw.main-master"_dep;

    // DataManager
    {
        DataManager += cpp20;
        DataManager.ApiName = "DATA_MANAGER_API";
        DataManager += "include/.*"_rr;
        DataManager += "src/manager/.*\\.cpp"_rr;

        DataManager.Public += "include"_idir;
        DataManager += "src/manager"_idir;

        DataManager.Public += schema;
        DataManager.Public += "org.sw.demo.sqlite3-3"_dep;
        DataManager.Public += "pub.egorpugin.primitives.log-master"_dep;

        {
            auto c = DataManager.addCommand();
            c << cmd::prog(generator)
                << cmd::in("data/schema.txt")
                << DataManager.BinaryDir
                << cmd::end()
                ;
            for (path o : {
                "include/detail/ForwardDeclarations.h",
                "include/detail/Types.h",
                "include/detail/Storage.h",
                "include/detail/StorageImpl.h",
                "include/detail/ObjectInterfaces.h",
                "include/detail/Enums.h",
                })
            {
                c << cmd::out(o);
            }
            for (path o : {
                "src/detail/Types.cpp",
                "src/detail/Storage.cpp",
                "src/detail/StorageImpl.cpp",
                "src/detail/schema/Tokens.cpp",
                "src/detail/Enums.cpp",
                })
            {
                c << cmd::out(o);
                DataManager[o].skip = true;
            }
            DataManager.Public += IncludeDirectory(DataManager.BinaryDir / "include");
            DataManager += IncludeDirectory(DataManager.BinaryDir / "src");
        }

        if (DataManager.getCompilerType() == CompilerType::MSVC)
            DataManager.CompileOptions.push_back("-bigobj");
    }
}
