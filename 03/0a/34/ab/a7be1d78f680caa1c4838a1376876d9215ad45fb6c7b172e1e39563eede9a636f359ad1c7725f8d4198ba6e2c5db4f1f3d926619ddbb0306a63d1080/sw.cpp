void build(Solution &s)
{
    auto &t = s.addLibrary("libusb", "1.0.26");
    t += Git("https://github.com/libusb/libusb", "v{v}");
    {
        t.setChecks("usb", true);
        t += "libusb/.*"_r;
        t -= "libusb/os/.*"_rr;
        t ^= "libusb/libusb-1.0.rc";
        t -= "msvc/.*\\.h"_rr;

        if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            t += "msvc"_idir;
            t += "libusb/os/.*windows.*"_rr;
            t += "Advapi32.lib"_slib;
            t.patch("libusb/libusb-1.0.def", "LIBRARY \"libusb-1.0.dll\"", "LIBRARY");
        }
        else if (t.getBuildSettings().TargetOS.is(OSType::Mingw))
        {
            t.ApiName = "USB_API";
            t += "libusb/os/.*windows.*"_rr;
            t += "OS_WINDOWS"_def;
            t.writeFileOnce(t.BinaryPrivateDir / "config.h", R"(
/* Define to 1 to enable message logging. */
#define ENABLE_LOGGING 1

/* Define to 1 if using the Windows poll() implementation. */
#define POLL_WINDOWS 1

/* Define to 1 if using Windows threads. */
#define THREADS_WINDOWS 1

#define DEFAULT_VISIBILITY USB_API
#define POLL_NFDS_TYPE int
)");
        }
        else if (t.getBuildSettings().TargetOS.isApple())
        {
            t += "libusb/os/.*darwin.*"_rr;
            t += "libusb/os/.*posix.*"_rr;
        }
        else
        {
            t.ApiName = "USB_API";
            t += "libusb/os/.*linux_netl.*"_rr;
            t += "libusb/os/.*linux_usb.*"_rr;
            t += "libusb/os/.*posix.*"_rr;
            t += "_GNU_SOURCE"_def;
            t += "OS_LINUX"_def;
            t += "THREADS_POSIX"_def;
            t += "pthread"_slib;
            t.writeFileOnce(t.BinaryPrivateDir / "config.h", R"(
#ifdef HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#ifdef HAVE_SYS_UN_H
#include <sys/un.h>
#endif
#include <unistd.h>

#define DEFAULT_VISIBILITY USB_API
#define POLL_NFDS_TYPE nfds_t
)");
        }

        t.Public += "libusb"_idir;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("usb");
    s.checkIncludeExists("asm/types.h");
    s.checkIncludeExists("libudev.h");
    s.checkIncludeExists("asm/types.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("linux/netlink.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("syslog.h");
    s.checkIncludeExists("sys/timerfd.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/ucred.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/un.h");
    s.checkIncludeExists("unistd.h");

    s.checkLibraryFunctionExists("udev_new", "udev_new.udev");
    s.checkDeclarationExists("TFD_NONBLOCK");
    s.checkDeclarationExists("TFD_CLOEXEC");
    s.checkFunctionExists("pipe2");
    s.checkFunctionExists("syslog");
    s.checkFunctionExists("sigaction");
    s.checkTypeSize("struct timespec");
    s.checkTypeSize("nfds_t");
}
