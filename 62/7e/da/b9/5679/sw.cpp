void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &c_ares = s.addTarget<LibraryTarget>("c_ares", "1.13.0");
    c_ares.Source = Git("https://github.com/c-ares/c-ares", "cares-{M}_{m}_{p}");

    c_ares.setChecks("c_ares");

    c_ares +=
        "[^/]*\\.c"_rr,
        "[^/]*\\.cmake"_rr,
        "[^/]*\\.h"_rr;

    c_ares -=
        "acountry.c",
        "adig.c",
        "ahost.c",
        "config-.*"_rr;

    c_ares.Private += "CARES_BUILDING_LIBRARY"_d;
    c_ares.Private += "HAVE_CONFIG_H"_d;
    c_ares.Public += "HAVE_RECV"_d;
    c_ares.Public += "HAVE_RECVFROM"_d;
    c_ares.Public += "HAVE_SEND"_d;
    c_ares.Public += "HAVE_SENDTO"_d;
    c_ares.Public += "HAVE_STRUCT_ADDRINFO"_d;
    c_ares.Public += "HAVE_STRUCT_SOCKADDR_IN6"_d;
    c_ares.Public += "HAVE_STRUCT_TIMEVAL"_d;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        c_ares.Public += "HAVE_CLOSESOCKET"_d;
        c_ares.Public += "HAVE_IOCTLSOCKET_FIONBIO"_d;
    }
    c_ares.Public += sw::Static, "CARES_STATICLIB"_d;
        
    //
    if (c_ares.Variables["HAVE_SOCKLEN_T"] != "0")
        c_ares.Variables["CARES_TYPEOF_ARES_SOCKLEN_T"] = "socklen_t";
    else
        c_ares.Variables["CARES_TYPEOF_ARES_SOCKLEN_T"] = "int";

    if (c_ares.Variables["HAVE_SSIZE_T"] != "0" && c_ares.Variables["HAVE_SOCKLEN_T"] != "0")
    {
        c_ares.Variables["RECVFROM_TYPE_RETV"] = "ssize_t";
        c_ares.Variables["RECVFROM_TYPE_ARG3"] = "size_t";
    }
    else
    {
        c_ares.Variables["RECVFROM_TYPE_RETV"] = "int";
        c_ares.Variables["RECVFROM_TYPE_ARG3"] = "int";
    }

    if (c_ares.Variables["HAVE_SOCKET"] != "0")
        c_ares.Variables["RECVFROM_TYPE_ARG1"] = "SOCKET";
    else
        c_ares.Variables["RECVFROM_TYPE_ARG1"] = "int";

    if (c_ares.Variables["HAVE_SOCKLEN_T"] != "0")
    {
        c_ares.Variables["RECVFROM_TYPE_ARG6"] = "socklen_t *";
        c_ares.Variables["GETNAMEINFO_TYPE_ARG2"] = "socklen_t";
        c_ares.Variables["GETNAMEINFO_TYPE_ARG46"] = "socklen_t";
    }
    else
    {
        c_ares.Variables["RECVFROM_TYPE_ARG6"] = "int *";
        c_ares.Variables["GETNAMEINFO_TYPE_ARG2"] = "int";
        c_ares.Variables["GETNAMEINFO_TYPE_ARG46"] = "int";
    }

    c_ares.Variables["RECV_TYPE_RETV"] = c_ares.Variables["RECVFROM_TYPE_RETV"];
    c_ares.Variables["SEND_TYPE_RETV"] = c_ares.Variables["RECVFROM_TYPE_RETV"];
    c_ares.Variables["RECV_TYPE_ARG1"] = c_ares.Variables["RECVFROM_TYPE_ARG1"];
    c_ares.Variables["SEND_TYPE_ARG1"] = c_ares.Variables["RECVFROM_TYPE_ARG1"];
    c_ares.Variables["RECV_TYPE_ARG3"] = c_ares.Variables["RECVFROM_TYPE_ARG3"];
    c_ares.Variables["SEND_TYPE_ARG3"] = c_ares.Variables["RECVFROM_TYPE_ARG3"];
    c_ares.Variables["GETHOSTNAME_TYPE_ARG2"] = c_ares.Variables["RECVFROM_TYPE_ARG3"];

    // These should always be "sane" values to use always
    c_ares.Variables["RECVFROM_QUAL_ARG5"] = "";
    c_ares.Variables["RECVFROM_TYPE_ARG2"] = "void *";
    c_ares.Variables["RECVFROM_TYPE_ARG4"] = "int";
    c_ares.Variables["RECVFROM_TYPE_ARG5"] = "struct sockaddr *";
    c_ares.Variables["RECV_TYPE_ARG2"] = "void *";
    c_ares.Variables["RECV_TYPE_ARG4"] = "int";
    c_ares.Variables["GETNAMEINFO_TYPE_ARG1"] = "struct sockaddr *";
    c_ares.Variables["GETNAMEINFO_TYPE_ARG7"] = "int";
    c_ares.Variables["SEND_TYPE_ARG2"] = "void *";
    c_ares.Variables["SEND_TYPE_ARG4"] = "int";

    c_ares.Variables["CARES_HAVE_SYS_TYPES_H"] = c_ares.Variables["HAVE_SYS_TYPES_H"];
    c_ares.Variables["CARES_HAVE_SYS_SOCKET_H"] = c_ares.Variables["HAVE_SYS_SOCKET_H"];
    c_ares.Variables["CARES_HAVE_WINDOWS_H"] = c_ares.Variables["HAVE_WINDOWS_H"];
    c_ares.Variables["CARES_HAVE_WS2TCPIP_H"] = c_ares.Variables["HAVE_WS2TCPIP_H"];
    c_ares.Variables["CARES_HAVE_WINSOCK2_H"] = c_ares.Variables["HAVE_WINSOCK2_H"];

    if (s.Settings.TargetOS.Type == OSType::Windows)
        c_ares.Public += "ws2_32.lib"_l, "Advapi32.lib"_l;

    if (c_ares.Variables["HAVE_FCNTL"] != "0" && c_ares.Variables["HAVE_O_NONBLOCK"] != "0")
        c_ares.Variables["HAVE_FCNTL_O_NONBLOCK"] = "1";

    if (c_ares.Variables["HAVE_IOCTL"] != "0" && c_ares.Variables["HAVE_FIONBIO"] != "0")
        c_ares.Variables["HAVE_IOCTL_FIONBIO"] = "1";

    if (c_ares.Variables["HAVE_IOCTLSOCKET"] != "0" && c_ares.Variables["HAVE_FIONBIO"] != "0")
        c_ares.Variables["HAVE_IOCTLSOCKET_FIONBIO"] = "1";

    if (c_ares.Variables["HAVE_IOCTLSOCKET_CAMEL"] != "0" && c_ares.Variables["HAVE_FIONBIO"] != "0")
        c_ares.Variables["HAVE_IOCTLSOCKET_CAMEL_FIONBIO"] = "1";

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        c_ares.Public.Definitions["MSG_NOSIGNAL"] = "0";
    }

    c_ares.configureFile("ares_build.h.cmake", "ares_build.h");
    c_ares.configureFile("ares_config.h.cmake", "ares_config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("c_ares");
    s.checkFunctionExists("bitncmp");
    s.checkFunctionExists("closesocket");
    s.checkFunctionExists("connect");
    s.checkFunctionExists("fcntl");
    s.checkFunctionExists("fork");
    s.checkFunctionExists("geteuid");
    s.checkFunctionExists("gethostbyname");
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
    s.checkTypeSize("socklen_t");
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
        for (auto &h : {"stdbool.h",
                        "sys/types.h",
                        "arpa/inet.h",
                        "arpa/nameser.h",
                        "netdb.h",
                        "net/if.h",
                        "netinet/in.h",
                        "netinet/tcp.h",
                        "signal.h",
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
                        "windows.h"})
        {
            for (auto &t : {
                     "SOCKET",
                     "socklen_t",
                     "ssize_t",
                     "bool",
                     "sig_atomic_t",
                     "long long",
                     "struct addrinfo",
                     "struct in6_addr",
                     "struct sockaddr_in6",
                     "struct sockaddr_storage",
                     "struct timeval"})
            {
                auto &c = s.checkTypeSize(t);
                c.Parameters.Includes.push_back(h);
            }

            for (auto &se : {
                     "AF_INET6",
                     "O_NONBLOCK",
                     "FIONBIO",
                     "SIOCGIFADDR",
                     "MSG_NOSIGNAL",
                     "PF_INET6",
                     "SO_NONBLOCK"})
            {
                auto &c = s.checkSymbolExists(se);
                c.Parameters.Includes.push_back(h);
            }

            for (auto &f : {
                "ioctl",
                "ioctlsocket",
                "recv",
                "recvfrom",
                "send",
                "sendto",
                "socket" })
            {
                auto &c = s.checkFunctionExists(f);
                c.Parameters.Includes.push_back(h);
            }

            {
                auto &c = s.checkFunctionExists("CloseSocket", "HAVE_CLOSESOCKET_CAMEL");
                c.Parameters.Includes.push_back(h);
            }

            {
                auto &c = s.checkFunctionExists("IoctlSocket", "HAVE_IOCTLSOCKET_CAMEL");
                c.Parameters.Includes.push_back(h);
            }
        }
    }
}
