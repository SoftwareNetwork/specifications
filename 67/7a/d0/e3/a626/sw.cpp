void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &p = s.addProject("pcre", "8.41.0");
    p.Source = RemoteFile("http://ftp.csx.cam.ac.uk/pub/software/programming/pcre/pcre-{M}.{m}.tar.gz");

    auto &pcre8 = p.addTarget<LibraryTarget>("pcre8");
    auto &pcre16 = p.addTarget<LibraryTarget>("pcre16");
    auto &pcre32 = p.addTarget<LibraryTarget>("pcre32");

    pcre8 +=
        "config-cmake.h.in",
        "pcre.h.in",
        "pcre_byte_order.c",
        "pcre_chartables.c.dist",
        "pcre_compile.c",
        "pcre_config.c",
        "pcre_dfa_exec.c",
        "pcre_exec.c",
        "pcre_fullinfo.c",
        "pcre_get.c",
        "pcre_globals.c",
        "pcre_internal.h",
        "pcre_jit_compile.c",
        "pcre_maketables.c",
        "pcre_newline.c",
        "pcre_ord2utf8.c",
        "pcre_refcount.c",
        "pcre_string_utils.c",
        "pcre_study.c",
        "pcre_tables.c",
        "pcre_ucd.c",
        "pcre_valid_utf8.c",
        "pcre_version.c",
        "pcre_xclass.c",
        "sljit/.*"_rr,
        "ucp.h";
    pcre8 -= "sljit/.*"_rr;

    pcre8.Public += "HAVE_CONFIG_H"_d;
    pcre8.Public += "SUPPORT_UCP"_d;

    if (fs::exists(pcre8.SourceDir / "pcre_chartables.c.dist") && !fs::exists(pcre8.BinaryDir / "pcre_chartables.c.dist"))
    {
        fs::copy_file(pcre8.SourceDir / "pcre_chartables.c.dist",
            pcre8.BinaryDir / "pcre_chartables.c", fs::copy_option::overwrite_if_exists);
        pcre8 += pcre8.BinaryDir / "pcre_chartables.c";
    }

    auto setup = [](auto &pcre, int bits)
    {
        const auto sbits = std::to_string(bits);

        if (bits > 8)
        {
            pcre +=
                path("config-cmake.h.in"),
                path("pcre.h.in"),
                path("pcre" + sbits + "_byte_order.c"),
                path("pcre" + sbits + "_chartables.c"),
                path("pcre" + sbits + "_compile.c"),
                path("pcre" + sbits + "_config.c"),
                path("pcre" + sbits + "_dfa_exec.c"),
                path("pcre" + sbits + "_exec.c"),
                path("pcre" + sbits + "_fullinfo.c"),
                path("pcre" + sbits + "_get.c"),
                path("pcre" + sbits + "_globals.c"),
                path("pcre" + sbits + "_jit_compile.c"),
                path("pcre" + sbits + "_maketables.c"),
                path("pcre" + sbits + "_newline.c"),
                path("pcre" + sbits + "_ord2utf" + sbits + ".c"),
                path("pcre" + sbits + "_refcount.c"),
                path("pcre" + sbits + "_string_utils.c"),
                path("pcre" + sbits + "_study.c"),
                path("pcre" + sbits + "_tables.c"),
                path("pcre" + sbits + "_ucd.c"),
                path("pcre" + sbits + "_utf" + sbits + "_utils.c"),
                path("pcre" + sbits + "_valid_utf" + sbits + ".c"),
                path("pcre" + sbits + "_version.c"),
                path("pcre" + sbits + "_xclass.c");
        }

        pcre.Public += sw::Static, "PCRE_STATIC"_d;

        pcre.setChecks("pcre");

        pcre.Variables["PCRE_LINK_SIZE"] = "2";
        pcre.Variables["PCRE_PARENS_NEST_LIMIT"] = "250";
        pcre.Variables["PCRE_MATCH_LIMIT"] = "10000000";
        pcre.Variables["PCRE_MATCH_LIMIT_RECURSION"] = pcre.Variables["PCRE_MATCH_LIMIT"];
        pcre.Variables["PCREGREP_BUFSIZE"] = "20480";
        pcre.Variables["PCRE_NEWLINE"] = "LF";
        pcre.Variables["PCRE_POSIX_MALLOC_THRESHOLD"] = "10";

        pcre.Variables["pcre_have_type_traits"] = "0";
        pcre.Variables["pcre_have_bits_type_traits"] = "0";

        if (pcre.Variables["HAVE_TYPE_TRAITS_H"] == "1")
            pcre.Variables["pcre_have_type_traits"] = "1";

        if (pcre.Variables["HAVE_BITS_TYPE_TRAITS_H"] == "1")
            pcre.Variables["pcre_have_bits_type_traits"] = "1";

        pcre.Variables["pcre_have_long_long"] = "0";
        pcre.Variables["pcre_have_ulong_long"] = "0";

        if (pcre.Variables["HAVE_LONG_LONG"] == "1")
            pcre.Variables["pcre_have_long_long"] = "1";

        if (pcre.Variables["HAVE_UNSIGNED_LONG_LONG"] == "1")
            pcre.Variables["pcre_have_ulong_long"] = "1";

        pcre.Variables["SUPPORT_PCREGREP_JIT"] = "1";

        pcre.Variables["NEWLINE"] = "10";

        pcre.Variables["PCRE_MAJOR"] = pcre.Variables["PACKAGE_VERSION_MAJOR"];
        pcre.Variables["PCRE_MINOR"] = pcre.Variables["PACKAGE_VERSION_MINOR"];
        pcre.Variables["PCRE_PRERELEASE"] = "\"\"";
        pcre.Variables["PCRE_DATE"] = "\"\"";

        pcre.Variables["SUPPORT_PCRE" + sbits] = "1";

        pcre.configureFile("config-cmake.h.in", "config.h");
        pcre.configureFile("pcre.h.in", "pcre.h");
    };

    setup(pcre8, 8);
    setup(pcre16, 16);
    setup(pcre32, 32);

    pcre8.replaceInFileOnce("pcre_internal.h",
        "extern const ucd_record  PRIV(ucd_records)[];",
        "PCRE_EXP_DECL const ucd_record  PRIV(ucd_records)[];"
    );

    pcre8.replaceInFileOnce("pcre_internal.h",
        "extern const pcre_uint8  PRIV(ucd_stage1)[];",
        "PCRE_EXP_DECL const pcre_uint8  PRIV(ucd_stage1)[];"
    );

    pcre8.replaceInFileOnce("pcre_internal.h",
        "extern const pcre_uint16 PRIV(ucd_stage2)[];",
        "PCRE_EXP_DECL const pcre_uint16 PRIV(ucd_stage2)[];"
    );

    pcre8.replaceInFileOnce("pcre_internal.h",
        "extern const pcre_uint32 PRIV(ucp_gentype)[];",
        "PCRE_EXP_DECL const pcre_uint32 PRIV(ucp_gentype)[];"
    );

    auto &d16 = pcre16 + pcre8;
    d16.IncludeDirectoriesOnly = true;

    auto &d32 = pcre32 + pcre8;
    d32.IncludeDirectoriesOnly = true;

    auto &pcreposix = p.addTarget<LibraryTarget>("pcreposix");
    pcreposix +=
        "pcreposix.c",
        "pcreposix.h";
    pcreposix.Public += sw::Shared, "PCREPOSIX_EXP_DECL"_d;
    pcreposix.Public += sw::Static, "PCRE_STATIC"_d;
    pcreposix.Public += "pub.cppan2.demo.pcre.pcre"_dep;
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
