#pragma sw header on

#pragma sw require pub.egorpugin.primitives.emitter-master

#include <primitives/emitter.h>

namespace flex_bison
{

static bool need_build(Solution &s)
{
    return
        // s.Settings.Native.CompilerType != CompilerType::GNU
        s.getHostOs().Type == OSType::Windows // && !::sw::detail::isHostCygwin()
        ;
}

} // namespace flex_bison

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
    auto bison = std::make_shared<Dependency>(base->package);
    bison->package.ppath /= "bison";

    d.setupFiles(t, ".y");

    auto c = t.addCommand();
    c << cmd::wdir(d.wdir);
    if (flex_bison::need_build(t.getSolution()))
        c << cmd::prog(bison);
    else
        c << "bison";
    c << cmd::out(d.out, cmd::Prefix{ "--output=" });
    c << cmd::out(d.outh, cmd::Prefix{ "--defines=" });
    for (auto &a : d.args)
        c << a;
    c << cmd::in(d.in);

    return c.c;
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
    auto flex = std::make_shared<Dependency>(base->package);
    flex->package.ppath /= "flex";

    d.setupFiles(t, ".l");

    auto c = t.addCommand();
    c << cmd::wdir(d.wdir);
    if (flex_bison::need_build(t.getSolution()))
        c << cmd::prog(flex);
    else
        c << "flex";
    c << "-o" << cmd::out(d.out);
    for (auto &a : d.args)
        c << a;
    c << cmd::in(d.in);

    return c.c;
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

    primitives::Emitter ctx;
    ctx.addLine("#pragma once");
    ctx.addLine();
    ctx.addLine("#undef  THIS_PARSER_NAME");
    ctx.addLine("#undef  THIS_PARSER_NAME_UP");
    ctx.addLine("#undef  THIS_LEXER_NAME");
    ctx.addLine("#undef  THIS_LEXER_NAME_UP");
    ctx.addLine();
    ctx.addLine("#define THIS_PARSER_NAME       " + name);
    ctx.addLine("#define THIS_PARSER_NAME_UP    " + name_upper);
    ctx.addLine("#define THIS_LEXER_NAME        THIS_PARSER_NAME");
    ctx.addLine("#define THIS_LEXER_NAME_UP     THIS_PARSER_NAME_UP");
    ctx.addLine();
    ctx.addLine("#undef  MY_PARSER");
    ctx.addLine("#define MY_PARSER              " + my_parser);
    ctx.addLine();
    ctx.addLine("#define " + type);
    ctx.addLine("#include <primitives/helper/bison.h>");
    ctx.addLine("#undef  " + type);
    ctx.addLine();
    ctx.addLine("#include <" + name + ".yy.hpp>");

    t.writeFileOnce(t.BinaryPrivateDir / (name + "_parser.h"), ctx.getText());
    t.Definitions["HAVE_BISON_" + name_upper + "_PARSER"] = 1;

    auto f = p;
    auto b = p;
    return gen_flex_bison(base, t, f += ".ll", b += ".yy", {"--prefix=ll_" + name}, {"-Dapi.prefix={yy_" + name + "}"});
};

#pragma sw header off

void build(Solution &s)
{
    bool win_flex_bison = flex_bison::need_build(s);
    if (!win_flex_bison && !s.DryRun)
        return;

    auto &winflexbison = s.addProject("lexxmark.winflexbison", "master");
    winflexbison += Git("https://github.com/lexxmark/winflexbison", "", "master");

    auto &common = winflexbison.addTarget<StaticLibraryTarget>("common");
    common += "common/.*"_rr;
    common -= "common/m4/lib/regcomp.c";
    common -= "common/m4/lib/regexec.c";
    common -= ".*\\.def"_rr;
    common.Public += "common/m4/lib"_idir;
    common.Public += "common/misc"_idir;
    // clang does not like static assert on win
    common.replaceInFileOnce("common/misc/verify.h", "verify(R) static_assert", "verify(R) //static_assert");

    auto &flex = winflexbison.addTarget<ExecutableTarget>("flex");
    flex += "flex/.*"_rr;
    flex -= "flex/src/libmain.c";
    flex -= "flex/src/libyywrap.c";
    flex += common;
    if (flex.getSettings().TargetOS.Type == OSType::Windows)
        flex += "ws2_32.lib"_slib;

    auto &bison = winflexbison.addTarget<ExecutableTarget>("bison");
    bison -= "bison/data/.*"_rr;
    bison += "bison/data/m4sugar/.*"_rr;
    bison += "bison/src/.*"_rr;
    bison += "bison"_idir;
    bison.Public += "bison/src"_idir;
    bison += common;
    bison.replaceInFileOnce("bison/src/config.h", "\"data", "\"" + normalize_path(bison.SourceDir / "bison/data/"));
    bison.replaceInFileOnce("bison/src/main.c", "if (!last_divider)", "");
    bison.replaceInFileOnce("bison/src/main.c", "free(local_pkgdatadir);", "");
}
