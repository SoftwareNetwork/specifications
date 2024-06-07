static int merge_sql(FilesOrdered files, path o)
{
    String out;
    for (auto &f : files)
        out += read_file(f);
    write_file(o, out);
    return 0;
}
SW_DEFINE_VISIBLE_FUNCTION_JUMPPAD2(merge_sql)

void build(Solution &s)
{
    auto &t = s.addLibrary("OSGeo.PROJ", "9.4.1");
    t += Git("https://github.com/OSGeo/PROJ");
    {
        t += cpp17;
        //t.ApiName = "PROJ_DLL";
        ////t.ApiName = "GEOD_DLL";
        ////t.ApiName = "PROJ_MSVC_DLL";
        ////t.ApiName = "PROJ_GCC_DLL";
        t.setChecks("proj");

        t += "include/.*"_rr;
        t += "src/.*"_r;
        t += "src/conversions/.*"_rr;
        t += "src/iso19111/.*"_rr;
        t += "src/projections/.*"_rr;
        t += "src/transformations/.*"_rr;

        // data files
        t += "data/sql/.*"_r;
        t += "data/sql_filelist.cmake";

        t.Public += "include"_idir;
        t.Public += "src"_idir;

        t += "org.sw.demo.sqlite3"_dep;

        t.configureFile("cmake/proj_config.cmake.in", "proj_config.h", ConfigureFlags::EnableUndefReplacements);

        if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            t += sw::Shared, "PROJ_MSVC_DLL_EXPORT"_def;
            if (t.getBuildSettings().Native.LibrariesType == LibraryType::Static) {
                t += "PROJ_DLL"_api;
            }
            t += "NOMINMAX"_def;
            t += "ole32.lib"_slib;
            t += "shell32.lib"_slib;
        }

        // proj.db
        auto datadir = t.SourceDir / "data";
        if (fs::exists(datadir / "sql_filelist.cmake"))
        {
            auto f = read_file(datadir / "sql_filelist.cmake");
            std::regex r(R"xxx(\"\$\{SQL_DIR\}/(.*?\.sql)\")xxx");
            std::smatch m;
            FilesOrdered files;
            while (std::regex_search(f, m, r))
            {
                files.push_back(datadir / "sql" / m[1].str());
                f = m.suffix().str();
            }

            {
                auto c = t.addCommand(SW_VISIBLE_FUNCTION(merge_sql));
                auto bc = std::dynamic_pointer_cast<BuiltinCommand>(c.getCommand());
                bc->push_back(files);
                for (auto &f : files)
                    c->addInput(f);
                c << cmd::out("all.sql");
            }
        }

        {
            auto c = t.addCommand();
            c << cmd::prog("org.sw.demo.sqlite3.shell"_dep)
                << "-init"
                << cmd::in("all.sql")
                << cmd::out("proj.db")
                << ".quit";
        }
        t += Definition("PROJ_LIB=\"" + to_string(to_path_string(normalize_path(t.BinaryDir))) + "\"");
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("proj");
    s.checkLibraryFunctionExists("exp", "m");
    s.checkIncludeExists("stdlib.h");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkFunctionExists("localeconv");
    s.checkFunctionExists("strerror");
    s.checkLibraryFunctionExists("dladdr", "dl");
    s.checkLibraryFunctionExists("pthread_create", "pthread");
    s.checkLibraryFunctionExists("pthread_mutexattr_settype", "pthread");
    s.checkDeclarationExists("PTHREAD_MUTEX_RECURSIVE");
    s.checkLibraryFunctionExists("curl_global_init", "curl");
}
