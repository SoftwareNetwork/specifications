void build(Solution &s)
{
    auto &t = s.addTarget<Library>("gnu.libidn", "1.35.0");
    t += RemoteFile("https://ftp.gnu.org/gnu/libidn/libidn-{M}.{m}.tar.gz");

    t.ApiName = "IDNAPI";
    t.PackageDefinitions = true;

    t += "lib/.*"_r;
    t += "lib/gl/.*\\.h"_r;
    t += "lib/gl/c-.*"_r;
    t += "lib/gl/strverscmp.c";
    t += "lib/gl/unistr/.*"_rr;
    t += "windows/include/.*"_rr;
    t.Public += "lib"_idir;
    t += "lib/gl"_idir;

    t += "_GL_ATTRIBUTE_PURE="_def;
    t += "LOCALEDIR=\".\""_def;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t += "ssize_t=intptr_t"_def;
    }

    t.configureFile("config.h.in", t.BinaryPrivateDir / "config.h");
    t.writeFileOnce("idn-int.h", R"(#include <stdint.h>
#ifdef _WIN32
typedef intptr_t ssize_t;
#endif
)");

    t += "org.sw.demo.gnu.unistring"_dep;
}
