void build(Solution &s)
{
    auto &uv = s.addTarget<LibraryTarget>("libuv", "1.51.0");
    uv += Git("https://github.com/libuv/libuv", "v{v}");

    uv.Private += sw::Shared, "BUILDING_UV_SHARED"_d;
    uv.Interface += sw::Shared, "USING_UV_SHARED"_d;

    uv += "include/.*"_rr;
    uv -= "src/.*"_rr;
    uv += "src/.*"_r;
    if (uv.getBuildSettings().TargetOS.Type == OSType::Windows || uv.getBuildSettings().TargetOS.Type == OSType::Mingw)
    {
        uv += "src/win/.*"_rr;
        uv += "_WIN32_WINNT=_WIN32_WINNT_WIN8"_def;
        uv.Public +=
            "ws2_32.lib"_slib,
            "user32.lib"_slib,
            "Advapi32.lib"_slib,
            "iphlpapi.lib"_slib,
            "psapi.lib"_slib,
            "userenv.lib"_slib,
            "uuid.lib"_slib,
            "ole32.lib"_slib,
            "dbghelp.lib"_slib,
            "Shell32.lib"_slib
            ;
    }
    else
    {
        uv -= "src/win/.*"_rr;
        uv +=
            "src/unix/async.c",
            //"src/unix/atomic-ops.h",
            "src/unix/core.c",
            "src/unix/dl.c",
            "src/unix/fs.c",
            "src/unix/getaddrinfo.c",
            "src/unix/getnameinfo.c",
            "src/unix/internal.h",
            "src/unix/loop-watcher.c",
            "src/unix/loop.c",
            "src/unix/pipe.c",
            "src/unix/poll.c",
            "src/unix/process.c",
            "src/unix/random-devurandom.c",
            "src/unix/signal.c",
            //"src/unix/spinlock.h",
            "src/unix/stream.c",
            "src/unix/tcp.c",
            "src/unix/thread.c",
            "src/unix/tty.c",
            "src/unix/udp.c";

        switch (uv.getBuildSettings().TargetOS.Type)
        {
        case OSType::AIX:
            break;
        case OSType::Android:
            break;
        case OSType::Macos:
            uv +=
                "src/unix/darwin.c",
                "src/unix/darwin-proctitle.c",
                "src/unix/fsevents.c",
                "src/unix/kqueue.c",
                "src/unix/proctitle.c",
                "src/unix/random-getentropy.c";
            break;
        case OSType::FreeBSD:
            break;
        case OSType::NetBSD:
            break;
        case OSType::OpenBSD:
            break;
        case OSType::SunOS:
            break;
        case OSType::Linux:
            uv +=
                //"src/unix/epoll.c",
                //"src/unix/linux-core.c",
                //"src/unix/linux-inotify.c",
                //"src/unix/linux-syscalls.c",
                //"src/unix/linux-syscalls.h",
                "src/unix/linux.c",
                "src/unix/proctitle.c",
                "src/unix/random-devurandom.c",
                "src/unix/random-getrandom.c",
                "src/unix/random-sysctl-linux.c"
                ;
            uv += "_GNU_SOURCE"_def;
            uv += "_POSIX_C_SOURCE=200112"_def;
            break;
        }
        uv += "dl"_slib;
        uv += "pthread"_slib;
    }
}
