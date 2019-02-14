void build(Solution &s)
{
    auto &p = s.addProject("gnome.glib", "2.58.3");
    p += RemoteFile("https://ftp.gnome.org/pub/gnome/sources/glib/{M}.{m}/glib-{v}.tar.xz");

    auto &glib = p.addTarget<LibraryTarget>("glib");
    {
        glib.ApiName = "SW_GLIB_API";

        glib.setChecks("glib", true);

        glib +=
            "build/win32/dirent/.*\\.c"_rr,
            "build/win32/dirent/dirent.h",
            "glib/[^/]*\\.c"_rr,
            "glib/[^/]*\\.h"_rr,
            "glib/deprecated/.*"_rr,
            "glib/glibconfig.h.in",
            "glib/gnulib/.*\\.c"_rr,
            "glib/gnulib/.*\\.h"_rr,
            "glib/libcharset/.*\\.c"_rr,
            "glib/libcharset/.*\\.h"_rr;

        glib -=
            "build/.*"_rr,
            "glib/gtester.c",
            "glib/win_iconv.c",
            "glib/gspawn.c",
            "glib/gstdio-private.c",
            "glib/.*helper.*"_rr
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

        glib.Private += sw::Shared, "DLL_EXPORT"_d;
        glib.Public += sw::Static, "GLIB_STATIC_COMPILATION"_d;

        glib.Public += "org.sw.demo.gnu.gettext.intl-0"_dep;
        glib.Public += "org.sw.demo.pcre.pcre8-8"_dep;

        glib.Variables["GLIB_MAJOR_VERSION"] = glib.Variables["PACKAGE_VERSION_MAJOR"];
        glib.Variables["GLIB_MINOR_VERSION"] = glib.Variables["PACKAGE_VERSION_MINOR"];
        glib.Variables["GLIB_MICRO_VERSION"] = glib.Variables["PACKAGE_VERSION_PATCH"];

        if (s.Settings.Native.LibrariesType == LibraryType::Static)
            glib.Variables["GLIB_WIN32_STATIC_COMPILATION_DEFINE"] = "#define GLIB_STATIC_COMPILATION 1";

        if (glib.Variables["WORDS_BIGENDIAN"])
        {
            glib.Variables["g_bs_native"] = "BE";
            glib.Variables["g_bs_alien"] = "LE";
            glib.Variables["g_byte_order"] = "G_BIG_ENDIAN";
        }
        else
        {
            glib.Variables["g_bs_native"] = "LE";
            glib.Variables["g_bs_alien"] = "BE";
            glib.Variables["g_byte_order"] = "G_LITTLE_ENDIAN";
        }

        glib.Variables["gint16"] = "short";
        glib.Variables["gint32"] = "int";
        glib.Variables["gint64"] = "long long";

        glib.Variables["glib_void_p"] = glib.Variables["SIZEOF_VOID_P"];
        glib.Variables["glib_long"] = glib.Variables["SIZEOF_LONG"];
        glib.Variables["glib_size_t"] = glib.Variables["SIZEOF_SIZE_T"];
        glib.Variables["glib_ssize_t"] = glib.Variables["SIZEOF_SSIZE_T"];

        glib.Variables["gint16_modifier"] = "\"h\"";
        glib.Variables["gint16_format"] = "\"hi\"";
        glib.Variables["guint16_format"] = "\"hu\"";

        glib.Variables["gint32_modifier"] = "\"\"";
        glib.Variables["gint32_format"] = "\"i\"";
        glib.Variables["guint32_format"] = "\"u\"";

        glib.Variables["gint64_modifier"] = "\"ll\"";
        glib.Variables["gint64_format"] = "\"lli\"";
        glib.Variables["guint64_format"] = "\"llu\"";

        glib.Variables["gint64_constant"] = "(val##L)";
        glib.Variables["guint64_constant"] = "(val##UL)";

        glib.Variables["g_pollin"] = "1";
        glib.Variables["g_pollout"] = "4";
        glib.Variables["g_pollpri"] = "2";
        glib.Variables["g_pollhup"] = "16";
        glib.Variables["g_pollerr"] = "8";
        glib.Variables["g_pollnval"] = "32";

        glib.Variables["g_pid_type"] = "int";
        glib.Variables["g_pid_format"] = "\"i\"";

        glib.Variables["g_af_unix"] = "1";
        glib.Variables["g_af_inet"] = "2";
        glib.Variables["g_af_inet6"] = "10";

        glib.Variables["g_msg_oob"] = "1";
        glib.Variables["g_msg_peek"] = "2";
        glib.Variables["g_msg_dontroute"] = "4";

        glib.Variables["glib_vacopy"] = R"(
#define G_VA_COPY va_copy
#define G_VA_COPY_AS_ARRAY 1
)";

        glib.Variables["g_have_iso_c_varargs"] = R"(
#ifndef __cplusplus
#define G_HAVE_ISO_VARARGS 1
#endif
)";

        glib.Variables["g_have_iso_cxx_varargs"] = R"(
#ifdef __cplusplus
#define G_HAVE_ISO_VARARGS 1
#endif
)";

        glib.Variables["gsize_modifier"] = "\"l\"";
        glib.Variables["gssize_modifier"] = "\"l\"";
        glib.Variables["gsize_format"] = "\"lu\"";
        glib.Variables["gssize_format"] = "\"li\"";
        glib.Variables["glib_msize_type"] = "LONG";

        glib.Variables["gintbits"] = "32";

        if (s.Settings.TargetOS.Type != OSType::Windows)
        {
            glib.Variables["glongbits"] = "64";
            glib.Variables["glib_size_type_define"] = "long";

            glib.Variables["gintptr_modifier"] = "G_GINT64_MODIFIER";
            glib.Variables["gintptr_format"] = "G_GINT64_FORMAT";
            glib.Variables["guintptr_format"] = "G_GINT64_CONSTANT(val)";

            glib.Variables["glib_os"] = "#define G_OS_UNIX";
            glib.Variables["g_threads_impl_def"] = "POSIX";
            glib.Public += "THREADS_POSIX"_d;
            glib += "HAVE_PTHREAD_COND_TIMEDWAIT_RELATIVE_NP"_d;

            glib.Variables["g_module_suffix"] = "so";
            glib.Variables["g_dir_separator"] = "/";
            glib.Variables["g_searchpath_separator"] = ":";

            glib -= "glib/g.*win\\d\\d.*"_r;

            glib.Variables["GLIB_HAVE_ALLOCA_H"] = 1;
            glib += "USE_LIBICONV_GNU"_d;
            glib += "_INTL_REDIRECT_MACROS"_d;
        }
        else
        {
            glib.Variables["glongbits"] = "32";
            if (s.Settings.TargetOS.Arch == ArchType::x86_64)
            {
                glib.Variables["glib_size_type_define"] = "long long";

                glib.Variables["gintptr_modifier"] = "G_GINT64_MODIFIER";
                glib.Variables["gintptr_format"] = "G_GINT64_FORMAT";
                glib.Variables["guintptr_format"] = "G_GINT64_CONSTANT(val)";
            }
            else
            {
                glib.Variables["glib_size_type_define"] = "long";

                glib.Variables["gintptr_modifier"] = "G_GINT32_MODIFIER";
                glib.Variables["gintptr_format"] = "G_GINT32_FORMAT";
                glib.Variables["guintptr_format"] = "G_GINT32_CONSTANT(val)";
            }

            glib.Variables["g_module_suffix"] = "dll";
            glib.Variables["g_dir_separator"] = "\\\\";
            glib.Variables["g_searchpath_separator"] = ";";

            glib.Variables["glib_os"] = "#define G_PLATFORM_WIN32";
            glib.Variables["g_threads_impl_def"] = "WIN32";
            glib.Public += "G_OS_WIN32"_d;
            glib.Public += "THREADS_WIN32"_d;
            glib.Public += "G_ATOMIC_LOCK_FREE"_d;

            glib -=
                "glib/giounix.c",
                "glib/gspawn.c",
                "glib/gthread-posix.c",
                "glib/glib-unix.c";
            glib += "Ole32.lib"_l, "Shell32.lib"_l, "User32.lib"_l, "ws2_32.lib"_l;
        }

        glib.configureFile("glib/glibconfig.h.in", "glibconfig.h");

        glib.replaceInFileOnce("glib/glib-init.c", "!strcasecmp", "!g_strcasecmp");
        glib.replaceInFileOnce("glib/glib-init.c", "#if defined (G_OS_WIN32)", R"(
#if defined  (G_OS_WIN32) && defined(GLIB_STATIC_COMPILATION)
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
#elif defined  (G_OS_WIN32)
        )");
        glib.replaceInFileOnce("glib/gthread-win32.c", "g_thread_impl_vtable.InitializeSRWLock (mutex);", R"(
#ifdef GLIB_STATIC_COMPILATION
    extern void glib_init_ctor(void);
    glib_init_ctor();
#endif
    g_thread_impl_vtable.InitializeSRWLock (mutex);
        )");
        glib.writeFileOnce(glib.BinaryPrivateDir / "config.h");
        //glib.writeFileOnce(glib.BinaryPrivateDir / "unistd.h");
        //glib.replaceInFileOnce("glib/gmessages.c", "myInvalidParameterHandler", "myInvalidParameterHandler_gmessages");
    }

    //
    auto &gobject = p.addTarget<LibraryTarget>("gobject");
    {
        gobject.setChecks("glib", true);
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

        gobject.Public += "org.sw.demo.weltling.winlibs.libffi-3"_dep;
        gobject.Public += glib;
        gobject.writeFileOnce(gobject.BinaryPrivateDir / "config.h");
    }

    /*auto &gmodule = p.addTarget<LibraryTarget>("gmodule");
    gmodule.setChecks("glib");
    gmodule +=
        "gmodule/[^/]*\\.c"_rr,
        "gmodule/[^/]*\\.h"_rr;

    gmodule -=
        "gmodule/gmodule-.*"_rr;

    gmodule.Public +=
        "gmodule"_id;

    gmodule.Public += glib;*/
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
