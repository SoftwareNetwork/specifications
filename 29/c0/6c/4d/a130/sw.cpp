void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &p = s.addProject("pcre", "10.30.0");
    p.Source = RemoteFile("https://ftp.pcre.org/pub/pcre/pcre2-{M}.{m}.tar.gz");

    auto &pcre8 = p.addTarget<LibraryTarget>("pcre8");
    auto &pcre16 = p.addTarget<LibraryTarget>("pcre16");
    auto &pcre32 = p.addTarget<LibraryTarget>("pcre32");

    auto setup = [](auto &pcre, int bits)
    {
        const auto sbits = std::to_string(bits);

        pcre +=
            "config-cmake.h.in",
            "src/.*"_rr;
        pcre -=
            "src/dftables.c",
            "src/pcre2_fuzzsupport.c",
            "src/pcre2_jit_test.c",
            "src/pcre2demo.c",
            "src/pcre2grep.c",
            "src/pcre2posix.*"_rr,
            "src/pcre2_jit_match.c",
            "src/pcre2_jit_misc.c",
            "src/pcre2_printint.c",
            "src/pcre2test.c",
            "src/sljit/.*"_rr
            ;

        pcre.Public += "HAVE_CONFIG_H"_d;
        pcre.Public += "SUPPORT_UCP"_d;
        pcre.Public += sw::Static, "PCRE2_STATIC"_d;
        pcre.Public.Definitions["PCRE2_CODE_UNIT_WIDTH"] = sbits;

        const auto in = pcre.SourceDir / "src" / "pcre2_chartables.c.dist";
        const auto out = pcre.BinaryDir / "pcre2_chartables.c";
        if (fs::exists(in) && !fs::exists(out))
        {
            fs::copy_file(in, out, fs::copy_option::overwrite_if_exists);
            pcre += out;
        }

        pcre.setChecks("pcre");

        pcre.Variables["PCRE2_LINK_SIZE"] = "2";
        pcre.Variables["PCRE2_PARENS_NEST_LIMIT"] = "250";
        pcre.Variables["PCRE2_HEAP_LIMIT"] = "20000000";
        pcre.Variables["PCRE2_MATCH_LIMIT"] = "10000000";
        pcre.Variables["PCRE2_MATCH_LIMIT_DEPTH"] = pcre.Variables["PCRE2_MATCH_LIMIT"];
        pcre.Variables["PCRE2GREP_BUFSIZE"] = "20480";
        pcre.Variables["PCRE2GREP_MAX_BUFSIZE"] = "1048576";
        pcre.Variables["PCRE2_NEWLINE"] = "LF";
        pcre.Variables["PCRE2_POSIX_MALLOC_THRESHOLD"] = "10";

        pcre.Variables["SUPPORT_PCRE2GREP_JIT"] = "1";
        pcre.Variables["SUPPORT_PCRE2GREP_CALLOUT"] = "1";

        pcre.Variables["SUPPORT_UNICODE"] = "1";

        pcre.Variables["NEWLINE_DEFAULT"] = "2";

        pcre.Variables["PCRE2_MAJOR"] = pcre.Variables["PACKAGE_VERSION_MAJOR"];
        pcre.Variables["PCRE2_MINOR"] = pcre.Variables["PACKAGE_VERSION_MINOR"];
        pcre.Variables["PCRE2_PRERELEASE"] = "\"\"";
        pcre.Variables["PCRE2_DATE"] = "\"\"";

        pcre.Variables["SUPPORT_PCRE2_" + sbits] = "1";

        pcre.configureFile("config-cmake.h.in", "config.h");
        pcre.configureFile("src/pcre2.h.in", "pcre2.h");
    };

    setup(pcre8, 8);
    setup(pcre16, 16);
    setup(pcre32, 32);

    auto &pcreposix = p.addTarget<LibraryTarget>("pcreposix");
    pcreposix +=
        "src/pcre2posix.c",
        "src/pcre2posix.h";
    pcreposix.Public += sw::Shared, "PCRE2POSIX_EXP_DECL"_d;
    pcreposix.Public += sw::Static, "PCRE2_STATIC"_d;
    pcreposix.Public += pcre8;
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
