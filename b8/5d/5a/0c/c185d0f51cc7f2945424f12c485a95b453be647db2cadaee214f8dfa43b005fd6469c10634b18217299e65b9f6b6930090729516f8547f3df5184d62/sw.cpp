void build(Solution &s)
{
    auto &pqxx = s.addTarget<LibraryTarget>("jtv.pqxx", "6.3.3");
    pqxx += Git("https://github.com/jtv/libpqxx", "{v}");

    pqxx.CPPVersion = CPPLanguageStandard::CPP17;
    pqxx.ApiName = "PQXX_LIBEXPORT";

    if (s.Settings.TargetOS.Type == OSType::Windows)
        pqxx.Public += "ws2_32.lib"_slib;

    pqxx.Public += "org.sw.demo.find.libpq-master"_dep;

    pqxx.writeFileOnce("pqxx/config-public-compiler.h");
    pqxx.writeFileOnce("pqxx/config-internal-libpq.h");
    pqxx.writeFileOnce("pqxx/config-internal-compiler.h");
}
