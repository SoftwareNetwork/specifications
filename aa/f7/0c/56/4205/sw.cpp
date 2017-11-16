void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &libssh2 = s.addTarget<LibraryTarget>("libssh2", "1.8.0");
    libssh2.Source = Git("https://github.com/libssh2/libssh2", "libssh2-{v}");

    libssh2.setChecks("libssh2");

    libssh2 +=
        "include/.*"_rr,
        "src/.*\\.c"_rr,
        "src/.*\\.h"_rr,
        "src/.*\\.in"_rr;

    libssh2.Private +=
        "src"_id;

    libssh2.Public +=
        "include"_id;

    libssh2.Private += "LIBSSH2_LIBRARY"_d;
    libssh2.Public += "LIBSSH2_OPENSSL"_d;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        libssh2.Public += sw::Shared, "LIBSSH2_WIN32"_d;
    }

    if (
        libssh2.Variables["HAVE_O_NONBLOCK"] == "0" &&
        libssh2.Variables["HAVE_IOCTLSOCKET"] == "0" &&
        libssh2.Variables["HAVE_IOCTLSOCKET_CASE"] == "0" &&
        libssh2.Variables["HAVE_SO_NONBLOCK"] == "0"
        )
        libssh2.Variables["HAVE_DISABLED_NONBLOCKING"] == "1";

    libssh2.configureFile("src/libssh2_config_cmake.h.in", "libssh2_config.h");

    libssh2.Public += "pub.cppan2.demo.openssl.ssl-1"_dep;
}

void check(Checker &c)
{
    auto &s = c.addSet("libssh2");
    s.checkFunctionExists("alloca");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("select");
    s.checkFunctionExists("strtoll");
    s.checkIncludeExists("arpa/inet.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("netinet/in.h");
    s.checkIncludeExists("stdio.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("sys/un.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("windows.h");
    s.checkIncludeExists("winsock2.h");
    s.checkIncludeExists("ws2tcpip.h");
    s.checkTypeSize("long");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    {
        auto &mb = s.checkSymbolExists("snprintf");
        mb.Parameters.Includes.push_back("stdio.h");
    }
    
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <float.h>
int main() {return 0;}
)sw_xxx");

    if (c.solution->Settings.TargetOS.Type != OSType::Windows)
    s.checkSourceCompiles("HAVE_O_NONBLOCK", R"sw_xxx(
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#if defined(sun) || defined(__sun__) || defined(__SUNPRO_C) || defined(__SUNPRO_CC)
# if defined(__SVR4) || defined(__srv4__)
#  define PLATFORM_SOLARIS
# else
#  define PLATFORM_SUNOS4
# endif
#endif
#if (defined(_AIX) || defined(__xlC__)) && !defined(_AIX41)
# define PLATFORM_AIX_V3
#endif
#if defined(PLATFORM_SUNOS4) || defined(PLATFORM_AIX_V3) || defined(__BEOS__)
#error \"O_NONBLOCK does not work on this platform\"
#endif
int main()
{
int socket;
int flags = fcntl(socket, F_SETFL, flags | O_NONBLOCK);
}
)sw_xxx");

    if (c.solution->Settings.TargetOS.Type != OSType::Windows)
    s.checkSourceCompiles("HAVE_FIONBIO", R"sw_xxx(
/* FIONBIO test (old-style unix) */
#include <unistd.h>
#include <stropts.h>
int main()
{
int socket;
int flags = ioctl(socket, FIONBIO, &flags);
}
)sw_xxx");

    s.checkSourceCompiles("HAVE_IOCTLSOCKET", R"sw_xxx(
/* ioctlsocket test (Windows) */
#undef inline
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <winsock2.h>
int main()
{
SOCKET sd;
unsigned long flags = 0;
sd = socket(0, 0, 0);
ioctlsocket(sd, FIONBIO, &flags);
}
)sw_xxx");

    s.checkSourceCompiles("HAVE_IOCTLSOCKET_CASE", R"sw_xxx(
/* IoctlSocket test (Amiga?) */
#include <sys/ioctl.h>
int main()
{
int socket;
int flags = IoctlSocket(socket, FIONBIO, (long)1);
}
)sw_xxx");

    s.checkSourceCompiles("HAVE_SO_NONBLOCK", R"sw_xxx(
/* SO_NONBLOCK test (BeOS) */
#include <socket.h>
int main()
{
long b = 1;
int socket;
int flags = setsockopt(socket, SOL_SOCKET, SO_NONBLOCK, &b, sizeof(b));
}
)sw_xxx");

}
