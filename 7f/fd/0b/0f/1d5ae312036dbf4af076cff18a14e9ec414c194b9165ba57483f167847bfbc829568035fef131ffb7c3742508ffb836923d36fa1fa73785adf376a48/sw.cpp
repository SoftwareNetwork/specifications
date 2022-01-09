void build(Solution &s)
{
    auto &pqxx = s.addTarget<LibraryTarget>("jtv.pqxx", "7.3.2");
    pqxx += Git("https://github.com/jtv/libpqxx", "{v}");

    pqxx.setChecks("pqxx", true);

    pqxx.ApiName = "PQXX_LIBEXPORT";
    pqxx += cpp17;

    if (pqxx.getBuildSettings().TargetOS.Type == OSType::Windows)
        pqxx.Public += "ws2_32.lib"_slib;

    pqxx += "org.sw.demo.postgres.pq"_dep;

    pqxx.writeFileOnce("pqxx/config-public-compiler.h");
    pqxx.writeFileOnce("pqxx/config-internal-libpq.h");
    pqxx.writeFileOnce("pqxx/config-internal-compiler.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("pqxx");
    s.checkFunctionExists("poll");
}
