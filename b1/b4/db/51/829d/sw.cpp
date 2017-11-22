void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");

    auto &pqxx = s.addTarget<LibraryTarget>("jtv.pqxx", "5.1.1");
    pqxx.Source = Git("https://github.com/jtv/libpqxx", "{v}", "{b}");
    pqxx.CPPVersion = CPPLanguageStandard::CPP11;
    pqxx.ApiName = "SW_PQXX_API";

    pqxx.setChecks("pqxx");

    pqxx +=
        "include/pqxx/.*"_rr,
        "src/.*\\.cxx"_rr;

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
    pqxx.Public += sw::Shared, "PQXX_LIBEXPORT=SW_PQXX_API"_d;

    pqxx.writeFileOnce("pqxx/config-public-compiler.h", true);
    pqxx.writeFileOnce("pqxx/config-internal-libpq.h", true);
    pqxx.writeFileOnce("pqxx/config-internal-compiler.h", true);

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        pqxx.Public += "ws2_32.lib"_lib;

        path root = "c:\\Program Files (x86)\\PostgreSQL";
        if (s.Settings.TargetOS.Arch == ArchType::x86_64)
            root = "c:\\Program Files\\PostgreSQL";
        for (auto &v : { "10", "9.6", "9.5", "9.4", "9.3", "9.2", "9.1", "9.0", "8.4", "8.3", "8.2", "8.1", "8.0" })
        {
            bool e =
                fs::exists(root / v / "include") &&
                fs::exists(root / v / "include" / "server") &&
                fs::exists(root / v / "lib");
            if (e)
            {
                pqxx.Public += IncludeDirectory(root / v / "include");
                pqxx.Public += IncludeDirectory(root / v / "include" / "server");
                pqxx.Public += LinkLibrary{ (root / v / "lib" / "libpq.lib").string() };
                break;
            }
        }
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("pqxx");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("boost/smart_ptr.hpp");
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
