#pragma sw header on

#pragma sw require pub.egorpugin.primitives.context-master

#include <primitives/context.h>

namespace flex_bison
{

static bool need_build(Solution &s)
{
    return
        // s.Settings.Native.CompilerType != CompilerType::GNU
        s.HostOS.Type == OSType::Windows // && !::sw::detail::isHostCygwin()
        ;
}

} // namespace flex_bison

static auto gen_flex_bison(const DependencyPtr &base, NativeExecutedTarget &t, const path &f, const path &b,
                           const Strings &flex_args = {}, const Strings &bison_args = {})
{
    // must be HostOS
    bool win_flex_bison = flex_bison::need_build(*t.getSolution());

    auto flex = std::make_shared<Dependency>(base->package);
    flex->package.ppath /= "flex";

    auto bison = std::make_shared<Dependency>(base->package);
    bison->package.ppath /= "bison";

    if (win_flex_bison)
    {
        auto d = t + flex;
        d->Dummy = true;
    }

    if (win_flex_bison)
    {
        auto d = t + bison;
        d->Dummy = true;
    }

    auto d = b.filename();
    auto bdir = t.BinaryPrivateDir / "fb" / d;

    auto o = bdir / (b.filename().u8string() + ".cpp");
    auto oh = bdir / (b.filename().u8string() + ".hpp");
    t += IncludeDirectory(oh.parent_path());

    fs::create_directories(bdir);

    SW_MAKE_COMMAND_AND_ADD(bc, t);
    {
        auto c = bc;
        if (win_flex_bison)
            c->setProgram(bison);
        else
            c->setProgram("bison");
        c->working_directory = bdir;
        c->args.push_back("-o");
        c->args.push_back(o.u8string());
        c->args.push_back("--defines=" + oh.u8string());
        c->args.insert(c->args.end(), bison_args.begin(), bison_args.end());
        c->args.push_back((t.SourceDir / b).u8string());
        c->addInput(t.SourceDir / b);
        c->addOutput(o);
        c->addOutput(oh);
        t += o, oh;
    }

    SW_MAKE_COMMAND_AND_ADD(fc, t);
    {
        auto o = bdir / (f.filename().u8string() + ".cpp");

        auto c = fc;
        if (win_flex_bison)
            c->setProgram(flex);
        else
            c->setProgram("flex");
        c->working_directory = bdir;
        c->args.push_back("-o");
        c->args.push_back(o.u8string());
        c->args.insert(c->args.end(), flex_args.begin(), flex_args.end());
        c->args.push_back((t.SourceDir / f).u8string());
        c->addInput(t.SourceDir / f);
        c->addInput(oh);
        c->addOutput(o);
        t += o;
    }

    return std::tuple{fc, bc};
}

static auto gen_flex_bison_pair(const DependencyPtr &base, NativeExecutedTarget &t, const String &type, const path &p)
{
    auto name = p.filename().string();
    auto name_upper = boost::to_upper_copy(name);
    auto my_parser = name + "Parser";
    my_parser[0] = toupper(my_parser[0]);

    t.Definitions["HAVE_BISON_" + name_upper + "_PARSER"];

    primitives::Context ctx;
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
    if (s.Settings.TargetOS.Type == OSType::Windows)
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
