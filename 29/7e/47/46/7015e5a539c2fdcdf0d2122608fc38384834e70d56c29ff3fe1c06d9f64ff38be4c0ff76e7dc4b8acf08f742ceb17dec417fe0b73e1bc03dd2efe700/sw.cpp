#pragma sw require header org.sw.demo.lexxmark.winflexbison.bison

struct SwigExecutable : ExecutableTarget
{
    using ExecutableTarget::ExecutableTarget;

    void setupCommand(builder::Command &c) const override
    {
        c.environment["SWIG_LIB"] = to_string(to_path_string(SourceDir / "Lib"));
    }
};

void build(Solution &s)
{
    auto &swig = s.addTarget<SwigExecutable>("swig", "4.4.0");
    swig += Git("https://github.com/swig/swig", "v{v}");

    swig.PackageDefinitions = true;
    swig -= "Lib/.*"_rr;
    swig += "Source/.*\\.[hcy](xx)?"_rr;
    swig += "Source/Swig"_idir;
    swig += "Source/DOH"_idir;
    swig += "Source/Preprocessor"_idir;
    swig += "Source/Include"_idir;
    swig += "Source/CParse"_idir;
    swig += "Source/Doxygen"_idir;
    swig += "Source/Modules"_idir;
    swig.writeFileOnce("swigconfig.h");
    gen_bison("org.sw.demo.lexxmark.winflexbison"_dep, swig, "Source/CParse/parser.y", "parser.c");

    swig += "SWIG_CXX=\"\""_def;
    swig += "SWIG_PLATFORM=\"\""_def;
    swig += "SWIG_LIB_WIN_UNIX=\"\""_def;
    swig += Definition("SWIG_LIB=\"" + to_printable_string(normalize_path(swig.SourceDir / "Lib")) + "\"");

    {
        auto c = swig.addCommand();
        c << cmd::prog("org.sw.demo.gnu.grep.grep-*"_dep)
            << R"xxx(^#define WARN\|/\*.*\*/\|^[ \t]*$$)xxx"
            << cmd::std_in("Source/Include/swigwarn.h")
            << cmd::std_out("swigwarn.h.1")
            ;
    }

    {
        auto c = swig.addCommand();
        c << cmd::prog("org.sw.demo.gnu.sed.sed-*"_dep)
            << "-E"
            << R"xxx(s/^#define (WARN.*[0-9][0-9]*)(.*)$$/%define SWIG\1 %enddef\2/)xxx"
            << cmd::std_in("swigwarn.h.1")
            << cmd::std_out(swig.SourceDir / "Lib/swigwarn.swg")
            ;
    }
}
