void build(Solution &s)
{
    auto &p = s.addProject("gnu.gpg");

    auto &a = p.addProject("assuan", "2.5.3");
    a += RemoteFile("https://www.gnupg.org/ftp/gcrypt/libassuan/libassuan-{v}.tar.bz2");

    auto &assuan = a.addTarget<LibraryTarget>("assuan");

    auto &mkheader = a.addExecutable("tools.mkheader");
    mkheader += "src/mkheader.c";

    assuan.PackageDefinitions = true;
    assuan.setChecks("assuan", true);

    assuan += "src/.*"_rr;
    assuan -=
        "src/funopen.c",
        "src/gpgce.*"_rr,
        "src/mkheader.c",
        "src/system-posix.c",
        "src/system-w32ce.c";
    assuan.Public +=
        "src"_id;

    assuan.Private += "BUILD_REVISION=\"\""_d;
    assuan.Private += "BUILD_TIMESTAMP=\"\""_d;
    assuan.Private += "HAVE_CONFIG_H"_d;

    assuan.Public += "org.sw.demo.gnu.gpg.error.gpg_error"_dep;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        assuan.patch("src/w32-types.inc.h", "typedef long ssize_t;", R"(
#ifdef _WIN64
#define ssize_t long long
#else
#define ssize_t int
#endif
)");
        assuan.patch("src/assuan-handler.c","{ } };", "};");
        assuan.Public += "ws2_32.lib"_slib;
    }

    {
        auto c = assuan.addCommand();
        c << cmd::prog(mkheader)
            << cmd::wdir(assuan.SourceDir / "src")
            << "mingw32"
            << cmd::in("src/assuan.h.in")
            << assuan.Variables["PACKAGE_VERSION"].toString()
            << assuan.Variables["PACKAGE_VERSION_NUM"].toString()
            << cmd::std_out("assuan.h")
            ;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("assuan");
    s.checkFunctionExists("flockfile");
    s.checkFunctionExists("fopencookie");
    s.checkFunctionExists("funlockfile");
    s.checkFunctionExists("funopen");
    s.checkFunctionExists("getaddrinfo");
    s.checkFunctionExists("getpeereid");
    s.checkFunctionExists("getpeerucred");
    s.checkFunctionExists("getrlimit");
    s.checkFunctionExists("inet_pton");
    s.checkFunctionExists("setsockopt");
    s.checkFunctionExists("stat");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/ucred.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("ucred.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("winsock2.h");
    s.checkTypeSize("signal");
    s.checkTypeSize("size_t");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("uintptr_t");
    s.checkTypeSize("void *");
    s.checkLibraryFunctionExists("socket", "setsockopt");
    {
        auto &c = s.checkStructMemberExists("struct cmsghdr", "cmsg_len");
    }
    {
        auto &c = s.checkStructMemberExists("struct sockpeercred", "pid");
    }
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>
int main() {return 0;}
)sw_xxx");
}
