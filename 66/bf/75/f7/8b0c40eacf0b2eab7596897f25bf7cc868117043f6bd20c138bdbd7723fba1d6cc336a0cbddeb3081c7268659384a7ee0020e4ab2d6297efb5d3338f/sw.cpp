void build(Solution &s)
{
    auto &json_c = s.addTarget<StaticLibraryTarget>("json_c", "master");
    json_c += Git("https://github.com/json-c/json-c", "", "{v}");

    json_c.setChecks("json_c");

    json_c +=
        ".*\\.c"_rr,
        ".*\\.h"_rr;

    json_c -=
        "test.*"_rr;

    json_c.writeFileOnce(json_c.BinaryPrivateDir / "config.h");
    json_c.writeFileOnce("json_config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("json_c");
    s.checkFunctionExists("memcmp");
    s.checkFunctionExists("open");
    s.checkFunctionExists("realloc");
    s.checkFunctionExists("setlocale");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strdup");
    s.checkFunctionExists("strerror");
    s.checkFunctionExists("strncasecmp");
    s.checkFunctionExists("uselocale");
    s.checkFunctionExists("vasprintf");
    s.checkFunctionExists("vprintf");
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("vsyslog");
    s.checkIncludeExists("endian.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("stdarg.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("syslog.h");
    s.checkIncludeExists("sys/cdefs.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("xlocale.h");
    s.checkIncludeExists("inttypes.h");
    s.checkTypeSize("int");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("long");
    s.checkTypeSize("long long");
    s.checkTypeSize("size_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("void *");
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
}
