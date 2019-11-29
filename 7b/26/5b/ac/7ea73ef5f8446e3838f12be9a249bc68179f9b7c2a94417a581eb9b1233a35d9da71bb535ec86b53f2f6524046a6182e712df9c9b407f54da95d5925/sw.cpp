void build(Solution &s)
{
    auto &pqxx = s.addTarget<LibraryTarget>("jtv.pqxx", "6.4.5");
    pqxx += Git("https://github.com/jtv/libpqxx", "{v}");

    pqxx.setChecks("pqxx", true);

    pqxx.CPPVersion = CPPLanguageStandard::CPP17;
    pqxx.ApiName = "PQXX_LIBEXPORT";

    if (pqxx.getBuildSettings().TargetOS.Type == OSType::Windows)
        pqxx.Public += "ws2_32.lib"_slib;

    pqxx.Public += "org.sw.demo.find.libpq-master"_dep;

    pqxx.writeFileOnce("pqxx/config-public-compiler.h");
    pqxx.writeFileOnce("pqxx/config-internal-libpq.h");
    pqxx.writeFileOnce("pqxx/config-internal-compiler.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("pqxx");
    s.checkFunctionExists("poll");
}
