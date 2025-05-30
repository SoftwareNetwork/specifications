void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("nghttp3", "1.6.0");
    t += Git("https://github.com/ngtcp2/nghttp3", "v{v}");

    t.setChecks("nghttp2");

    t +=
        "cmakeconfig.h.in",
        "lib/.*\\.c"_rr,
        "lib/.*\\.h"_rr,
        "lib/includes/.*\\.h"_rr,
        "lib/includes/.*\\.h.in"_rr;

    t.Public +=
        "lib/includes"_id;

    t.Private += "BUILDING_NGHTTP3"_d;
    t.Public += sw::Static, "NGHTTP3_STATICLIB"_d;

    t += "HAVE_CONFIG_H"_d;

    t.Definitions["PACKAGE"] = "\"" + t.getPackage().getPath().toString() + "\"";
    t.Variables["PACKAGE_VERSION"] = t.getPackage().getVersion().toString();
    t.Variables["PACKAGE_VERSION_NUM"] = "0x0" +
        std::to_string(t.getPackage().getVersion().getMajor()) +
        std::to_string(t.getPackage().getVersion().getMinor()) +
        + "0" +
        std::to_string(t.getPackage().getVersion().getPatch()) +
        "LL";

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        if (t.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
            t.Variables["ssize_t"] = "int64_t";
        else
            t.Variables["ssize_t"] = "int";
    }

    t.configureFile("lib/includes/nghttp3/version.h.in", "nghttp3/version.h");
    t.configureFile("cmakeconfig.h.in", t.BinaryPrivateDir / "config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("nghttp2");
    s.checkFunctionExists("accept4");
    s.checkFunctionExists("chown");
    s.checkFunctionExists("dup2");
    s.checkFunctionExists("error_at_line");
    s.checkFunctionExists("fork");
    s.checkFunctionExists("getcwd");
    s.checkFunctionExists("getpwnam");
    s.checkFunctionExists("localtime_r");
    s.checkFunctionExists("malloc");
    s.checkFunctionExists("memchr");
    s.checkFunctionExists("memmove");
    s.checkFunctionExists("memset");
    s.checkFunctionExists("realloc");
    s.checkFunctionExists("socket");
    s.checkFunctionExists("sqrt");
    s.checkFunctionExists("strchr");
    s.checkFunctionExists("strdup");
    s.checkFunctionExists("strerror");
    s.checkFunctionExists("strerror_r");
    s.checkFunctionExists("strndup");
    s.checkFunctionExists("strnlen");
    s.checkFunctionExists("strstr");
    s.checkFunctionExists("strtol");
    s.checkFunctionExists("strtoul");
    s.checkFunctionExists("timegm");
    s.checkFunctionExists("timerfd_create");
    s.checkFunctionExists("_Exit");
    s.checkIncludeExists("arpa/inet.h");
    s.checkIncludeExists("assert.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("netdb.h");
    s.checkIncludeExists("netinet/in.h");
    s.checkIncludeExists("pwd.h");
    s.checkIncludeExists("syslog.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("time.h");
    s.checkTypeSize("int");
    s.checkTypeSize("int16_t");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("int8_t");
    s.checkTypeSize("off_t");
    s.checkTypeSize("pid_t");
    s.checkTypeSize("ptrdiff_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("time_t");
    s.checkTypeSize("uid_t");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("uint8_t");
    s.checkTypeSize("void *");
    s.checkLibraryFunctionExists("cunit", "CU_initialize_registry");
    s.checkLibraryFunctionExists("ev", "ev_time");
}
