void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("libsigpp", "3.0.0");
    t += Git("https://github.com/libsigcplusplus/libsigcplusplus");

    t +=
        "sigc\\+\\+/.*\\.cc"_rr,
        "sigc\\+\\+/.*\\.h"_rr;

    t += "SIGC_BUILD"_d;
    t.Public += sw::Shared, "SIGC_DLL"_d;

    t.Variables["SIGCXX_DISABLE_DEPRECATED"] = 1;
    t.Variables["SIGCXX_MAJOR_VERSION"] = t.Variables["PACKAGE_MAJOR_VERSION"];
    t.Variables["SIGCXX_MINOR_VERSION"] = t.Variables["PACKAGE_MINOR_VERSION"];
    t.Variables["SIGCXX_MICRO_VERSION"] = t.Variables["PACKAGE_PATCH_VERSION"];
    t.configureFile("sigc++config.h.in", "sigc++config.h", ConfigureFlags::EnableUndefReplacements);
}
