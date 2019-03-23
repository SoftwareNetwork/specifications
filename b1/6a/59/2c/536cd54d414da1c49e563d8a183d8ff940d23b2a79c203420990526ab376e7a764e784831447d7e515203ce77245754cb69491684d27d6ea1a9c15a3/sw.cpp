void build(Solution &s)
{
    auto &libevent = s.addTarget<LibraryTarget>("libevent", "master");
    libevent += Git("https://github.com/libevent/libevent", "", "{v}");

    libevent.setChecks("libevent");

    libevent +=
        "[^/]*\\.[hc]"_rr,
        "compat/.*"_rr,
        "evconfig-private.h.cmake",
        "event-config.h.cmake",
        "include/.*\\.h"_rr;

    libevent -=
        "arc4random.c";

    libevent.Private +=
        "compat"_id,
        "."_id;

    libevent.Public +=
        "include"_id;

    libevent.Interface += sw::Shared, "EVENT_VISIBILITY_WANT_DLLIMPORT"_d;
    libevent.Private += sw::Shared, "event_shared_EXPORTS"_d;

    libevent.Public += "org.sw.demo.openssl.ssl-1"_dep;

    libevent.Variables["EVENT_VERSION_MAJOR"] = 2;
    libevent.Variables["EVENT_VERSION_MINOR"] = 1;
    libevent.Variables["EVENT_VERSION_PATCH"] = 9;

    libevent.Variables["EVENT_NUMERIC_VERSION"] =
        "0x" + libevent.Variables["EVENT_VERSION_MAJOR"].toString() +
        "0" + libevent.Variables["EVENT_VERSION_MINOR"].toString() +
        "0" + libevent.Variables["EVENT_VERSION_PATCH"].toString() +
        "00";
    libevent.Variables["EVENT_PACKAGE_VERSION"] = libevent.Variables["PACKAGE_VERSION"];
    libevent.Variables["EVENT_VERSION"] = libevent.Variables["PACKAGE_VERSION"];

    libevent.Variables["EVENT__inline"] = "inline";
    libevent.Variables["EVENT__size_t"] = "unsigned";
    libevent.Variables["EVENT__socklen_t"] = "unsigned int";
    libevent.Variables["EVENT__pid_t"] = "int";
    libevent.Variables["EVENT__ssize_t"] = "int";

    libevent.Public += "EVENT__HAVE_STRUCT_ADDRINFO"_d;
    libevent.Variables["EVENT__HAVE_STRUCT_SOCKADDR_STORAGE"] = 1;
    libevent.Variables["EVENT__HAVE_STRUCT_IN6_ADDR"] = 1;
    libevent.Variables["EVENT__HAVE_STRUCT_SOCKADDR_IN6"] = 1;

    libevent.configureFile("evconfig-private.h.cmake", "evconfig-private.h");
    libevent.configureFile("event-config.h.cmake", "event2/event-config.h");

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        libevent -= "evthread_pthread.c";
        libevent -= "epoll_sub.c";
        libevent += "Shell32.lib"_slib;
    }
    else
    {
        libevent -= "evthread_win32.c";
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("libevent");
    s.checkFunctionExists("accept4");
    s.checkFunctionExists("arc4random");
    s.checkFunctionExists("arc4random_addrandom");
    s.checkFunctionExists("arc4random_buf");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("epoll_create1");
    s.checkFunctionExists("epoll_ctl");
    s.checkFunctionExists("eventfd");
    s.checkFunctionExists("fcntl");
    s.checkFunctionExists("getegid");
    s.checkFunctionExists("geteuid");
    s.checkFunctionExists("gethostbyname_r");
    s.checkFunctionExists("getifaddrs");
    s.checkFunctionExists("getnameinfo");
    s.checkFunctionExists("getprotobynumber");
    s.checkFunctionExists("getservbyname");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("inet_ntop");
    s.checkFunctionExists("inet_pton");
    s.checkFunctionExists("issetugid");
    s.checkFunctionExists("kqueue");
    s.checkFunctionExists("mach_absolute_time");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("nanosleep");
    s.checkFunctionExists("pipe");
    s.checkFunctionExists("pipe2");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("port_create");
    s.checkFunctionExists("putenv");
    s.checkFunctionExists("select");
    s.checkFunctionExists("sendfile");
    s.checkFunctionExists("setenv");
    s.checkFunctionExists("setrlimit");
    s.checkFunctionExists("sigaction");
    s.checkFunctionExists("signal");
    s.checkFunctionExists("splice");
    s.checkFunctionExists("strlcpy");
    s.checkFunctionExists("strsep");
    s.checkFunctionExists("strtok_r");
    s.checkFunctionExists("strtoll");
    s.checkFunctionExists("sysctl");
    s.checkFunctionExists("timerfd_create");
    s.checkFunctionExists("umask");
    s.checkFunctionExists("unsetenv");
    s.checkFunctionExists("usleep");
    s.checkFunctionExists("vasprintf");
    s.checkIncludeExists("arpa/inet.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("ifaddrs.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("mach/mach_time.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("netdb.h");
    s.checkIncludeExists("netinet/in6.h");
    s.checkIncludeExists("netinet/in.h");
    s.checkIncludeExists("netinet/tcp.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("port.h");
    s.checkIncludeExists("stdarg.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/devpoll.h");
    s.checkIncludeExists("sys/epoll.h");
    s.checkIncludeExists("sys/eventfd.h");
    s.checkIncludeExists("sys/event.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/queue.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/sendfile.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/sysctl.h");
    s.checkIncludeExists("sys/timerfd.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("sys/wait.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("zlib.h");
    s.checkTypeSize("addrinfo");
    s.checkTypeSize("fd_mask");
    s.checkTypeSize("in6_addr");
    s.checkTypeSize("int");
    s.checkTypeSize("linger");
    s.checkTypeSize("long");
    s.checkTypeSize("long long");
    s.checkTypeSize("off_t");
    s.checkTypeSize("pid_t");
    s.checkTypeSize("pthread_t");
    s.checkTypeSize("sa_family_t");
    s.checkTypeSize("short");
    s.checkTypeSize("size_t");
    s.checkTypeSize("sockaddr_in6");
    s.checkTypeSize("sockaddr_storage");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("struct");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("uint8_t");
    s.checkTypeSize("uintptr_t");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkStructMemberExists("struct in6_addr", "s6_addr16");
    }
    {
        auto &c = s.checkStructMemberExists("struct in6_addr", "s6_addr32");
    }
    {
        auto &c = s.checkStructMemberExists("struct sockaddr_in6", "sin6_len");
    }
    {
        auto &c = s.checkStructMemberExists("struct sockaddr_in", "sin_len");
    }
    {
        auto &c = s.checkStructMemberExists("struct sockaddr_storage", "ss_family");
    }
    {
        auto &c = s.checkStructMemberExists("struct sockaddr_storage", "__ss_family");
    }
    s.checkSourceCompiles("HAVE_TIME_WITH_SYS_TIME", R"sw_xxx(
    #include <time.h>
    #include <sys/time.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkDeclarationExists("CTL_KERN");
    s.checkDeclarationExists("KERN_ARND");
    s.checkDeclarationExists("KERN_RANDOM");
    s.checkDeclarationExists("RANDOM_UUID");

    for (auto &check : s.all)
        check->Prefixes.insert("EVENT__");
}
