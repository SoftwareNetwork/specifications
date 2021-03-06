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

    //gss += "USE_KERBEROS5"_def;
    //gss += "lib/krb5/.*"_rr;
    //gss += "lib/"_idir;
    //gss += "org.sw.demo.gnu.shishi"_dep;

    gss.Public += "lib/headers"_id;

    gss.Public += "org.sw.demo.gnu.gettext.intl"_dep;

    gss.Definitions["PACKAGE"] = "\"" + gss.getPackage().getPath().toString() + "\"";
    gss.Definitions["PACKAGE_VERSION"] = "\"" + gss.getPackage().getVersion().toString() + "\"";
    gss.Public += "PO_SUFFIX=\".po\""_d;

    gss.writeFileOnce(gss.BinaryPrivateDir / "config.h");
    gss.patch("lib/version.c", "#include \"internal.h\"", R"xxx(
#include "gl/strverscmp.c"
#define strverscmp __strverscmp
#include  "internal.h"
)xxx");
}
