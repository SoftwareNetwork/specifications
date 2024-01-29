void build(Solution &s)
{
    auto recv_args = [&s](auto &t)
    {
        if (t.Variables["HAVE_SSIZE_T"] && t.Variables["HAVE_SOCKLEN_T"])
        {
            t.Variables["RECVFROM_TYPE_RETV"] = "ssize_t";
            t.Variables["RECVFROM_TYPE_ARG3"] = "size_t";
        }
        else
        {
            t.Variables["RECVFROM_TYPE_RETV"] = "int";
            t.Variables["RECVFROM_TYPE_ARG3"] = "int";
        }

        if (t.getBuildSettings().TargetOS.Type == OSType::Windows || t.getBuildSettings().TargetOS.Type == OSType::Mingw)
            t.Variables["RECVFROM_TYPE_ARG1"] = "SOCKET";
        else
            t.Variables["RECVFROM_TYPE_ARG1"] = "int";

        if (t.Variables["HAVE_SOCKLEN_T"])
        {
            t.Variables["RECVFROM_TYPE_ARG6"] = "socklen_t *";
            t.Variables["GETNAMEINFO_TYPE_ARG2"] = "socklen_t";
            t.Variables["GETNAMEINFO_TYPE_ARG46"] = "socklen_t";
        }
        else
        {
            t.Variables["RECVFROM_TYPE_ARG6"] = "int *";
            t.Variables["GETNAMEINFO_TYPE_ARG2"] = "int";
            t.Variables["GETNAMEINFO_TYPE_ARG46"] = "int";
        }

        t.Variables["RECV_TYPE_RETV"] = t.Variables["RECVFROM_TYPE_RETV"];
        t.Variables["SEND_TYPE_RETV"] = t.Variables["RECVFROM_TYPE_RETV"];
        t.Variables["RECV_TYPE_ARG1"] = t.Variables["RECVFROM_TYPE_ARG1"];
        t.Variables["SEND_TYPE_ARG1"] = t.Variables["RECVFROM_TYPE_ARG1"];
        t.Variables["RECV_TYPE_ARG3"] = t.Variables["RECVFROM_TYPE_ARG3"];
        t.Variables["SEND_TYPE_ARG3"] = t.Variables["RECVFROM_TYPE_ARG3"];
        t.Variables["GETHOSTNAME_TYPE_ARG2"] = t.Variables["RECVFROM_TYPE_ARG3"];

        // These should always be "sane" values to use always
        t.Variables["RECVFROM_QUAL_ARG5"] = "";
        t.Variables["RECVFROM_TYPE_ARG2"] = "void *";
        t.Variables["RECVFROM_TYPE_ARG4"] = "int";
        t.Variables["RECVFROM_TYPE_ARG5"] = "struct sockaddr *";
        t.Variables["RECV_TYPE_ARG2"] = "void *";
        t.Variables["RECV_TYPE_ARG4"] = "int";
        t.Variables["GETNAMEINFO_TYPE_ARG1"] = "struct sockaddr *";
        t.Variables["GETNAMEINFO_TYPE_ARG7"] = "int";
        t.Variables["SEND_TYPE_ARG2"] = "void *";
        t.Variables["SEND_QUAL_ARG2"] = "const";
        t.Variables["SEND_TYPE_ARG4"] = "int";

        if (t.Variables["HAVE_FCNTL"] && t.Variables["HAVE_O_NONBLOCK"])
            t.Variables["HAVE_FCNTL_O_NONBLOCK"] = 1;

        if (t.Variables["HAVE_IOCTL"] && t.Variables["HAVE_FIONBIO"])
            t.Variables["HAVE_IOCTL_FIONBIO"] = 1;

        if (t.Variables["HAVE_IOCTLSOCKET"] && t.Variables["HAVE_FIONBIO"])
            t.Variables["HAVE_IOCTLSOCKET_FIONBIO"] = 1;

        if (t.Variables["HAVE_IOCTLSOCKET_CAMEL"] && t.Variables["HAVE_FIONBIO"])
            t.Variables["HAVE_IOCTLSOCKET_CAMEL_FIONBIO"] = 1;
    };

    auto &c_ares = s.addTarget<LibraryTarget>("c_ares", "1.26.0");
    c_ares += Git("https://github.com/c-ares/c-ares", "cares-{M}_{m}_{p}");

    c_ares.setChecks("c_ares");

    if (c_ares.getBuildSettings().TargetOS.Type != OSType::Windows && c_ares.getBuildSettings().TargetOS.Type != OSType::Mingw)
        c_ares.ExportAllSymbols = true;

    c_ares += "include/.*\\.h"_rr;
    c_ares += "include/.*\\.in"_rr;
    c_ares += "src/lib/.*\\.[hc]"_rr;

    c_ares.Public += "include"_idir;
    c_ares += "src/lib"_idir;

    c_ares.Private += "CARES_BUILDING_LIBRARY"_d;
    c_ares.Public += "CARES_SYMBOL_HIDING"_d;
    c_ares.Public += sw::Static, "CARES_STATICLIB"_d;

    c_ares.Private += "HAVE_CONFIG_H"_d;
    c_ares.Public += "HAVE_GETENV"_d;
    c_ares.Public += "HAVE_RECV"_d;
    c_ares.Public += "HAVE_RECVFROM"_d;
    c_ares.Public += "HAVE_SEND"_d;
    c_ares.Public += "HAVE_SENDTO"_d;
    c_ares.Public += "HAVE_STRUCT_ADDRINFO"_d;
    c_ares.Public += "HAVE_STRUCT_SOCKADDR_IN6"_d;
    c_ares.Public += "HAVE_STRUCT_TIMEVAL"_d;
    if (c_ares.getBuildSettings().TargetOS.Type == OSType::Windows || c_ares.getBuildSettings().TargetOS.Type == OSType::Mingw)
    {
        c_ares.Public += "HAVE_CLOSESOCKET"_d;
        c_ares.Public += "HAVE_IOCTLSOCKET_FIONBIO"_d;
        c_ares += "USE_WINSOCK"_d;
        c_ares += "_WIN32_WINNT=0x0600"_d;
        c_ares += "HAVE_IPHLPAPI_H"_d;
        c_ares += "HAVE_NETIOAPI_H"_d;
        c_ares += "Iphlpapi.lib"_slib;
    }
    else
    {
        //c_ares += "CARES_USE_LIBRESOLV"_def;
    }

    //
    if (c_ares.Variables["HAVE_SOCKLEN_T"])
        c_ares.Variables["CARES_TYPEOF_ARES_SOCKLEN_T"] = "socklen_t";
    else
        c_ares.Variables["CARES_TYPEOF_ARES_SOCKLEN_T"] = "int";

    recv_args(c_ares);

    c_ares.Variables["CARES_HAVE_SYS_TYPES_H"] = c_ares.Variables["HAVE_SYS_TYPES_H"];
    c_ares.Variables["CARES_HAVE_SYS_SOCKET_H"] = c_ares.Variables["HAVE_SYS_SOCKET_H"];
    c_ares.Variables["CARES_HAVE_WINDOWS_H"] = c_ares.Variables["HAVE_WINDOWS_H"];
    c_ares.Variables["CARES_HAVE_WS2TCPIP_H"] = c_ares.Variables["HAVE_WS2TCPIP_H"];
    c_ares.Variables["CARES_HAVE_WINSOCK2_H"] = c_ares.Variables["HAVE_WINSOCK2_H"];

    if (c_ares.getBuildSettings().TargetOS.Type != OSType::Windows && c_ares.getBuildSettings().TargetOS.Type != OSType::Mingw || c_ares.getBuildSettings().TargetOS.is(ArchType::x86))
        c_ares.Variables["CARES_TYPEOF_ARES_SSIZE_T"] = "long";
    else
        c_ares.Variables["CARES_TYPEOF_ARES_SSIZE_T"] = "long long";

    if (c_ares.getBuildSettings().TargetOS.Type == OSType::Windows || c_ares.getBuildSettings().TargetOS.Type == OSType::Mingw)
        c_ares.Public += "ws2_32.lib"_slib, "Advapi32.lib"_slib;

    if (c_ares.getBuildSettings().TargetOS.Type == OSType::Windows || c_ares.getBuildSettings().TargetOS.Type == OSType::Mingw)
    {
        c_ares.Public.Definitions["MSG_NOSIGNAL"] = 0;
    }

    c_ares.Variables["CARES_SYMBOL_SCOPE_EXTERN"] = "SW_EXPORT";
    c_ares.Variables["CARES_THREADS"] = "1";
    c_ares.replaceInFileOnce("src/lib/ares_config.h.cmake",
        "#undef CARES_SYMBOL_SCOPE_EXTERN",
        "#cmakedefine CARES_SYMBOL_SCOPE_EXTERN @CARES_SYMBOL_SCOPE_EXTERN@");
    c_ares.configureFile("src/lib/ares_config.h.cmake", "ares_config.h");
    c_ares.configureFile("include/ares_build.h.cmake", "ares_build.h");
    c_ares.pushFrontToFileOnce("src/lib/ares__htable.c", "#include <time.h>");
}

void check(Checker &c)
{
    auto &s = c.addSet("c_ares");
    s.checkFunctionExists("bitncmp");
    s.checkFunctionExists("closesocket");
    s.checkFunctionExists("connect");
    s.checkFunctionExists("fcntl");
    s.checkFunctionExists("fork");
    //s.checkFunctionExists("getenv");
    s.checkFunctionExists("geteuid");
    s.checkFunctionExists("gethostbyname");
    s.checkFunctionExists("getifaddrs");
    s.checkFunctionExists("getpass_r");
    s.checkFunctionExists("getppid");
    s.checkFunctionExists("getprotobyname");
    s.checkFunctionExists("getpwuid");
    s.checkFunctionExists("getpwuid_r");
    s.checkFunctionExists("getrlimit");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("if_indextoname");
    s.checkFunctionExists("if_nametoindex");
    s.checkFunctionExists("inet_addr");
    s.checkFunctionExists("ldap_init_fd");
    s.checkFunctionExists("ldap_url_parse");
    s.checkFunctionExists("perror");
    s.checkFunctionExists("pipe");
    s.checkFunctionExists("pthread_create");
    s.checkFunctionExists("recv");
    s.checkFunctionExists("recvfrom");
    s.checkFunctionExists("send");
    s.checkFunctionExists("sendto");
    s.checkFunctionExists("setlocale");
    s.checkFunctionExists("setmode");
    s.checkFunctionExists("setrlimit");
    s.checkFunctionExists("strcasecmp");
    //s.checkFunctionExists("time").Parameters.Includes.push_back("time.h");
    //s.checkFunctionExists("time").Parameters.Includes.push_back("sys/time.h");
    s.checkFunctionExists("writev");
    s.checkFunctionExists("uname");
    s.checkFunctionExists("utime");
    s.checkIncludeExists("alloca.h");
    s.checkIncludeExists("arpa/inet.h");
    s.checkIncludeExists("arpa/nameser_compat.h");
    s.checkIncludeExists("arpa/nameser.h");
    s.checkIncludeExists("arpa/tftp.h");
    s.checkIncludeExists("assert.h");
    s.checkIncludeExists("crypto.h");
    s.checkIncludeExists("cyassl/error-ssl.h");
    s.checkIncludeExists("cyassl/options.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("err.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("gssapi/gssapi_generic.h");
    s.checkIncludeExists("gssapi/gssapi.h");
    s.checkIncludeExists("gssapi/gssapi_krb5.h");
    s.checkIncludeExists("idn2.h");
    s.checkIncludeExists("ifaddrs.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("libgen.h");
    s.checkIncludeExists("librtmp/rtmp.h");
    s.checkIncludeExists("libssh2.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("netdb.h");
    s.checkIncludeExists("netinet/in.h");
    s.checkIncludeExists("netinet/tcp.h");
    s.checkIncludeExists("net/if.h");
    s.checkIncludeExists("netioapi.h").Parameters.Includes.push_back("iphlpapi.h");
    s.checkIncludeExists("iphlpapi.h");
    s.checkIncludeExists("ws2ipdef.h");
    s.checkIncludeExists("nghttp2/nghttp2.h");
    s.checkIncludeExists("openssl/crypto.h");
    s.checkIncludeExists("openssl/engine.h");
    s.checkIncludeExists("openssl/err.h");
    s.checkIncludeExists("openssl/pem.h");
    s.checkIncludeExists("openssl/pkcs12.h");
    s.checkIncludeExists("openssl/rsa.h");
    s.checkIncludeExists("openssl/ssl.h");
    s.checkIncludeExists("openssl/x509.h");
    s.checkIncludeExists("pem.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("pwd.h");
    s.checkIncludeExists("rsa.h");
    s.checkIncludeExists("setjmp.h");
    s.checkIncludeExists("sgtty.h");
    s.checkIncludeExists("socket.h");
    s.checkIncludeExists("ssl.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("sys/filio.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/poll.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/sockio.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("sys/un.h");
    s.checkIncludeExists("sys/utime.h");
    s.checkIncludeExists("sys/wait.h");
    s.checkIncludeExists("termios.h");
    s.checkIncludeExists("termio.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("utime.h");
    s.checkIncludeExists("windows.h");
    s.checkIncludeExists("winsock2.h");
    s.checkIncludeExists("ws2tcpip.h");
    s.checkIncludeExists("x509.h");
    s.checkTypeSize("bool");
    s.checkTypeSize("int");
    s.checkTypeSize("long");
    s.checkTypeSize("long long");
    s.checkTypeSize("off_t");
    s.checkTypeSize("short");
    s.checkTypeSize("signal");
    s.checkTypeSize("sig_atomic_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("socket");
    s.checkTypeSize("socklen_t").Parameters.Includes.push_back("sys/socket.h");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("struct addrinfo");
    s.checkTypeSize("struct in6_addr");
    s.checkTypeSize("struct sockaddr_in6");
    s.checkTypeSize("struct sockaddr_storage");
    s.checkTypeSize("struct timeval");
    s.checkTypeSize("time_t");
    s.checkTypeSize("void *");
    s.checkLibraryFunctionExists("nsl", "gethostbyname");
    s.checkLibraryFunctionExists("pthread", "pthread_create");
    s.checkLibraryFunctionExists("resolve", "strcasecmp");
    s.checkSourceCompiles("HAVE_TIME_WITH_SYS_TIME", R"sw_xxx(
    #include <time.h>
    #include <sys/time.h>
    int main() {return 0;}
    )sw_xxx");

    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
#include <float.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
int main() {return 0;}
)sw_xxx");

    {
        auto add_headers = [](auto &c)
        {
            for (auto &h : { "stdbool.h",
                "sys/types.h",
                "sys/stat.h",
                "arpa/inet.h",
                "arpa/nameser.h",
                "netdb.h",
                "net/if.h",
                "netinet/in.h",
                "netinet/tcp.h",
                "signal.h",
                "stdio.h",
                "stdlib.h",
                "string.h",
                "strings.h",
                "sys/ioctl.h",
                "sys/select.h",
                "sys/socket.h",
                "sys/time.h",
                "sys/uio.h",
                "time.h",
                "fcntl.h",
                "unistd.h",
                "winsock2.h",
                "ws2tcpip.h",
                "windows.h" })
                c.Parameters.Includes.push_back(h);
        };

        auto &c = s.checkTypeSize("SOCKET", "HAVE_TYPE_SOCKET");
        add_headers(c);

        for (auto &t : {
            "socklen_t",
            "ssize_t",
            "bool",
            "sig_atomic_t",
            "long long",
            "struct addrinfo",
            "struct in6_addr",
            "struct sockaddr_in6",
            "struct sockaddr_storage",
            "struct timeval" })
        {
            auto &c = s.checkTypeSize(t);
            add_headers(c);
        }

        for (auto &se : {
            "AF_INET6",
            "O_NONBLOCK",
            "FIONBIO",
            "SIOCGIFADDR",
            "MSG_NOSIGNAL",
            "PF_INET6",
            "SO_NONBLOCK" })
        {
            auto &c = s.checkSymbolExists(se);
            add_headers(c);
        }

        for (auto &f : {
            "ioctl",
            //"ioctlsocket",
            "recv",
            "recvfrom",
            "send",
            "sendto",
            "socket" })
        {
            auto &c = s.checkFunctionExists(f);
            add_headers(c);
        }

        {
            auto &c = s.checkFunctionExists("CloseSocket", "HAVE_CLOSESOCKET_CAMEL");
            add_headers(c);
        }

        {
            auto &c = s.checkFunctionExists("IoctlSocket", "HAVE_IOCTLSOCKET_CAMEL");
            add_headers(c);
        }
    }
}
