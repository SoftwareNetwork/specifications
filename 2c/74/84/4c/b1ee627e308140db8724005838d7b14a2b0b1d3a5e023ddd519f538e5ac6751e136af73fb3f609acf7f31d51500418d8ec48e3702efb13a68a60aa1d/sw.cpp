void build(Solution &s)
{
    auto &t = s.addLibrary("OSGeo.PROJ", "7.0.1");
    t += Git("https://github.com/OSGeo/PROJ");
    {
        t.ApiName = "PROJ_DLL";
        t.setChecks("proj");

        t += "include/.*"_rr;
        t += "src/.*"_r;
        t += "src/conversions/.*"_r;
        t += "src/iso19111/.*"_r;
        t += "src/projections/.*"_r;
        t += "src/transformations/.*"_r;

        t.Public += "include"_idir;
        t.Public += "src"_idir;

        t += "org.sw.demo.sqlite3"_dep;

        t.configureFile("cmake/proj_config.cmake.in", "proj_config.h", ConfigureFlags::EnableUndefReplacements);

        if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            t += "NOMINMAX"_def;
            t += "shell32.lib"_slib;
        }
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("proj");
    s.checkLibraryFunctionExists("exp", "exp.m");
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
    s.checkLibraryFunctionExists("dladdr", "dladdr.dl");
    s.checkLibraryFunctionExists("pthread_create", "pthread_create.pthread");
    s.checkLibraryFunctionExists("pthread_mutexattr_settype", "pthread_mutexattr_settype.pthread");
    s.checkDeclarationExists("PTHREAD_MUTEX_RECURSIVE");
    s.checkLibraryFunctionExists("curl_global_init", "curl_global_init.curl");
}
