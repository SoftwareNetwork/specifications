void build(Solution &s)
{
    auto &gflags = s.addTarget<LibraryTarget>("google.gflags", "2.3.0");
    gflags += Git("https://github.com/gflags/gflags", "v{v}");

    gflags.setChecks("gflags", true);

    gflags +=
        "src/.*\\.cc"_rr,
        "src/.*\\.h"_rr,
        "src/.*\\.in"_rr;

    //gflags += "GFLAGS_BAZEL_BUILD"_d; // mimic

    gflags += "src"_idir; // private!

    if (gflags.getBuildSettings().TargetOS.Type == OSType::Windows)
        gflags += "OS_WINDOWS"_d;
    gflags.Public += sw::Shared, "GFLAGS_IS_A_DLL=1"_d;
    gflags.Public += sw::Static, "GFLAGS_DLL_DEFINE_FLAG="_d;
    gflags.Public += sw::Static, "GFLAGS_IS_A_DLL=0"_d;

    gflags.Variables["GFLAGS_INTTYPES_FORMAT_C99"] = 1;
    gflags.Variables["GFLAGS_NAMESPACE"] = "google";
    gflags.Variables["NS"] = "GFLAGS";
    gflags.Variables["ns"] = "gflags";
    gflags.Variables["INCLUDE_GFLAGS_NS_H"] = "#include \"gflags/gflags_ns.h\"";

    gflags.writeFileOnce(gflags.BinaryPrivateDir / "defines.h");

    gflags.configureFile("src/gflags.h.in", "gflags/gflags.h");
    gflags.configureFile("src/gflags_completions.h.in", "gflags/gflags_completions.h");
    gflags.configureFile("src/gflags_declare.h.in", "gflags/gflags_declare.h");
    gflags.configureFile("src/gflags_ns.h.in", "gflags/gflags_ns.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("gflags");
    s.checkFunctionExists("strtoll");
    s.checkFunctionExists("strtoq");
    s.checkIncludeExists("fnmatch.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("unistd.h");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
}
