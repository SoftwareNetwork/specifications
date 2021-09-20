void build(Solution &s)
{
    auto &nghttp2 = s.addTarget<LibraryTarget>("nghttp2", "1.45.0");
    nghttp2 += Git("https://github.com/nghttp2/nghttp2", "v{v}");

    nghttp2.setChecks("nghttp2");

    nghttp2 +=
        "cmakeconfig.h.in",
        "lib/.*\\.c"_rr,
        "lib/.*\\.h"_rr,
        "lib/includes/.*\\.h"_rr,
        "lib/includes/.*\\.h.in"_rr;

    nghttp2.Public +=
        "lib/includes"_id;

    nghttp2.Private += "BUILDING_NGHTTP2"_d;
    nghttp2.Public += sw::Static, "NGHTTP2_STATICLIB"_d;

    nghttp2 += "HAVE_CONFIG_H"_d;

    nghttp2.Definitions["PACKAGE"] = "\"" + nghttp2.getPackage().getPath().toString() + "\"";
    nghttp2.Variables["PACKAGE_VERSION"] = nghttp2.getPackage().getVersion().toString();
    nghttp2.Variables["PACKAGE_VERSION_NUM"] = "0x0" +
        std::to_string(nghttp2.getPackage().getVersion().getMajor()) +
        std::to_string(nghttp2.getPackage().getVersion().getMinor()) +
        + "0" +
        std::to_string(nghttp2.getPackage().getVersion().getPatch()) +
        "LL";

    if (nghttp2.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        if (nghttp2.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
            nghttp2.Variables["ssize_t"] = "int64_t";
        else
            nghttp2.Variables["ssize_t"] = "int";
    }

    nghttp2.configureFile("lib/includes/nghttp2/nghttp2ver.h.in", "nghttp2/nghttp2ver.h");
    nghttp2.configureFile("cmakeconfig.h.in", nghttp2.BinaryPrivateDir / "config.h");
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
