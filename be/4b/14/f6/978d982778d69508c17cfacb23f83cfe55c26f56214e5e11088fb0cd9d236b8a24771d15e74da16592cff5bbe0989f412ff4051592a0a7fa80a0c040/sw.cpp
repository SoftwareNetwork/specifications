void build(Solution &s)
{
    auto &gmp = s.addProject("gnu.gmp", "6.2.1");
    gmp += RemoteFile("https://gmplib.org/download/gmp/gmp-{v}.tar.xz");

    auto &mini = gmp.addTarget<StaticLibraryTarget>("mini");
    mini += "mini-gmp/mini-gmp.*"_rr;

    auto &lib = gmp.addTarget<LibraryTarget>("lib");
    {
        lib.setChecks("gmp", true);

        lib +=
            ".*\\.h"_rr,
            "assert.c",
            "bootstrap.c",
            "errno.c",
            "extract-dbl.c",
            "gen-.*.c"_rr,
            "gmp-h.in",
            "invalid.c",
            "memory.c",
            "mini-gmp/mini-gmp.*"_rr,
            "mp_.*"_rr,
            "mpf/.*"_rr,
            "mpn/.*"_rr,
            "mpq/.*"_rr,
            "mpz/.*"_rr,
            "nextprime.c",
            "primesieve.c",
            "printf/.*"_rr,
            "rand/.*"_rr,
            "scanf/.*"_rr,
            "tal-reent.c",
            "version.c";

        lib -=
            "bootstrap.c",
            "gen-.*.c"_rr,
            "mini-gmp/mini-gmp.*"_rr,
            "mpn/.*"_rr;

        lib.Public += "."_idir;

        lib.Private += "OPERATION_and_n"_d;
        lib.Private += "OPERATION_sec_add_1"_d;
        lib.Private += "OPERATION_sec_div_r"_d;
        lib.Private += "OPERATION_sec_pi1_div_r"_d;
        lib.Private += Definition{ "VERSION=\"" + lib.Variables["PACKAGE_VERSION"].toString() + "\"" };
        lib.Private += "__GMP_WITHIN_GMP"_d;
        lib.Public += "WANT_FAKE_CPUID"_d;
        lib.Public += "WANT_FAT_BINARY"_d;
        lib.Public += "WANT_TMP_ALLOCA"_d;
        lib.Public += "WANT_TMP_REENTRANT"_d;

        lib.Variables["LIBGMP_DLL"] = int(lib.getBuildSettings().Native.LibrariesType == LibraryType::Shared);

        lib.Variables["HAVE_HOST_CPU_FAMILY_power"] = 0;
        lib.Variables["HAVE_HOST_CPU_FAMILY_powerpc"] = 0;

        if (lib.Variables["SIZEOF_VOID_P"])
            lib.Variables["GMP_LIMB_BITS"] = lib.Variables["SIZEOF_VOID_P"].get<int>() * 8;
        lib.Variables["GMP_NAIL_BITS"] = 0;

        lib.writeFileOnce(lib.BinaryPrivateDir / "config.h");
        lib.writeFileOnce(lib.BinaryDir / "fat.h");
        lib.configureFile("gmp-h.in", "gmp.h");

        lib.replaceInFileOnce("gmp-impl.h", "struct cpuvec_t {", R"(
            #pragma push_macro("__GMP_DECLSPEC")
            #undef __GMP_DECLSPEC
            #define __GMP_DECLSPEC
            struct cpuvec_t {
        )");

        lib.replaceInFileOnce("gmp-impl.h", "__GMP_DECLSPEC extern struct cpuvec_t __gmpn_cpuvec;", R"(
            #pragma pop_macro("__GMP_DECLSPEC")
            __GMP_DECLSPEC extern struct cpuvec_t __gmpn_cpuvec;
        )");

        lib.replaceInFileOnce("nextprime.c", "#include <string.h>", "//#include <string.h>");

        auto &tools = gmp.addDirectory("tools");

#define ADD_EXE(n)                               \
    auto &g##n = tools.addExecutable("gen_" #n); \
    g##n += "gen-" #n ".c";                      \
    (g##n + lib)->IncludeDirectoriesOnly = true; \
    (g##n + mini)->IncludeDirectoriesOnly = true

        ADD_EXE(fib);
        ADD_EXE(fac);
        ADD_EXE(bases);
        ADD_EXE(trialdivtab);
        ADD_EXE(jacobitab);
        ADD_EXE(psqr);

        // gfib
        {
            auto c = lib.addCommand();
            c << cmd::prog(gfib)
                << "header"
                << lib.Variables["GMP_LIMB_BITS"].toString()
                << lib.Variables["GMP_NAIL_BITS"].toString()
                << cmd::std_out("fib_table.h");
        }
        {
            auto c = lib.addCommand();
            c << cmd::prog(gfib)
                << "table"
                << lib.Variables["GMP_LIMB_BITS"].toString()
                << lib.Variables["GMP_NAIL_BITS"].toString()
                << cmd::std_out("fib_table.c");
        }

        // gfac
        {
            auto c = lib.addCommand();
            c << cmd::prog(gfac)
                << lib.Variables["GMP_LIMB_BITS"].toString()
                << lib.Variables["GMP_NAIL_BITS"].toString()
                << cmd::std_out("fac_table.h");
        }

        // gbases
        {
            auto c = lib.addCommand();
            c << cmd::prog(gbases)
                << "header"
                << lib.Variables["GMP_LIMB_BITS"].toString()
                << lib.Variables["GMP_NAIL_BITS"].toString()
                << cmd::std_out("mp_bases.h");
        }
        {
            auto c = lib.addCommand();
            c << cmd::prog(gbases)
                << "table"
                << lib.Variables["GMP_LIMB_BITS"].toString()
                << lib.Variables["GMP_NAIL_BITS"].toString()
                << cmd::std_out("mp_bases.c");
        }

        // gtrialdivtab
        {
            auto c = lib.addCommand();
            c << cmd::prog(gtrialdivtab)
                << lib.Variables["GMP_LIMB_BITS"].toString()
                << "8000"
                << cmd::std_out("trialdivtab.h");
        }

        // gjacobitab
        {
            auto c = lib.addCommand();
            c << cmd::prog(gjacobitab)
                << cmd::std_out("jacobitab.h");
        }

        // gpsqr
        {
            auto c = lib.addCommand();
            c << cmd::prog(gpsqr)
                << lib.Variables["GMP_LIMB_BITS"].toString()
                << lib.Variables["GMP_NAIL_BITS"].toString()
                << cmd::std_out("perfsqr.h");
        }

        lib += "mpn/generic/.*"_rr;
        lib -= ".*udiv_w_sdiv.c"_rr;

        lib -= "mpn/generic/add_1.c";
        lib -= "mpn/generic/sub_1.c";
        lib -= "mpn/generic/zero_p.c";

        lib -= "mpn/generic/add.c";
        lib -= "mpn/generic/sub.c";
        lib -= "mpn/generic/neg.c";
        lib -= "mpn/generic/cmp.c";

        lib -= "mpz/perfsqr.c";
        lib -= "mpz/get_ui.c";
        lib -= "mpz/neg.c";

        lib.configureFile("mpn/generic/popham.c", "popham.c", ConfigureFlags::CopyOnly);
        lib += "popham.c";
        lib["mpn/generic/popham.c"].args["c"].push_back("-DOPERATION_popcount");
        lib["popham.c"].args["c"].push_back("-DOPERATION_hamdist");

        if (lib.getBuildSettings().TargetOS.Arch == ArchType::x86)
        {
            lib.Public += "mpn/x86"_idir;
            lib += "HAVE_HOST_CPU_FAMILY_x86"_d;
            lib += "SIZEOF_MP_LIMB_T=4"_d;
        }
        else
        {
            lib.Public += "mpn/x86_64"_idir;
            lib += "HAVE_HOST_CPU_FAMILY_x86_64"_d;
            lib += "SIZEOF_MP_LIMB_T=8"_d;
            lib += "_LONG_LONG_LIMB"_d;
        }
    }

    auto &cxx = gmp.addTarget<StaticLibraryTarget>("cxx");
    cxx += "cxx/.*\\.cc"_rr;
    cxx.writeFileOnce(cxx.BinaryPrivateDir / "config.h");
    cxx.Public += lib;
}

void check(Checker &c)
{
    auto &s = c.addSet("gmp");
    s.checkFunctionExists("alarm");
    s.checkFunctionExists("attr_get");
    s.checkFunctionExists("clock");
    s.checkFunctionExists("cputime");
    s.checkFunctionExists("getpagesize");
    s.checkFunctionExists("getrusage");
    s.checkFunctionExists("getsysinfo");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("localeconv");
    s.checkFunctionExists("memset");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("mprotect");
    s.checkFunctionExists("nl_langinfo");
    s.checkFunctionExists("obstack_vprintf");
    s.checkFunctionExists("popen");
    s.checkFunctionExists("processor_info");
    s.checkFunctionExists("pstat_getprocessor");
    s.checkFunctionExists("raise");
    s.checkFunctionExists("read_real_time");
    s.checkFunctionExists("sigaction");
    s.checkFunctionExists("sigaltstack");
    s.checkFunctionExists("sigstack");
    s.checkFunctionExists("strchr");
    s.checkFunctionExists("strerror");
    s.checkFunctionExists("strnlen");
    s.checkFunctionExists("strtol");
    s.checkFunctionExists("strtoul");
    s.checkFunctionExists("sysconf");
    s.checkFunctionExists("sysctl");
    s.checkFunctionExists("sysctlbyname");
    s.checkFunctionExists("syssgi");
    s.checkFunctionExists("times");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("float.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("invent.h");
    s.checkIncludeExists("langinfo.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("machine/hal_sysinfo.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("nl_types.h");
    s.checkIncludeExists("readline/history.h");
    s.checkIncludeExists("readline/readline.h");
    s.checkIncludeExists("sstream");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/attributes.h");
    s.checkIncludeExists("sys/iograph.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/processor.h");
    s.checkIncludeExists("sys/pstat.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/sysctl.h");
    s.checkIncludeExists("sys/sysinfo.h");
    s.checkIncludeExists("sys/syssgi.h");
    s.checkIncludeExists("sys/systemcfg.h");
    s.checkIncludeExists("sys/times.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("double");
    s.checkTypeSize("intmax_t");
    s.checkTypeSize("intptr_t");
    s.checkTypeSize("long");
    s.checkTypeSize("mp_limb_t");
    s.checkTypeSize("ptrdiff_t");
    s.checkTypeSize("quad_t");
    s.checkTypeSize("short");
    s.checkTypeSize("signal");
    s.checkTypeSize("size_t");
    s.checkTypeSize("stack_t");
    s.checkTypeSize("std::locale");
    s.checkTypeSize("uint_least32_t");
    s.checkTypeSize("unsigned");
    s.checkTypeSize("unsigned long");
    s.checkTypeSize("void *");
    s.checkLibraryFunctionExists("readline", "readline");
    s.checkLibraryFunctionExists("ncurses", "tputs");
    {
        auto &c = s.checkStructMemberExists("struct pst_processor", "psp_iticksperclktick");
    }
    s.checkSourceCompiles("HAVE_TIME_WITH_SYS_TIME", R"sw_xxx(
    #include <time.h>
    #include <sys/time.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
}
