void build(Solution &s)
{
    auto &p = s.addProject("pcre", "10.39.0");
    p += Git("https://github.com/PhilipHazel/pcre2", "pcre2-{M}.{m}");

    auto add_pcre = [&p](int bits) -> decltype(auto)
    {
        auto &pcre = p.addLibrary("pcre" + std::to_string(bits));
        pcre.setChecks("pcre");
        pcre +=
            "config-cmake.h.in",
            "src/.*"_rr;
        pcre -=
            "src/pcre2_jit_match.c",
            "src/pcre2_jit_misc.c",
            "src/pcre2_printint.c",
            "src/pcre2test.c";
        pcre -=
            "src/pcre2_fuzzsupport.c",
            "src/pcre2_jit_test.c",
            "src/pcre2demo.c",
            "src/pcre2grep.c",
            "src/pcre2posix.*"_rr;
        pcre -= "src/sljit/.*"_rr;
        pcre.Public += "src"_id;
        pcre.Public += "HAVE_CONFIG_H"_d;
        Definition dwidth{ "PCRE2_CODE_UNIT_WIDTH=" + std::to_string(bits) };
        pcre.Public += dwidth;
        pcre.Public += "SUPPORT_UCP"_d;
        pcre.Public += sw::Static, "PCRE2_STATIC"_d;
        if (!pcre.getBuildSettings().TargetOS.is(OSType::Windows))
            pcre.ExportAllSymbols = true;

        pcre.configureFile("src/pcre2_chartables.c.dist", "src/pcre2_chartables.c");
        pcre += pcre.BinaryDir / "src/pcre2_chartables.c";

        pcre.Variables["PCRE2_HEAP_LIMIT"] = 20000000;
        pcre.Variables["PCRE2_MATCH_LIMIT"] = 10000000;
        pcre.Variables["PCRE2_MATCH_LIMIT_DEPTH"] = pcre.Variables["PCRE2_MATCH_LIMIT"];
        pcre.Variables["PCRE2_LINK_SIZE"] = 2;
        pcre.Variables["PCRE2_PARENS_NEST_LIMIT"] = 250;
        pcre.Variables["PCRE2_MATCH_LIMIT_RECURSION"] = pcre.Variables["PCRE2_MATCH_LIMIT"];
        pcre.Variables["PCRE2GREP_BUFSIZE"] = 20480;
        pcre.Variables["PCRE2GREP_MAX_BUFSIZE"] = 1048576;
        pcre.Variables["PCRE2_NEWLINE"] = "LF";
        pcre.Variables["PCRE2_SUPPORT_PCRE2GREP_JIT"] = 1;
        pcre.Variables["PCRE2_SUPPORT_PCRE2GREP_CALLOUT"] = 1;
        pcre.Variables["PCRE2_SUPPORT_UNICODE"] = 1;

        pcre.Variables["SUPPORT_PCRE2_" + std::to_string(bits)] = 1;
        pcre.Variables["SUPPORT_UNICODE"] = 1;
        pcre.Variables["SUPPORT_PCRE2GREP_JIT"] = 1;
        pcre.Variables["SUPPORT_PCRE2GREP_CALLOUT"] = 1;
        pcre.Variables["NEWLINE_DEFAULT"] = 2;

        pcre.Variables["PCRE2_MAJOR"] = pcre.Variables["PACKAGE_VERSION_MAJOR"];
        pcre.Variables["PCRE2_MINOR"] = pcre.Variables["PACKAGE_VERSION_MINOR"];
        pcre.Variables["PCRE2_PRERELEASE"] = "\"\"";
        pcre.Variables["PCRE2_DATE"] = "\"\"";

        pcre.Variables["PCRE2_HAVE_STDINT_H"] = pcre.Variables["HAVE_STDINT_H"];
        pcre.Variables["PCRE2_HAVE_INTTYPES_H"] = pcre.Variables["HAVE_INTTYPES_H"];

        pcre.configureFile("config-cmake.h.in", pcre.BinaryPrivateDir / "config.h");
        pcre.configureFile("src/pcre2.h.in", "pcre2.h");
        return pcre;
    };

    auto &pcre8 = add_pcre(8);
    add_pcre(16);
    add_pcre(32);

    auto &pcreposix = p.addTarget<LibraryTarget>("pcreposix");
    pcreposix +=
        "src/pcre2posix.c",
        "src/pcre2posix.h";
    pcreposix.Public += sw::Static, "PCRE2_STATIC"_d;
    pcreposix.Public += pcre8;
//    pcreposix.Public += "PCRE2_LOCAL_WIDTH=8"_def;
    pcreposix.configureFile("config-cmake.h.in", pcreposix.BinaryPrivateDir / "config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("pcre");
    s.checkFunctionExists("bcopy");
    s.checkFunctionExists("memmove");
    s.checkFunctionExists("strerror");
    s.checkFunctionExists("strtoll");
    s.checkFunctionExists("strtoq");
    s.checkFunctionExists("_strtoi64");
    s.checkIncludeExists("bits/type_traits.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("type_traits.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("windows.h");
    s.checkTypeSize("long long");
    s.checkTypeSize("size_t");
    s.checkTypeSize("unsigned long long");
    s.checkTypeSize("void *");
}
