void build(Solution &s)
{
    auto &t = s.addLibrary("json_c", "0.16.2022.4.14");
    t += Git("https://github.com/json-c/json-c", "json-c-{M}.{m}-{p}{t:02}{v5:02}");

    t.setChecks("json_c", true);

    t.ApiName = "JSON_EXPORT";
    t += ".*\\.[hc]"_r;

    t.Variables["SIZEOF_SSIZE_T"] = t.Variables["SIZEOF_SIZE_T"];
    t.configureFile("json.h.cmakein", t.BinaryPrivateDir / "json.h");
    t.configureFile("cmake/config.h.in", t.BinaryPrivateDir / "config.h");
    t.configureFile("cmake/json_config.h.in", "json_config.h", ConfigureFlags::EnableUndefReplacements);
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
    s.checkFunctionExists("strtoll");
    s.checkFunctionExists("strtoull");
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
    s.checkDeclarationExists("isnan").Parameters.Includes.push_back("math.h");
    s.checkDeclarationExists("isinf").Parameters.Includes.push_back("math.h");
    s.checkDeclarationExists("infinity").Parameters.Includes.push_back("math.h");
    s.checkDeclarationExists("nan").Parameters.Includes.push_back("math.h");
    s.checkTypeSize("int");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("long");
    s.checkTypeSize("long long");
    s.checkTypeSize("size_t");
    //s.checkTypeSize("ssize_t");
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

    for (auto &check : s.all)
        check->Prefixes.insert("JSON_C_");
}
