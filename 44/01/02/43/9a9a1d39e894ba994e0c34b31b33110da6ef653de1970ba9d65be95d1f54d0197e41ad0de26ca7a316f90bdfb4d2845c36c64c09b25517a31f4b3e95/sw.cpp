void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("gnu.unistring", "0.9.10");
    t += RemoteFile("https://ftp.gnu.org/gnu/libunistring/libunistring-{v}.tar.xz");

    //t.ApiName = "SW_UNISTRING_API";

    t -= "lib/frexp.c";
    t -= "lib/frexpl.c";
    t -= "lib/iconv.*"_rr;
    t -= "lib/isnand.c";
    t -= "lib/iswblank.c";
    t -= "lib/memchr.c";
    t -= "lib/mbsinit.c";
    t -= "lib/strncat.c";
    t -= "lib/vasnprintf.c";
    t -= "lib/libunistring.rc";

    t += "IN_LIBUNISTRING"_def;
    //t += "LIBUNISTRING_DLL_VARIABLE="_def;

    t += "org.sw.demo.gnu.iconv.libiconv"_dep;

    t.configureFile("config.h.in", t.BinaryPrivateDir / "config.h");
    t.writeFileOnce(t.BinaryPrivateDir / "configmake.h");
    t.writeFileOnce(t.BinaryPrivateDir / "unistd.h");
    t.writeFileOnce("unistring/stdint.h", "#include <stdint.h>");
    t.writeFileOnce("unistring/stdbool.h", "#include <stdbool.h>");
    t.writeFileOnce("unistring/cdefs.h", "#define _GL_UNUSED_PARAMETER");
    t.writeFileOnce(t.BinaryPrivateDir / "alloca.h");
    t.writeFileOnce("unistring/woe32dll.h", "#define LIBUNISTRING_DLL_VARIABLE");
    //t.writeFileOnce("unistring/woe32dll.h", "#define LIBUNISTRING_DLL_VARIABLE SW_UNISTRING_API");

    t.writeFileOnce("unistring/iconveh.h", "#include <iconveh.h>");
    t.writeFileOnce("unistring/localcharset.h", "#include <localcharset.h>");
    t.configureFile("lib/unicase/special-casing.in.h", "unicase/special-casing.h");

    t.Variables["HEXVERSION"] = t.Variables["PACKAGE_VERSION_NUM"];
    t.configureFile("lib/unistring/version.in.h", "unistring/version.h");
}
