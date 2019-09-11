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

        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
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

        if (t.getBuildSettings().TargetOS.Type == OSType::Linux)
            t.Variables["HAVE_IOCTL_FIONBIO"] = 1;

        if (t.Variables["HAVE_IOCTLSOCKET"] && t.Variables["HAVE_FIONBIO"])
            t.Variables["HAVE_IOCTLSOCKET_FIONBIO"] = 1;

        if (t.Variables["HAVE_IOCTLSOCKET_CAMEL"] && t.Variables["HAVE_FIONBIO"])
            t.Variables["HAVE_IOCTLSOCKET_CAMEL_FIONBIO"] = 1;

        if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            t.Variables["HAVE_STRUCT_TIMEVAL"] = 1;
            t.Variables["HAVE_BOOL_T"] = 1;
            t.Variables["HAVE_SEND"] = 1;
            t.Variables["HAVE_RECV"] = 1;
        }
    };

    auto &libcurl = s.addTarget<LibraryTarget>("badger.curl.libcurl", "7.66.0");
    libcurl += Git("https://github.com/curl/curl", "curl-{M}_{m}_{p}");

    libcurl.setChecks("libcurl");
    libcurl.setChecks("c_ares");

    libcurl +=
        "include/.*\\.h"_rr,
        "lib/.*\\.c"_rr,
        "lib/.*\\.h"_rr,
        "lib/curl_config.h.cmake";

    libcurl.Private +=
        "lib"_id;

    libcurl.Public +=
        "include"_id;

    libcurl.Private += "BUILDING_LIBCURL"_d;
    libcurl.Public += "CURL_HIDDEN_SYMBOLS"_d;
    libcurl.Public += sw::Static, "CURL_STATICLIB"_d;

    libcurl.Public += "HAVE_GSSAPI"_d;
    if (!libcurl.getBuildSettings().TargetOS.isApple())
        libcurl.Public += "HAVE_GSSGNU"_d;
    libcurl.Public += "HAVE_LIBSSH2_H"_d;
    libcurl.Public += "HAVE_LIBZ"_d;
    libcurl.Public += "HAVE_SOCKET"_d;
    libcurl.Public += "HAVE_ZLIB_H"_d;
    //libcurl.Public += "USE_ARES"_d;
    libcurl.Public += "USE_LIBSSH2"_d;
    libcurl.Public += "USE_NGHTTP2"_d;
    libcurl.Public += "USE_OPENSSL"_d;
    if (libcurl.getBuildSettings().TargetOS.Type != OSType::Windows)
    {
        libcurl.Private += "HAVE_CONFIG_H"_d;
    }

    //libcurl.Public += "org.sw.demo.c_ares-1"_dep;
    libcurl.Public += "org.sw.demo.gnu.gss-1"_dep;
    libcurl.Public += "org.sw.demo.nghttp2-1"_dep;
    libcurl.Public += "org.sw.demo.libssh2-1"_dep;
    libcurl.Public += "org.sw.demo.madler.zlib-1"_dep;
    libcurl.Public -= "org.sw.demo.openldap.ldap_r"_dep;

    if (libcurl.getBuildSettings().TargetOS.Type == OSType::Windows)
        libcurl.Public += "Wldap32.lib"_slib;
    else
        libcurl.Public += "org.sw.demo.openldap.ldap_r"_dep;

    libcurl.Variables["OPERATING_SYSTEM"] = "${CMAKE_SYSTEM_NAME}";
    libcurl.Variables["OS"] = "\"${CMAKE_SYSTEM_NAME}\"";
    libcurl.Variables["HAVE_POSIX_STRERROR_R"] = "1";

    libcurl.Variables["CURL_SIZEOF_LONG"] = libcurl.Variables["SIZEOF_LONG"];

    if (libcurl.getBuildSettings().TargetOS.Type != OSType::Windows)
        libcurl.Definitions["SIZEOF_CURL_OFF_T"] = 8;

    if (libcurl.Variables["SIZEOF_LONG"] == 8)
    {
        libcurl.Variables["CURL_TYPEOF_CURL_OFF_T"] = "long";
        libcurl.Variables["CURL_SIZEOF_CURL_OFF_T"] = 8;
        libcurl.Definitions["CURL_SIZEOF_CURL_OFF_T"] = 8;
        libcurl.Definitions["SIZEOF_CURL_OFF_T"] = 8;
        libcurl.Variables["CURL_FORMAT_CURL_OFF_T"] = "ld";
        libcurl.Variables["CURL_FORMAT_CURL_OFF_TU"] = "lu";
        libcurl.Variables["CURL_FORMAT_OFF_T"] = "%ld";
        libcurl.Variables["CURL_SUFFIX_CURL_OFF_T"] = "L";
        libcurl.Variables["CURL_SUFFIX_CURL_OFF_TU"] = "UL";
    }

    if (libcurl.Variables["SIZEOF_LONG_LONG"] == 8)
    {
        libcurl.Variables["CURL_TYPEOF_CURL_OFF_T"] = "long long";
        libcurl.Variables["CURL_SIZEOF_CURL_OFF_T"] = 8;
        libcurl.Definitions["CURL_SIZEOF_CURL_OFF_T"] = 8;
        libcurl.Definitions["SIZEOF_CURL_OFF_T"] = 8;
        libcurl.Variables["CURL_FORMAT_CURL_OFF_T"] = "lld";
        libcurl.Variables["CURL_FORMAT_CURL_OFF_TU"] = "llu";
        libcurl.Variables["CURL_FORMAT_OFF_T"] = "%lld";
        libcurl.Variables["CURL_SUFFIX_CURL_OFF_T"] = "LL";
        libcurl.Variables["CURL_SUFFIX_CURL_OFF_TU"] = "ULL";
    }

    if (libcurl.Variables["CURL_TYPEOF_CURL_OFF_T"])
    {
        libcurl.Variables["CURL_TYPEOF_CURL_OFF_T"] = libcurl.Variables["ssize_t"];
        libcurl.Variables["CURL_SIZEOF_CURL_OFF_T"] = libcurl.Variables["SIZEOF_SSIZE_T"];
        libcurl.Variables["CURL_FORMAT_CURL_OFF_T"] = "ld";
        libcurl.Variables["CURL_FORMAT_CURL_OFF_TU"] = "lu";
        libcurl.Variables["CURL_FORMAT_OFF_T"] = "%ld";
        libcurl.Variables["CURL_SUFFIX_CURL_OFF_T"] = "L";
        libcurl.Variables["CURL_SUFFIX_CURL_OFF_TU"] = "UL";
    }

    if (libcurl.Variables["HAVE_LONG_LONG"])
    {
        libcurl.Variables["HAVE_LONGLONG"] = 1;
        libcurl.Variables["HAVE_LL"] = 1;
    }

    if (libcurl.Variables["HAVE_SOCKLEN_T"])
    {
        libcurl.Variables["CURL_TYPEOF_CURL_SOCKLEN_T"] = "socklen_t";
        libcurl.Variables["CURL_SIZEOF_CURL_SOCKLEN_T"] = libcurl.Variables["SIZEOF_SOCKLEN_T"];
    }
    else
    {
        libcurl.Variables["CURL_TYPEOF_CURL_SOCKLEN_T"] = "int";
        libcurl.Variables["CURL_SIZEOF_CURL_SOCKLEN_T"] = libcurl.Variables["SIZEOF_INT"];
    }

    for (auto &v : { "WS2TCPIP_H","SYS_TYPES_H","STDINT_H" ,"INTTYPES_H" ,"SYS_SOCKET_H" ,"SYS_POLL_H" })
    {
        libcurl.Variables["CURL_PULL_" + String(v)] = libcurl.Variables["HAVE_" + String(v)];
    }

    recv_args(libcurl);

    libcurl.Variables["CURL_EXTERN_SYMBOL"] = "SW_EXPORT";

    libcurl.configureFile("lib/curl_config.h.cmake", "curl_config.h");
}

void check(Checker &c)
{
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

    {
        auto &s = c.addSet("libcurl");
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
        s.checkFunctionExists("if_nametoindex");
        s.checkFunctionExists("inet_addr");
        s.checkFunctionExists("ldap_init_fd");
        s.checkFunctionExists("ldap_url_parse");
        s.checkFunctionExists("perror");
        s.checkFunctionExists("pipe");
        s.checkFunctionExists("pthread_create");
        s.checkFunctionExists("select");
        s.checkFunctionExists("setlocale");
        s.checkFunctionExists("setmode");
        s.checkFunctionExists("setrlimit");
        s.checkFunctionExists("uname");
        s.checkFunctionExists("utime");
        s.checkIncludeExists("alloca.h");
        s.checkIncludeExists("arpa/inet.h");
        s.checkIncludeExists("arpa/tftp.h");
        s.checkIncludeExists("assert.h");
        s.checkIncludeExists("crypto.h");
        s.checkIncludeExists("cyassl/error-ssl.h");
        s.checkIncludeExists("cyassl/options.h");
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
        s.checkIncludeExists("ws2tcpip.h");
        s.checkIncludeExists("x509.h");
        s.checkTypeSize("bool");
        s.checkTypeSize("int");
        s.checkTypeSize("long");
        s.checkTypeSize("long long");
        s.checkTypeSize("off_t");
        s.checkTypeSize("short");
        s.checkTypeSize("signal");
        s.checkTypeSize("size_t");
        s.checkTypeSize("socklen_t");
        s.checkTypeSize("ssize_t");
        s.checkTypeSize("time_t");
        s.checkTypeSize("void *");
        s.checkLibraryFunctionExists("pthread", "pthread_create");
        s.checkSourceCompiles("HAVE_TIME_WITH_SYS_TIME", R"sw_xxx(
#include <sys/time.h>
#include <time.h>
int main() {return 0;}
)sw_xxx");
        s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
#include <float.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
int main() {return 0;}
)sw_xxx");
    }
}
