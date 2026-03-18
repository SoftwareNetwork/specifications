void build(Solution &s)
{
    auto &pqxx = s.addTarget<LibraryTarget>("jtv.pqxx", "8.0.0");
    pqxx += Git("https://github.com/jtv/libpqxx", "{v}");

    pqxx.setChecks("pqxx", true);

    pqxx.ApiName = "PQXX_LIBEXPORT";
    pqxx += cpp20;

    if (pqxx.getBuildSettings().TargetOS.Type == OSType::Windows) {
        pqxx += "ws2_32.lib"_slib;
    } else {
        pqxx += "PQXX_HAVE_POLL=1"_v;
        pqxx += "PQXX_HAVE_GCC_VISIBILITY=1"_v;
    }

    pqxx += "org.sw.demo.postgres.pq"_dep;

    pqxx += "HAVE_CXX20=1"_v;
    pqxx += "PQXX_HAVE_SLEEP_FOR=1"_v;
    pqxx += "PQXX_HAVE_CHARCONV_INT=1"_v;
    pqxx += "PQXX_HAVE_CHARCONV_FLOAT=1"_v;

    pqxx.Variables["PACKAGE"] = "\"" + pqxx.Variables["PACKAGE"].toString() + "\"";
    pqxx.Variables["PACKAGE_BUGREPORT"] = "\"" + pqxx.Variables["PACKAGE_BUGREPORT"].toString() + "\"";
    pqxx.Variables["PACKAGE_NAME"] = "\"" + pqxx.Variables["PACKAGE_NAME"].toString() + "\"";
    pqxx.Variables["PACKAGE_STRING"] = "\"" + pqxx.Variables["PACKAGE_STRING"].toString() + "\"";
    pqxx.Variables["PACKAGE_TARNAME"] = "\"" + pqxx.Variables["PACKAGE_TARNAME"].toString() + "\"";
    pqxx.Variables["PACKAGE_URL"] = "\"" + pqxx.Variables["PACKAGE_URL"].toString() + "\"";
    pqxx.Variables["PACKAGE_VERSION"] = "\"" + pqxx.Variables["PACKAGE_VERSION"].toString() + "\"";

    pqxx.writeFileOnce("pqxx/config-public-compiler.h");
    pqxx.writeFileOnce("pqxx/config-internal-libpq.h");
    pqxx.writeFileOnce("pqxx/config-internal-compiler.h");
    pqxx.configureFile("include/pqxx/config.h.in", "pqxx/config-compiler.h", ConfigureFlags::EnableUndefReplacements);
}

void check(Checker &c)
{
    auto &s = c.addSet("pqxx");
    s.checkFunctionExists("poll");
}
