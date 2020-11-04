void build(Solution &s)
{
    auto &p = s.addProject("gnu.fribidi", "1.0.10");
    p += Git("https://github.com/fribidi/fribidi", "v{v}");

    auto &fribidi = p.addTarget<LibraryTarget>("fribidi");
    fribidi.setChecks("fribidi");
    //fribidi.PackageDefinitions = false;

    fribidi += sw::Shared, "FRIBIDI_BUILD"_def;
    fribidi.Public += sw::Static, "FRIBIDI_LIB_STATIC"_def;

    fribidi +=
        "gen.tab/.*\\.[hc]"_rr,
        "gen.tab/.*\\.txt"_rr,
        "lib/.*\\.[hci]"_rr,
        "lib/.*\\.in"_rr;

    fribidi -=
        "gen.tab/.*"_rr;

    fribidi.Public += sw::Static, "FRIBIDI_STATIC"_def;

    fribidi.Public +=
        "lib"_id;
    //fribidi.replaceInFileOnce("gen.tab/packtab.c", "pow", "powxxx");
    fribidi.replaceInFileOnce("lib/fribidi-bidi-types.h", "#if defined(__FRIBIDI_DOC)", "#if 1");

    fribidi.Variables["FRIBIDI_MAJOR_VERSION"] = fribidi.Variables["PACKAGE_VERSION_MAJOR"];
    fribidi.Variables["FRIBIDI_MINOR_VERSION"] = fribidi.Variables["PACKAGE_VERSION_MINOR"];
    fribidi.Variables["FRIBIDI_MICRO_VERSION"] = fribidi.Variables["PACKAGE_VERSION_PATCH"];
    fribidi.Variables["FRIBIDI_INTERFACE_VERSION"] = 0;
    fribidi.Variables["FRIBIDI_CHARSETS"] = 1;

    fribidi.Variables["PACKAGE"] = "";
    fribidi.Variables["PACKAGE_NAME"] = "";

    auto set_defs = [](auto &t)
    {
        t +=
            "HAVE_STRINGIZE"_d,
            "HAVE_STRING_H"_d,
            "HAVE_STDLIB_H"_d,
            "STDC_HEADERS"_d;
    };
    set_defs(fribidi);

    fribidi.configureFile("lib/fribidi-config.h.in", "fribidi-config.h");

    auto &gen = p.addDirectory("gen");
    //gen.Scope = TargetScope::Tool;

    auto &gen_unicode_version = gen.addTarget<ExecutableTarget>("unicode_version");
    gen_unicode_version += "gen.tab/gen-unicode-version.c";
    gen_unicode_version += "gen.tab/packtab.h";
    set_defs(gen_unicode_version);
    auto d = gen_unicode_version + fribidi;
    d->IncludeDirectoriesOnly = true;

    auto uv = fribidi.BinaryDir / "fribidi-unicode-version.h";
    auto c = fribidi.addCommand();
    {
        c << cmd::prog(gen_unicode_version)
            << cmd::in("gen.tab/unidata/ReadMe.txt")
            << cmd::in("gen.tab/unidata/BidiMirroring.txt")
            << cmd::std_out(uv);
    }

    auto add_tab = [&gen, &fribidi, &uv, &set_defs, &c](const String &n, const path &p1, const path &p2 = {})
    {
        auto COMPRESSION = 2;

        String name = "gen-" + n + "-tab";
        auto &gen_tab = gen.addTarget<ExecutableTarget>(boost::replace_all_copy(n, "-", "_") + "_tab");
        auto d = gen_tab + fribidi;
        d->IncludeDirectoriesOnly = true;
        gen_tab += path("gen.tab/" + name + ".c");
        gen_tab += "gen.tab/packtab.*"_rr;
        gen_tab += uv;
        set_defs(gen_tab);
#if SW_CPP_DRIVER_API_VERSION >= 2
        gen_tab.addGeneratedCommand(c.getCommand());
#endif

        auto c = fribidi.addCommand();
        c << cmd::prog(gen_tab) << COMPRESSION << cmd::in(p1);
        if (!p2.empty())
            c << cmd::in(p2);
        c << cmd::std_out(n + ".tab.i");
    };

    add_tab("bidi-type", "gen.tab/unidata/UnicodeData.txt");
    add_tab("joining-type", "gen.tab/unidata/UnicodeData.txt", "gen.tab/unidata/ArabicShaping.txt");
    add_tab("arabic-shaping", "gen.tab/unidata/UnicodeData.txt");
    add_tab("mirroring", "gen.tab/unidata/BidiMirroring.txt");
    add_tab("brackets", "gen.tab/unidata/BidiBrackets.txt", "gen.tab/unidata/UnicodeData.txt");
    add_tab("brackets-type", "gen.tab/unidata/BidiBrackets.txt");
}

void check(Checker &c)
{
    auto &s = c.addSet("fribidi");
    s.checkFunctionExists("memmove");
    s.checkFunctionExists("memset");
    s.checkFunctionExists("strdup");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
