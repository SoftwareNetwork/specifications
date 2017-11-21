void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &p = s.addProject("gnome.glib", "2.54.2");
    p.Source = Git("git://git.gnome.org/glib", "{v}");

    auto &glib = p.addTarget<LibraryTarget>("glib");
    glib.ApiName = "SW_GLIB_API";

    glib.setChecks("glib");

    glib +=
        "build/win32/dirent/.*\\.c"_rr,
        "build/win32/dirent/dirent.h",
        "glib/[^/]*\\.c"_rr,
        "glib/[^/]*\\.h"_rr,
        "glib/deprecated/.*"_rr,
        "glib/glibconfig.h.win32.in",
        "glib/gnulib/.*\\.c"_rr,
        "glib/gnulib/.*\\.h"_rr,
        "glib/libcharset/.*\\.c"_rr,
        "glib/libcharset/.*\\.h"_rr;

    glib -=
        "build/.*"_rr,
        "glib/gtester.c",
        "glib/win_iconv.c",
        "glib/gspawn.c",
        "glib/gspawn-win32-helper.c"
        ;

    glib.Public +=
        "."_id,
        "glib"_id;

    glib.Private += "GLIB_COMPILATION"_d;
    glib.Public += "GETTEXT_PACKAGE=\"\""_d;
    glib.Public += "GLIB_BINARY_AGE=2"_d;
    glib.Public += "GLIB_INTERFACE_AGE=2"_d;
    glib.Public += "GLIB_LOCALE_DIR=\"\""_d;
    glib.Public += "USE_SYSTEM_PCRE"_d;
    glib.Public += "_GLIB_EXTERN=SW_GLIB_API"_d;
    glib.Public.Definitions["ALIGNOF_GUINT32"] += glib.Variables["ALIGNOF_UINT32_T"];
    glib.Public.Definitions["ALIGNOF_GUINT64"] += glib.Variables["ALIGNOF_UINT64_T"];

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        glib.Public += "THREADS_WIN32"_d;
    }
    glib.Private += sw::Shared, "DLL_EXPORT"_d;
    glib.Public += sw::Static, "GLIB_STATIC_COMPILATION"_d;

    glib.Public += "pub.cppan2.demo.gnu.gettext.intl-0"_dep;
    glib.Public += "pub.cppan2.demo.pcre.pcre8-8"_dep;

    glib.Variables["GLIB_MAJOR_VERSION"] = glib.Variables["PACKAGE_VERSION_MAJOR"];
    glib.Variables["GLIB_MINOR_VERSION"] = glib.Variables["PACKAGE_VERSION_MINOR"];
    glib.Variables["GLIB_MICRO_VERSION"] = glib.Variables["PACKAGE_VERSION_PATCH"];

    if (s.Settings.Native.LibrariesType == LibraryType::Static)
        glib.Variables["GLIB_WIN32_STATIC_COMPILATION_DEFINE"] = "#define GLIB_STATIC_COMPILATION 1";

    glib.configureFile("glib/glibconfig.h.win32.in", "glibconfig.h");

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        glib -=
            "glib/giounix.c",
            "glib/gthread-posix.c",
            "glib/glib-unix.c";
        glib += "Ole32.lib"_l, "Shell32.lib"_l, "User32.lib"_l, "ws2_32.lib"_l;
    }
    else
    {
        glib -=
            "glib/giowin32.c";
    }

    glib.replaceInFileOnce("glib/glib-init.c", "strcasecmp", "g_strcasecmp");
    glib.replaceInFileOnce("glib/glib-init.c", "#if defined (G_OS_WIN32)", R"(
        #if defined (G_OS_WIN32) && defined(GLIB_STATIC_COMPILATION)
        HMODULE glib_dll;

        int is_glib_initialized = 0;

        void
        glib_init_ctor(void)
        {
            static int is_glib_initialized = 0;
            if (is_glib_initialized == 0)
            {
                glib_dll = 0;
                g_clock_win32_init();
                g_thread_win32_init();
                glib_init();
                is_glib_initialized = 1;
            }
        }
        #elif defined (G_OS_WIN32)
        )");
    glib.replaceInFileOnce("glib/gthread-win32.c", "g_thread_impl_vtable.InitializeSRWLock (mutex);", R"(
        #ifdef GLIB_STATIC_COMPILATION
          extern void glib_init_ctor(void);
          glib_init_ctor();
        #endif
          g_thread_impl_vtable.InitializeSRWLock (mutex);
        )");
    //glib.replaceInFileOnce("glib/gmessages.c", "myInvalidParameterHandler", "myInvalidParameterHandler_gmessages");


    //
    auto &gobject = p.addTarget<LibraryTarget>("gobject");
    gobject.setChecks("glib");
    gobject +=
        "gobject/[^/]*\\.c"_rr,
        "gobject/[^/]*\\.h"_rr;

    gobject -=
        "gobject/glib-genmarshal.c";

    gobject.Public +=
        "gobject"_id;

    gobject.Private += "GOBJECT_COMPILATION"_d;
    gobject.Private += sw::Shared, "DLL_EXPORT"_d;
    gobject.Public += sw::Static, "GOBJECT_STATIC_COMPILATION"_d;

    gobject.Public += "pub.cppan2.demo.weltling.winlibs.libffi-3"_dep;
    gobject.Public += glib;

    auto &gmodule = p.addTarget<LibraryTarget>("gmodule");
    gmodule.setChecks("glib");
    gmodule +=
        "gmodule/[^/]*\\.c"_rr,
        "gmodule/[^/]*\\.h"_rr;

    gmodule -=
        "gmodule/gmodule-.*"_rr;

    gmodule.Public +=
        "gmodule"_id;

    gmodule.Public += glib;
}

void check(Checker &c)
{
    auto &s = c.addSet("glib");
    s.checkFunctionExists("alloca");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("dlerror");
    s.checkFunctionExists("dlopen");
    s.checkFunctionExists("dlsym");
    s.checkFunctionExists("endmntent");
    s.checkFunctionExists("endservent");
    s.checkFunctionExists("fallocate");
    s.checkFunctionExists("fchmod");
    s.checkFunctionExists("fchown");
    s.checkFunctionExists("fdwalk");
    s.checkFunctionExists("fsync");
    s.checkFunctionExists("getc_unlocked");
    s.checkFunctionExists("getfsstat");
    s.checkFunctionExists("getgrgid_r");
    s.checkFunctionExists("getmntent_r");
    s.checkFunctionExists("getprotobyname_r");
    s.checkFunctionExists("getpwuid_r");
    s.checkFunctionExists("getresuid");
    s.checkFunctionExists("getvfsstat");
    s.checkFunctionExists("gmtime_r");
    s.checkFunctionExists("hasmntopt");
    s.checkFunctionExists("iconv_open");
    s.checkFunctionExists("if_indextoname");
    s.checkFunctionExists("if_nametoindex");
    s.checkFunctionExists("inotify_init1");
    s.checkFunctionExists("issetugid");
    s.checkFunctionExists("kevent");
    s.checkFunctionExists("kqueue");
    s.checkFunctionExists("lchmod");
    s.checkFunctionExists("lchown");
    s.checkFunctionExists("localtime_r");
    s.checkFunctionExists("lstat");
    s.checkFunctionExists("mbrtowc");
    s.checkFunctionExists("memalign");
    s.checkFunctionExists("memmem");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("newlocale");
    s.checkFunctionExists("NSLinkModule");
    s.checkFunctionExists("pipe2");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("posix_memalign");
    s.checkFunctionExists("printf_unix98");
    s.checkFunctionExists("prlimit");
    s.checkFunctionExists("readlink");
    s.checkFunctionExists("recvmmsg");
    s.checkFunctionExists("sendmmsg");
    s.checkFunctionExists("setenv");
    s.checkFunctionExists("setmntent");
    s.checkFunctionExists("snprintf_c99");
    s.checkFunctionExists("socket");
    s.checkFunctionExists("splice");
    s.checkFunctionExists("statfs");
    s.checkFunctionExists("statvfs");
    s.checkFunctionExists("stpcpy");
    s.checkFunctionExists("strcasecmp");
    s.checkFunctionExists("strerror_r");
    s.checkFunctionExists("strncasecmp");
    s.checkFunctionExists("strnlen");
    s.checkFunctionExists("strsignal");
    s.checkFunctionExists("strtod_l");
    s.checkFunctionExists("strtoll_l");
    s.checkFunctionExists("strtoull_l");
    s.checkFunctionExists("symlink");
    s.checkFunctionExists("sysctlbyname");
    s.checkFunctionExists("timegm");
    s.checkFunctionExists("unsetenv");
    s.checkFunctionExists("uselocale");
    s.checkFunctionExists("utimes");
    s.checkFunctionExists("valloc");
    s.checkFunctionExists("vasprintf");
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("vsnprintf_c99");
    s.checkFunctionExists("wcrtomb");
    s.checkFunctionExists("wcslen");
    s.checkFunctionExists("wcsnlen");
    s.checkFunctionExists("_NSGetEnviron");
    s.checkIncludeExists("attr/xattr.h");
    s.checkIncludeExists("crt_externs.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("fam.h");
    s.checkIncludeExists("fstab.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("linux/magic.h");
    s.checkIncludeExists("mach/mach_time.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("mntent.h");
    s.checkIncludeExists("linux/netlink.h");
    s.checkIncludeExists("sched.h");
    s.checkIncludeExists("selinux/selinux.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/event.h");
    s.checkIncludeExists("sys/filio.h");
    s.checkIncludeExists("sys/inotify.h");
    s.checkIncludeExists("sys/mntctl.h");
    s.checkIncludeExists("sys/mnttab.h");
    s.checkIncludeExists("sys/mount.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/statfs.h");
    s.checkIncludeExists("sys/statvfs.h");
    s.checkIncludeExists("sys/sysctl.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/vfstab.h");
    s.checkIncludeExists("sys/vfs.h");
    s.checkIncludeExists("sys/vmount.h");
    s.checkIncludeExists("sys/xattr.h");
    s.checkIncludeExists("termios.h");
    s.checkIncludeExists("values.h");
    s.checkIncludeExists("xlocale.h");
    s.checkTypeSize("char");
    s.checkTypeSize("int");
    s.checkTypeSize("intmax_t");
    s.checkTypeSize("ip_mreqn");
    s.checkTypeSize("long");
    s.checkTypeSize("long_long");
    s.checkTypeSize("ptrdiff_t");
    s.checkTypeSize("short");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("struct");
    s.checkTypeSize("unsigned");
    s.checkTypeSize("void *");
    s.checkTypeSize("__int64");
    s.checkTypeAlignment("uint32_t");
    s.checkTypeAlignment("uint64_t");
    s.checkTypeAlignment("unsigned long");
    s.checkLibraryFunctionExists("rt", "clock_gettime");
    s.checkLibraryFunctionExists("dl", "dlopen");
    s.checkLibraryFunctionExists("dl", "dlsym");
    s.checkLibraryFunctionExists("elf", "elf_begin");
    s.checkLibraryFunctionExists("elf", "elf_getshdrnum");
    s.checkLibraryFunctionExists("elf", "elf_getshdrstrndx");
    s.checkLibraryFunctionExists("fam", "FAMNoExists");
    s.checkLibraryFunctionExists("fam", "FAMOpen");
    s.checkLibraryFunctionExists("c", "getxattr");
    s.checkLibraryFunctionExists("selinux", "is_selinux_enabled");
    s.checkLibraryFunctionExists("selinux", "lgetfilecon_raw");
    s.checkLibraryFunctionExists("socket", "socket");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct dirent", "d_type");
        c.Parameters.Includes.push_back("dirent.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct statfs", "f_bavail");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct statfs", "f_fstypename");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct statvfs", "f_basetype");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct statvfs", "f_fstypename");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_atimensec");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_atim.tv_nsec");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_birthtim");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_birthtime");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_birthtimensec");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_birthtim.tv_nsec");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_blksize");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_blocks");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_ctimensec");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_ctim.tv_nsec");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_mtimensec");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_mtim.tv_nsec");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct tm", "tm_gmtoff");
        c.Parameters.Includes.push_back("time.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct tm", "__tm_gmtoff");
        c.Parameters.Includes.push_back("time.h");
    }
    s.checkSourceCompiles("MAJOR_IN_MKDEV", R"sw_xxx(
    #include <sys/mkdev.h>
    int main() { makedev(0, 0); return 0; }
    )sw_xxx");
    s.checkSourceCompiles("MAJOR_IN_SYSMACROS", R"sw_xxx(
    #include <sys/sysmacros.h>
    int main() { makedev(0, 0); return 0; }
    )sw_xxx");
}
