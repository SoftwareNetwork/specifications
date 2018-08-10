void build(Solution &s)
{
    auto &gss = s.addTarget<StaticLibraryTarget>("gnu.gss", "1.0.3");
    gss += RemoteFile("https://ftp.gnu.org/gnu/gss/gss-{M}.{m}.{p}.tar.gz");

    gss +=
        "lib/[^/]*\\.c"_rr,
        "lib/[^/]*\\.h"_rr,
        "lib/gl/strverscmp.c",
        "lib/headers/.*\\.h"_rr;

    gss -=
        "lib/gl/strverscmp.c";

    gss.Public +=
        "lib/headers"_id;

    gss.Public += "org.sw.demo.gnu.gettext.intl"_dep;

    gss.Definitions["PACKAGE"] = "\"" + gss.pkg.ppath.toString() + "\"";
    gss.Definitions["PACKAGE_VERSION"] = "\"1.0.3\"";
    gss.Public += "PO_SUFFIX=\".po\""_d;

    gss.fileWriteOnce(gss.BinaryPrivateDir / "config.h", "", true);
    gss.pushFrontToFileOnce("lib/version.c", R"xxx(
#include "gl/strverscmp.c"
#define strverscmp __strverscmp
)xxx");
}

void check(Checker &c)
{
    {
        auto &s = c.addSet("gss");
        s.checkTypeSize("size_t");
        s.checkTypeSize("void *");
    }
}
