void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("libsigpp", "2.10.3");
    t += Git("https://github.com/libsigcplusplus/libsigcplusplus");

    t +=
        "sigc\\+\\+/.*\\.m4"_rr,
        "sigc\\+\\+/.*\\.cc"_rr,
        "sigc\\+\\+/.*\\.h"_rr;

    t += "SIGC_BUILD"_d;
    t.Public += sw::Shared, "SIGC_DLL"_d;

    t.Variables["SIGCXX_DISABLE_DEPRECATED"] = 1;
    t.Variables["SIGCXX_MAJOR_VERSION"] = t.Variables["PACKAGE_MAJOR_VERSION"];
    t.Variables["SIGCXX_MINOR_VERSION"] = t.Variables["PACKAGE_MINOR_VERSION"];
    t.Variables["SIGCXX_MICRO_VERSION"] = t.Variables["PACKAGE_PATCH_VERSION"];
    t.configureFile("sigc++config.h.in", "sigc++config.h", ConfigureFlags::EnableUndefReplacements);

    for (auto &[p, sf] : t[".*\\.[hc]c?\\.m4"_rr])
    {
        auto rel = p.lexically_relative(t.SourceDir);
        auto c = t.addCommand();
        c << cmd::prog("org.sw.demo.gnu.m4.m4"_dep)
            << "-I" << normalize_path(t.SourceDir / "sigc++" / "macros")
            << "-I" << "macros"
            << cmd::in(p)
            << cmd::std_out(rel.parent_path().parent_path() / rel.stem())
            ;
    }
}
