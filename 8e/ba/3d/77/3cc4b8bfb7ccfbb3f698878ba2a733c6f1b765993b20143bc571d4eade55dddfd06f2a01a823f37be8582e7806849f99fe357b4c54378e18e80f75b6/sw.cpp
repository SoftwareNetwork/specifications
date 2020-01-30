void build(Solution &s)
{
    auto &t = s.addTarget<Library>("gnu.libidn", "2.3.0");
    t += RemoteFile("https://ftp.gnu.org/gnu/libidn/libidn2-{v}.tar.gz");

    t.ApiName = "_IDN2_API";

    t += "lib/.*"_r;
    t -= "lib/gentr46map.c";
    t += "gl/rawmemchr.c";
    t += "gl/strchrnul.c";
    t += "gl/strverscmp.c";

    t.pushBackToFileOnce("config.h.in", R"(
#include <stdint.h>
#ifdef _WIN32
typedef intptr_t ssize_t;
#endif
)");
    t.configureFile("config.h.in", t.BinaryPrivateDir / "config.h");

    t += "org.sw.demo.gnu.unistring"_dep;
}
