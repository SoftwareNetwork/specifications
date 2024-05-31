// no working atm, requires gnulib

void build(Solution &s)
{
    auto &sasl = s.addTarget<LibraryTarget>("gnu.sasl", "2.2.1");
    sasl += Git("https://gitlab.com/gsasl/gsasl");

    sasl += c99;
    sasl += "_GSASL_API"_api;
    //sasl.setChecks("c_ares");

    //if (sasl.getBuildSettings().TargetOS.Type != OSType::Windows && sasl.getBuildSettings().TargetOS.Type != OSType::Mingw)
        //sasl.ExportAllSymbols = true;

    sasl += "lib/.*"_rr;
    if (sasl.getBuildSettings().TargetOS.Type == OSType::Windows) {
        sasl += "lib/win32/include"_idir;
    }
    sasl.Public += "lib/src"_idir;
    sasl.Public += "lib/gl/lib"_idir;

    sasl += "_GL_UNUSED="_def;

    sasl.Variables["VERSION"] = sasl.Variables["PACKAGE_VERSION"];
    sasl.Variables["MAJOR_VERSION"] = sasl.Variables["PACKAGE_VERSION_MAJOR"];
    sasl.Variables["MINOR_VERSION"] = sasl.Variables["PACKAGE_VERSION_MINOR"];
    sasl.Variables["PATCH_VERSION"] = sasl.Variables["PACKAGE_VERSION_PATCH"];
    sasl.Variables["NUMBER_VERSION"] = sasl.Variables["PACKAGE_VERSION_NUM"];
    sasl.configureFile("lib/src/gsasl-version.h.in", "gsasl-version.h");

    sasl += "HAVE_LIBGSS"_def;
    sasl += "org.sw.demo.gnu.gss"_dep;
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
    s.checkFunctionExists("pthread_create").Parameters.Includes.push_back("pthread.h");
    s.checkFunctionExists("recv");
    s.checkFunctionExists("recvfrom");
    s.checkFunctionExists("send");
    s.checkFunctionExists("sendto");
    s.checkFunctionExists("setlocale");
    s.checkFunctionExists("setmode");
    s.checkFunctionExists("setrlimit");
    s.checkFunctionExists("strcasecmp").Parameters.Includes.push_back("strings.h");
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
    s.checkIncludeExists("netinet6/in6.h");
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
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("pthread_np.h");
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
    s.checkIncludeExists("sys/epoll.h");
    s.checkIncludeExists("sys/event.h");
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
    s.checkIncludeExists("ws2tcpip.h").Parameters.Includes.push_back("windows.h");
    s.checkIncludeExists("ntdef.h").Parameters.Includes.push_back("windows.h");
    s.checkIncludeExists("ntstatus.h").Parameters.Includes.push_back("windows.h");
    s.checkIncludeExists("winternl.h").Parameters.Includes.push_back("windows.h");
    s.checkIncludeExists("mswsock.h").Parameters.Includes.push_back("windows.h");
    s.checkIncludeExists("winsock.h").Parameters.Includes.push_back("windows.h");
    s.checkIncludeExists("winsock2.h").Parameters.Includes.push_back("windows.h");
    s.checkIncludeExists("ws2tcpip.h").Parameters.Includes.push_back("windows.h");
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
    s.checkLibraryFunctionExists("pthread", "pthread_create").Parameters.Includes.push_back("pthread.h");
    s.checkLibraryFunctionExists("pthread", "pthread_init").Parameters.Includes.push_back("pthread.h");
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
