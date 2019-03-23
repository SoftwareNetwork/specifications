void build(Solution &s)
{
    auto &glog = s.addTarget<LibraryTarget>("google.glog", "0.4.0");
    glog += Git("https://github.com/google/glog", "v{v}");

    glog.setChecks("glog", true);

    glog.ApiNames.insert("GOOGLE_GLOG_DLL_DECL");
    glog +=
        "src/.*\\.cc"_rr,
        "src/.*\\.h"_rr,
        "src/.*\\.in"_rr;
    glog -= ".*test.*"_rr;
    glog.Public += "src"_id;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        glog += "src/windows.*"_rr;
        glog.Public += "src/windows"_idir;
        //glog.Public -= IncludeDirectory(glog.BinaryDir);
    }

    glog.Public += "HAVE_LIB_GFLAGS"_d;
    glog.Public += "org.sw.demo.google.gflags"_dep;
    glog.Variables["ac_cv_have_libgflags"] = 1;

    glog.Variables["ac_google_start_namespace"] = "namespace google {";
    glog.Variables["ac_google_end_namespace"] = "} // namespace google";

    glog.writeFileOnce(glog.BinaryPrivateDir / "defines.h");

    if (s.Settings.TargetOS.Type != OSType::Windows)
    {
        glog.configureFile("src/config.h.cmake.in", glog.BinaryPrivateDir / "config.h");

        glog.configureFile("src/glog/logging.h.in", "glog/logging.h");
        glog.configureFile("src/glog/raw_logging.h.in", "glog/raw_logging.h");
        glog.configureFile("src/glog/stl_logging.h.in", "glog/stl_logging.h");
        glog.configureFile("src/glog/vlog_is_on.h.in", "glog/vlog_is_on.h");
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("glog");
    s.checkFunctionExists("__builtin_expect");
    s.checkFunctionExists("dladdr");
    s.checkFunctionExists("fcntl");
    s.checkFunctionExists("pread");
    s.checkFunctionExists("pwrite");
    s.checkFunctionExists("sigaction");
    s.checkFunctionExists("sigaltstack");
    s.checkIncludeExists("execinfo.h");
    s.checkIncludeExists("glob.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("libunwind.h");
    s.checkIncludeExists("pwd.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("syscall.h");
    s.checkIncludeExists("syslog.h");
    s.checkIncludeExists("sys/syscall.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/utsname.h");
    s.checkIncludeExists("ucontext.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("unwind.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("u_int16_t");
    s.checkTypeSize("void *");
    s.checkTypeSize("__uint16");
    s.checkLibraryFunctionExists("unwind", "backtrace");
    s.checkLibraryFunctionExists("pthread", "pthread_self");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");

    for (auto &c : s)
    {
        auto defs = c->Definitions;
        for (auto &d : defs)
            c->Definitions.insert("ac_cv_" + boost::to_lower_copy(d));
    }
}
