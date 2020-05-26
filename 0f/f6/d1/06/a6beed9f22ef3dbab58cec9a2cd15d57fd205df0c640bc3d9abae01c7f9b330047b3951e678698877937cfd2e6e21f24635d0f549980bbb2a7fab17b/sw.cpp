// not working yet

/*static auto gen_flex(const DependencyPtr &flex, const DependencyPtr &m4, NativeExecutedTarget &t, FlexBisonData d)
{
...
}*/

void build(Solution &s)
{
    auto &t = s.addTarget<Executable>("westes.flex", "2.6.4");
    t += RemoteFile("https://github.com/westes/flex/releases/download/v{v}/flex-{v}.tar.gz");

    t.setChecks("flex");

    t += "src/.*"_rr;
    t -= "src/libmain.c";
    t -= "src/libyywrap.c";
    t -= "org.sw.demo.gnu.grep.gnulib"_dep;
    t -= "org.sw.demo.lexxmark.winflexbison.common"_dep;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        //t += "S_ISREG=0100000"_def;
        t += "S_IXUSR=0000100"_def;
        t += "strcasecmp=_stricmp"_def;
        t += "org.sw.demo.gnu.grep.gnulib"_dep;
        t += "org.sw.demo.lexxmark.winflexbison.common"_dep;
        t += "ws2_32.lib"_slib;

        t.writeFileOnce("unistd.h");
        t.writeFileOnce("sys/wait.h");
        t.replaceInFileOnce("src/flexdef.h", "#include <strings.h>", "//#include  <strings.h>");
    }

    t.replaceInFileOnce("src/flexdef.h", "#include <libgen.h>", "//#include  <libgen.h>");

    t.Variables["VERSION"] = "\"" + t.getPackage().getVersion().toString() + "\"";
    t.Variables["M4"] = "\"m4\"";

    t.configureFile("src/config.h.in", "config.h", ConfigureFlags::EnableUndefReplacements);
    t += "HAVE_CONFIG_H"_def;
}

void check(Checker &c)
{
    auto &s = c.addSet("flex");
    s.checkIncludeExists("assert.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("netinet/in.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/params.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/wait.h");
    s.checkIncludeExists("unistd.h");

    s.checkIncludeExists("regex.h");
    //s.checkIncludeExists("libintl.h");
    //s.checkLibraryFunctionExists("pthread_mutex_lock", "pthread_mutex_lock.pthread");
    //s.checkLibraryFunctionExists("log10", "log10.m");
    s.checkIncludeExists("stdbool.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("_Bool");
    s.checkFunctionExists("alloca");
    s.checkFunctionExists("fork");
    s.checkFunctionExists("malloc");
    s.checkFunctionExists("realloc");
    s.checkFunctionExists("dup2");
    s.checkFunctionExists("memset");
    s.checkFunctionExists("regcomp");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strchr");
    s.checkFunctionExists("strdup");
    s.checkFunctionExists("strtol");
    s.checkFunctionExists("pow");
    s.checkFunctionExists("setlocale");
    s.checkFunctionExists("reallocarray");
    s.checkFunctionExists("fork");
    s.checkFunctionExists("vfork");

    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
}
