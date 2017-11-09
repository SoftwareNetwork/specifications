void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &gss = s.addTarget<StaticLibraryTarget>("gnu.gss", "1.0.3");
    gss.Source = RemoteFile("http://ftp.gnu.org/gnu/gss/gss-{M}.{m}.{p}.tar.gz");

    gss +=
        "lib/[^/]*\\.c"_rr,
        "lib/[^/]*\\.h"_rr,
        "lib/gl/strverscmp.c",
        "lib/headers/.*\\.h"_rr;

    gss -=
        "lib/gl/strverscmp.c";

    gss.Public +=
        "lib/headers"_id;

    gss.Public += "pub.cppan2.demo.gnu.gettext.intl"_dep;

    gss.Definitions["PACKAGE"] = "\"" + gss.Name.toString() + "\"";
    gss.Definitions["PACKAGE_VERSION"] = "\"1.0.3\"";
    gss.Public += "PO_SUFFIX=\".po\""_d;

    gss.fileWriteOnce("config.h", "", true);
    gss.pushFrontToFileOnce("lib/version.c", R"xxx(
#include "gl/strverscmp.c"
#define strverscmp __strverscmp
)xxx");
}