#pragma sw header on

/*namespace flex_bison
{

static bool need_build(const Solution &s)
{
return
// s.Settings.Native.CompilerType != CompilerType::GNU
s.getHostOs().Type == OSType::Windows // && !::sw::detail::isHostCygwin()
;
}

}*/ // namespace flex_bison

struct FlexBisonData
{
    path out;
    path outh;
    //bool cpp = true;

    path in;
    path wdir;

    Strings args; // additional

    void setupFiles(NativeExecutedTarget &t, const String &cext)
    {
        if (out.empty())
        {
            wdir = t.BinaryPrivateDir / "fb" / in.filename();
            t += IncludeDirectory(wdir);
        }
        else
        {
            // user will handle includes himself
            wdir = out.parent_path();
        }

        if (out.empty())
        {
            if (in.extension() == cext)
            {
                out = wdir / (in.filename() += ".c");
                if (outh.empty())
                    outh = wdir / (in.filename() += ".h");
            }
            else
            {
                out = wdir / (in.filename() += ".cpp");
                if (outh.empty())
                    outh = wdir / (in.filename() += ".hpp");
            }
        }

        if (out.is_relative())
            out = t.BinaryPrivateDir / out;

        if (outh.empty())
        {
            String ext = out.extension().string();
            if (ext.size() > 1)
            {
                ext[1] = 'h';
                boost::replace_all(ext, "c", "h"); // for .cc
            }
            else
                ext = ".hpp";
            outh = out.parent_path() / (out.stem() += ext);
        }

        wdir = out.parent_path();
    }
};

static auto gen_bison(const DependencyPtr &base, NativeExecutedTarget &t, FlexBisonData d)
{
#if SW_CPP_DRIVER_API_VERSION > 1
    auto bison = std::make_shared<Dependency>(sw::UnresolvedPackageName{
        base->getUnresolvedPackageId().getName().getPath() / "bison",
        base->getUnresolvedPackageId().getName().getRange()});
#else
    auto bison = std::make_shared<Dependency>(sw::UnresolvedPackage{base->package.getPath() / "bison", base->package.getRange()});
#endif

    d.setupFiles(t, ".y");

    auto c = t.addCommand();
    c << cmd::wdir(d.wdir);
    //if (flex_bison::need_build(t.getSolution()))
    c << cmd::prog(bison);
    c << cmd::out(d.out, cmd::Prefix{ "--output=" });
    c << cmd::out(d.outh, cmd::Prefix{ "--defines=" });
    for (auto &a : d.args)
        c << a;
    c << cmd::in(d.in);

    return c.getCommand();
}

static auto gen_bison(const DependencyPtr &base, NativeExecutedTarget &t, const path &in, FlexBisonData d)
{
    d.in = in;
    return gen_bison(base, t, d);
}

static auto gen_bison(const DependencyPtr &base, NativeExecutedTarget &t, const path &in, const path &out = {}, const path &outh = {})
{
    FlexBisonData d;
    d.out = out;
    d.outh = outh;
    return gen_bison(base, t, in, d);
}

static auto gen_flex(const DependencyPtr &base, NativeExecutedTarget &t, FlexBisonData d)
{
#if SW_CPP_DRIVER_API_VERSION > 1
    auto flex = std::make_shared<Dependency>(sw::UnresolvedPackageName{
        base->getUnresolvedPackageId().getName().getPath() / "flex",
        base->getUnresolvedPackageId().getName().getRange()});
#else
    auto flex = std::make_shared<Dependency>(sw::UnresolvedPackage{base->package.getPath() / "flex", base->package.getRange()});
#endif

    d.setupFiles(t, ".l");

    // any case
    //t -= flex;
    //t -= "org.sw.demo.westes.flex"_dep;

    //
    auto c = t.addCommand();
    c << cmd::wdir(d.wdir);
    //if (flex_bison::need_build(t.getSolution()))
    //if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
    //c << cmd::prog("org.sw.demo.westes.flex"_dep);
    //else
    c << cmd::prog(flex);
    c << "-o" << cmd::out(d.out);
    for (auto &a : d.args)
        c << a;
    c << cmd::in(d.in);

    return c.getCommand();
}

static auto gen_flex(const DependencyPtr &base, NativeExecutedTarget &t, const path &in, FlexBisonData d)
{
    d.in = in;
    return gen_flex(base, t, d);
}

static auto gen_flex(const DependencyPtr &base, NativeExecutedTarget &t, const path &in, const path &out = {}, const path &outh = {})
{
    FlexBisonData d;
    d.out = out;
    d.outh = outh;
    return gen_flex(base, t, in, d);
}

static auto gen_flex_bison(const DependencyPtr &base, NativeExecutedTarget &t,
    const path &f, const path &b,
    const Strings &flex_args = {}, const Strings &bison_args = {})
{
    FlexBisonData bd;
    bd.args = bison_args;
    auto bc = gen_bison(base, t, b, bd);

    FlexBisonData fd;
    fd.args = flex_args;
    auto fc = gen_flex(base, t, f, fd);

    return std::tuple{fc, bc};
}

static auto gen_flex_bison_pair(const DependencyPtr &base, NativeExecutedTarget &t, const String &type, const path &p)
{
    auto name = p.filename().string();
    auto name_upper = boost::to_upper_copy(name);
    auto my_parser = name + "Parser";
    my_parser[0] = toupper(my_parser[0]);

    t.Definitions["HAVE_BISON_" + name_upper + "_PARSER"];

    String s;
    auto add_line = [&s](const String &l)
    {
        s += l + "\n";
    };
    add_line("#pragma once");
    add_line("");
    add_line("#undef  THIS_PARSER_NAME");
    add_line("#undef  THIS_PARSER_NAME_UP");
    add_line("#undef  THIS_LEXER_NAME");
    add_line("#undef  THIS_LEXER_NAME_UP");
    add_line("");
    add_line("#define THIS_PARSER_NAME       " + name);
    add_line("#define THIS_PARSER_NAME_UP    " + name_upper);
    add_line("#define THIS_LEXER_NAME        THIS_PARSER_NAME");
    add_line("#define THIS_LEXER_NAME_UP     THIS_PARSER_NAME_UP");
    add_line("");
    add_line("#undef  MY_PARSER");
    add_line("#define MY_PARSER              " + my_parser);
    add_line("");
    add_line("#define " + type);
    add_line("#include <primitives/helper/bison.h>");
    add_line("#undef  " + type);
    add_line("");
    add_line("#include <" + name + ".yy.hpp>");

    t.writeFileOnce(t.BinaryPrivateDir / (name + "_parser.h"), s);
    t.Definitions["HAVE_BISON_" + name_upper + "_PARSER"] = 1;

    auto f = p;
    auto b = p;
    return gen_flex_bison(base, t, f += ".ll", b += ".yy", {"--prefix=ll_" + name}, {"-Dapi.prefix={yy_" + name + "}"});
};

#pragma sw header off

struct BisonExecutable : ExecutableTarget
{
    using ExecutableTarget::ExecutableTarget;

    void setupCommand(builder::Command &c) const override
    {
        if (getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            c.setProgram("bison");
            return;
        }
        ExecutableTarget::setupCommand(c);
        c.environment["BISON_PKGDATADIR"] = to_string(normalize_path(SourceDir / "bison" / "data"));
    }
};

struct FlexExecutable : ExecutableTarget
{
    using ExecutableTarget::ExecutableTarget;

    void setupCommand(builder::Command &c) const override
    {
        if (getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            c.setProgram("flex");
            return;
        }
        ExecutableTarget::setupCommand(c);
    }
};

void build(Solution &s)
{
    /*bool win_flex_bison = flex_bison::need_build(s);
    if (!win_flex_bison && !s.DryRun)
    return;*/

    auto &winflexbison = s.addProject("lexxmark.winflexbison", "2.5.24");
    Git src("https://github.com/lexxmark/winflexbison", "v" + winflexbison.getPackage().getVersion().toString());
    winflexbison += src;

    auto &common = winflexbison.addTarget<StaticLibraryTarget>("common");
    {
        if (!common.getBuildSettings().TargetOS.is(OSType::Windows))
            common.HeaderOnly = true;
        common += "common/.*"_rr;
        common -= "common/m4/lib/regcomp.c";
        common -= "common/m4/lib/regexec.c";
        common -= ".*\\.def"_rr;
        common.Public += "common/m4/lib"_idir;
        common.Public += "common/misc"_idir;
        common.replaceInFileOnce("common/m4/lib/unistd-safer.h", "int dup_safer (int);", "//int dup_safer (int);");
        // clang does not like static assert on win
        common.replaceInFileOnce("common/misc/verify.h", "verify(R) static_assert", "verify(R) //static_assert");
    }

    // upgrade tweak on winflexbison updates
    auto &flex = winflexbison.addTarget<FlexExecutable>("flex", "2.6.4.3");
    {
        if (!flex.getBuildSettings().TargetOS.is(OSType::Windows))
            flex.HeaderOnly = true;
        flex += src;
        flex += "flex/.*"_rr;
        flex -= "flex/src/libmain.c";
        flex -= "flex/src/libyywrap.c";
        flex += common;
        if (flex.getBuildSettings().TargetOS.Type == OSType::Windows)
            flex += "ws2_32.lib"_slib;
    }

    auto &bison = winflexbison.addTarget<BisonExecutable>("bison", "3.8.2.1");
    {
        if (!bison.getBuildSettings().TargetOS.is(OSType::Windows))
            bison.HeaderOnly = true;
        bison += src;
        bison -= "bison/data/.*"_rr;
        bison += "bison/data/m4sugar/.*"_rr;
        bison += "bison/src/.*"_rr;
        bison -= "bison/src/scan-code.c";
        bison -= "bison/src/scan-gram.c";
        bison -= "bison/src/scan-skel.c";
        bison += "bison"_idir;
        bison.Public += "bison/src"_idir;
        bison += common;
        /*bison.replaceInFileOnce("bison/src/config.h", "\"data", "\"" + normalize_path(bison.SourceDir / "bison/data/"));
        bison.replaceInFileOnce("bison/src/files.c",
        "return cp ? cp : relocate2(PKGDATADIR, &relocate_buffer);",
        "\nreturn  cp ? cp : relocate2(PKGDATADIR, &relocate_buffer);");*/
        bison.replaceInFileOnce("bison/src/main.c", "if (!last_divider)", "");
        bison.replaceInFileOnce("bison/src/main.c", "free(local_pkgdatadir);", "");
        if (bison.getCompilerType() == CompilerType::MSVC) {
            bison.CompileOptions.push_back("-utf-8");
        }
    }
}
