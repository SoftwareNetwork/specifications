void build(Solution &s)
{
    auto &pqxx = s.addTarget<LibraryTarget>("jtv.pqxx", "6.2.5");
    pqxx += Git("https://github.com/jtv/libpqxx", "{v}");

    pqxx.setChecks("pqxx");

    pqxx.Public += "HAVE_PQEXEC"_d;
    pqxx.Public += "PGSTD=std"_d;
    pqxx.Public += "PQXXTR1=std"_d;
    pqxx.Public += "PQXX_HAVE_PQCANCEL"_d;
    pqxx.Public += "PQXX_HAVE_PQCLIENTENCODING"_d;
    pqxx.Public += "PQXX_HAVE_PQDESCRIBEPORTAL"_d;
    pqxx.Public += "PQXX_HAVE_PQENCRYPTPASSWORD"_d;
    pqxx.Public += "PQXX_HAVE_PQESCAPEIDENTIFIER"_d;
    pqxx.Public += "PQXX_HAVE_PQESCAPELITERAL"_d;
    pqxx.Public += "PQXX_HAVE_PQISTHREADSAFE"_d;
    pqxx.Public += "PQXX_HAVE_PQMBLEN"_d;
    pqxx.Public += "PQXX_HAVE_PQUNESCAPEBYTEA_9"_d;
    pqxx.Public += "PQXX_HAVE_QUIET_NAN"_d;
    pqxx.Public += "PQXX_HAVE_REVERSE_ITERATOR"_d;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        pqxx.Public += "NOMINMAX"_d;
    }
    pqxx.ApiName = "PQXX_LIBEXPORT";

    if (s.Settings.TargetOS.Type == OSType::Windows)
        pqxx.Public += "ws2_32.lib"_lib;

    pqxx.Public += "org.sw.demo.find.libpq-master"_dep;

    pqxx.writeFileOnce("pqxx/config-public-compiler.h", true);
    pqxx.writeFileOnce("pqxx/config-internal-libpq.h", true);
    pqxx.writeFileOnce("pqxx/config-internal-compiler.h", true);
}

void check(Checker &c)
{
    auto &s = c.addSet("pqxx");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("ios");
    s.checkIncludeExists("limits");
    s.checkIncludeExists("locale");
    s.checkIncludeExists("streambuf");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
}
