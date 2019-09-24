#pragma sw require pub.egorpugin.primitives.filesystem-master

void build(Solution &s)
{
    auto &p = s.addProject("gnome.glib", "2.61.2");
    p += RemoteFile("https://ftp.gnome.org/pub/gnome/sources/glib/{M}.{m}/glib-{v}.tar.xz");

    auto &glib = p.addTarget<LibraryTarget>("glib");
    {
        glib.ExportAllSymbols = true;
        glib.setChecks("glib", true);

        glib +=
            "glib/dirent/.*"_rr,
            "glib/[^/]*\\.c"_rr,
            "glib/[^/]*\\.h"_rr,
            "glib/deprecated/.*"_rr,
            "glib/glibconfig.h.in",
            "glib/gnulib/.*\\.c"_rr,
            "glib/gnulib/.*\\.h"_rr,
            "glib/libcharset/.*\\.c"_rr,
            "glib/libcharset/.*\\.h"_rr;

        glib -=
            "glib/gtester.c",
            "glib/win_iconv.c",
            "glib/gspawn.c",
            "glib/gnulib/isnan.c",
            "glib/gstdio-private.c",
            "glib/gwin32-private.c",
            "glib/.*helper.*"_rr
            ;

        if (glib.getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            glib -=
                "glib/dirent/.*"_rr;
            glib.Public +=
                "glib/gnulib"_id;
        }
        glib.Public +=
            "."_id,
            "glib"_id;

        glib ^= "glib/glib-object.h";

        glib.Private += "GLIB_COMPILATION"_d;
        glib.Public += "GETTEXT_PACKAGE=\"\""_d;
        glib.Public += "GLIB_BINARY_AGE=2"_d;
        glib.Public += "GLIB_INTERFACE_AGE=2"_d;
        glib.Public += "GLIB_LOCALE_DIR=\"\""_d;
        glib.Public += "USE_SYSTEM_PCRE"_d;
        glib.Public.Definitions["ALIGNOF_GUINT32"] += glib.Variables["ALIGNOF_UINT32_T"];
        glib.Public.Definitions["ALIGNOF_GUINT64"] += glib.Variables["ALIGNOF_UINT64_T"];

        glib.Private += sw::Shared, "DLL_EXPORT"_d;
        glib.Public += sw::Static, "GLIB_STATIC_COMPILATION"_d;

        glib.Public += "org.sw.demo.gnu.gettext.intl"_dep;
        glib.Public += "org.sw.demo.pcre.pcre8-8"_dep;

        glib.Variables["GLIB_MAJOR_VERSION"] = glib.Variables["PACKAGE_VERSION_MAJOR"];
        glib.Variables["GLIB_MINOR_VERSION"] = glib.Variables["PACKAGE_VERSION_MINOR"];
        glib.Variables["GLIB_MICRO_VERSION"] = glib.Variables["PACKAGE_VERSION_PATCH"];

        if (glib.getBuildSettings().Native.LibrariesType == LibraryType::Static)
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

        if (glib.getBuildSettings().TargetOS.Type != OSType::Windows)
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
            if (glib.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
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

            glib +=
                "Ole32.lib"_slib,
                "Shell32.lib"_slib,
                "User32.lib"_slib,
                "ws2_32.lib"_slib
                ;
        }

        glib.configureFile("glib/glibconfig.h.in", "glibconfig.h");
        glib.writeFileOnce(glib.BinaryPrivateDir / "config.h", R"(
#define _GL_INLINE_HEADER_BEGIN
#define _GL_INLINE_HEADER_END
#define _GL_INLINE inline
)");

        glib.replaceInFileOnce("glib/glib-init.c", "!strcasecmp", "!g_strcasecmp");

        // win+static
        glib.replaceInFileOnce("glib/glib-init.c", "#if defined (G_OS_WIN32)",
            "#if defined  (G_OS_WIN32) && !defined(GLIB_STATIC_COMPILATION)");
        glib.replaceInFileOnce("glib/glib-init.c", "G_DEFINE_CONSTRUCTOR(glib_init_ctor)",
            R"(G_DEFINE_CONSTRUCTOR (glib_init_ctor)

#if defined  (G_OS_WIN32)
HMODULE glib_dll;
#endif
)");
        glib.replaceInFileOnce("glib/glib-init.c", "glib_inited = TRUE;",
            R"(glib_inited = TRUE;

#if defined  (G_OS_WIN32)
    g_clock_win32_init();
    g_thread_win32_init();
#endif
)");

        // math
        {
            glib += "glib/gnulib/meson.build";

            if (!glib.DryRun)
            {
                auto f = read_file(glib["glib/gnulib/meson.build"].file);

                auto read_lines = [&f](const String &s)
                {
                    auto p = f.find(s);
                    if (p == f.npos)
                        throw SW_RUNTIME_ERROR("missing: " + s);
                    while (!isspace(f[p-1]))
                        p = f.find(s, p + s.size());
                    if (p == f.npos)
                        throw SW_RUNTIME_ERROR("missing: " + s);
                    p = f.find('[', p);
                    if (p == f.npos)
                        throw SW_RUNTIME_ERROR("no [");
                    auto e = f.find(']', ++p);
                    if (e == f.npos)
                        throw SW_RUNTIME_ERROR("no ]");
                    auto sub = f.substr(p, e - p);
                    return split_string(sub, "\r\n,'\t ");
                };

                for (auto &l : read_lines("unneeded_funcs"))
                    glib.Variables["GNULIB_" + l] = 0;
                for (auto &l : read_lines("needed_funcs"))
                    glib.Variables["GNULIB_" + l] = 1;
                for (auto &l : read_lines("decls_for_unused_funcs"))
                    glib.Variables["HAVE_DECL_" + l] = 0;
                //for (auto &l : read_lines("decls_for_used_funcs"))
            }

            glib.Variables["INCLUDE_NEXT_AS_FIRST_DIRECTIVE"] = "include";
            glib.Variables["NEXT_AS_FIRST_DIRECTIVE_MATH_H"] = "<math.h>";
            glib.Variables["PRAGMA_COLUMNS"] = "";
            glib.Variables["PRAGMA_SYSTEM_HEADER"] = "";

            glib.Variables["GUARD_PREFIX"] = "GL";

            glib.Variables["REPLACE_ITOLD"] = 0;
            glib.Variables["REPLACE_NAN"] = 0;
            glib.Variables["REPLACE_HUGE_VAL"] = 0;

            glib.Variables["REPLACE_FREXP"] = 0;
            glib.Variables["REPLACE_FREXPL"] = 0;
            glib.Variables["HAVE_DECL_FREXPL"] = 1;

            glib.Variables["REPLACE_LDEXPL"] = 0;
            glib.Variables["HAVE_DECL_LDEXPL"] = 1;

            glib.Variables["REPLACE_ISFINITE"] = 0;
            glib.Variables["REPLACE_ISINF"] = 0;
            if (glib.getBuildSettings().TargetOS.Type == OSType::Windows)
                glib.Variables["REPLACE_ISNAN"] = 0;
            else
                glib.Variables["REPLACE_ISNAN"] = 1;

            glib.Variables["REPLACE_SIGNBIT_USING_GCC"] = 0;
            glib.Variables["REPLACE_SIGNBIT"] = 0;

            glib.configureFile("glib/gnulib/gnulib_math.h.in", "gnulib_math.h");

            if (glib.getBuildSettings().TargetOS.Type != OSType::Windows)
            {
                glib -= "glib/gnulib/frexp.c";
                glib -= "glib/gnulib/frexpl.c";
                glib -= "glib/gnulib/isinf.c";
            }
        }
    }

    //
    auto &gobject = p.addTarget<LibraryTarget>("gobject");
    {
        gobject.ExportAllSymbols = true;
        gobject.setChecks("glib", true);

        gobject +=
            "gobject/[^/]*\\.in"_rr, // for glib-mkenums.in
            "gobject/[^/]*\\.c"_rr,
            "gobject/[^/]*\\.h"_rr;

        gobject.Public +=
            "."_id,
            "glib"_id,
            "gobject"_id
            ;

        gobject += "glib/glib-object.h";

        gobject.Private += "GOBJECT_COMPILATION"_d;
        gobject.Private += sw::Shared, "DLL_EXPORT"_d;
        gobject.Public += sw::Static, "GOBJECT_STATIC_COMPILATION"_d;

        gobject.Public += "org.sw.demo.weltling.winlibs.libffi"_dep;
        gobject.Public += glib;
        gobject.writeFileOnce(gobject.BinaryPrivateDir / "config.h");

        for (auto ext : {"h", "c"})
        {
            // glib.mkenums
            auto c = gobject.addCommand();
            c << cmd::prog("org.sw.demo.python.exe"_dep)
                << cmd::in("gobject/glib-mkenums.in")
                << "--template"
                << cmd::in("gobject/glib-enumtypes."s + ext + ".template")
                << "--output"
                << cmd::out("gobject/glib-enumtypes."s + ext)
                ;
            for (auto &[p, f] : glib["glib/gunicode.h"_rr])
            {
                if (!f->skip)
                    c << cmd::in(p);
            }
        }
    }

    auto &gmodule = p.addTarget<LibraryTarget>("gmodule");
    {
        gmodule.ExportAllSymbols = true;
        gmodule.setChecks("glib");

        gmodule += "gmodule/.*"_rr;
        gmodule -= "gmodule/gmodule-.*"_rr;

        gmodule.Public +=
            "."_id,
            "gmodule"_id;

        gmodule.Public += glib;

        if (gmodule.getBuildSettings().TargetOS.Type == OSType::Windows)
            gmodule.Variables["G_MODULE_IMPL"] = "G_MODULE_IMPL_WIN32";
        else if (gmodule.getBuildSettings().TargetOS.Type == OSType::Macos)
            gmodule.Variables["G_MODULE_IMPL"] = "G_MODULE_IMPL_DYLD";
        else
            gmodule.Variables["G_MODULE_IMPL"] = "G_MODULE_IMPL_DL";
        // G_MODULE_IMPL_AR

        gmodule.Variables["G_MODULE_HAVE_DLERROR"] = 1;
        gmodule.Variables["G_MODULE_NEED_USCORE"] = 0;
        gmodule.Variables["G_MODULE_BROKEN_RTLD_GLOBAL"] = 0;

        gmodule.configureFile("gmodule/gmoduleconf.h.in", "gmoduleconf.h");

        gmodule.writeFileOnce(gmodule.BinaryPrivateDir / "config.h");
    }

    auto &gio = p.addTarget<LibraryTarget>("gio");
    {
        gio.ExportAllSymbols = true;
        gio.PackageDefinitions = true;
        gio.setChecks("glib", true);

        gio -= "gio/.*"_rr;

        gio.Public +=
            "."_id,
            "gio"_id;
        gio += IncludeDirectory(gio.BinaryDir / "gio");

        gio += "gio/.*\\.c"_r;
        gio += "gio/.*\\.h"_r;
        gio += "GIO_COMPILATION"_def;

        gio -= "gio/gio-launch-desktop.c";
        gio -= "gio/gio-querymodules.c";
        gio -= "gio/glib-compile-resources.c";
        gio -= "gio/glib-compile-.*.c"_rr;
        gio -= "gio/.*tool.*"_rr;
        gio -= "gio/.*-win32.*"_rr;

        gio -= "gio/gdbusauthmechanism.h";
        gio -= "gio/gwin32registrykey.h";

        if (gio.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            gio -= "gio/gdesktopappinfo.c";

            gio -= "gio/.*portal.*\\.c"_rr;

            gio -= "gio/.*unix.*\\.c"_rr;
            gio -= "gio/.*networkmonitornetlink.*\\.c"_rr;
            gio -= "gio/.*networkmonitornm.*\\.c"_rr;

            gio += "gio/win32/.*"_rr;

            gio.Variables["WSPIAPI_INCLUDE"] = "#include <wspiapi.h>";

            gio += "ws2_32.lib"_slib;
            gio += "Iphlpapi.lib"_slib;
            gio += "shell32.lib"_slib;
            gio += "Dnsapi.lib"_slib;
            gio += "User32.lib"_slib;
            gio += "Shlwapi.lib"_slib;

            gio.writeFileOnce(gio.BinaryPrivateDir / "unistd.h");
            gio.writeFileOnce(gio.BinaryPrivateDir / "fnmatch.h", R"(#include <shlwapi.h>

inline int fnmatch (const char *__pattern, const char *__string,
			 int __flags)
{
    return PathMatchSpecA(__string, __pattern);
}
)");
            gio.writeFileOnce(gio.BinaryPrivateDir / "netinet/in.h");
            gio.writeFileOnce(gio.BinaryPrivateDir / "sys/time.h", R"(#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdint.h> // portable: uint64_t   MSVC: __int64

// also see https://git.postgresql.org/gitweb/?p=postgresql.git;a=blob;f=src/port/gettimeofday.c;h=75a91993b74414c0a1c13a2a09ce739cb8aa8a08;hb=HEAD

// MSVC defines this in winsock2.h!?
typedef struct timeval {
    long tv_sec;
    long tv_usec;
} timeval;

inline int gettimeofday(struct timeval * tp, struct timezone * tzp)
{
    // Note: some broken versions only have 8 trailing zero's, the correct epoch has 9 trailing zero's
    // This magic number is the number of 100 nanosecond intervals since January 1, 1601 (UTC)
    // until 00:00:00 January 1, 1970
    static const uint64_t EPOCH = ((uint64_t) 116444736000000000ULL);

    SYSTEMTIME  system_time;
    FILETIME    file_time;
    uint64_t    time;

    GetSystemTime( &system_time );
    SystemTimeToFileTime( &system_time, &file_time );
    time =  ((uint64_t)file_time.dwLowDateTime )      ;
    time += ((uint64_t)file_time.dwHighDateTime) << 32;

    tp->tv_sec  = (long) ((time - EPOCH) / 10000000L);
    tp->tv_usec = (long) (system_time.wMilliseconds * 1000);
    return 0;
}
)");
            gio.patch("gio/xdgmime/xdgmimecache.c", "#warning", "//# warning");
        }
        else
            gio -= "gio/.*win32.*"_rr;

        //gio.CompileOptions.push_back("/W0");

        gio.Public += gobject, gmodule;
        gio.Public += "org.sw.demo.madler.zlib"_dep;
        gio.Public += "org.sw.demo.tronkko.dirent-master"_dep;

        gio.writeFileOnce(gio.BinaryPrivateDir / "config.h");

        gio.configureFile("gio/gnetworking.h.in", "gio/gnetworking.h");

        for (auto ext : {"h", "c"})
        {
            // glib.mkenums
            auto c = gio.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << gio.getFile(gobject, "gobject/glib-mkenums.in")
                << "--template"
                << cmd::in("gio/gioenumtypes."s + ext + ".template")
                << "--output"
                << cmd::out("gio/gioenumtypes."s + ext)
                ;
            for (auto &[p, f] : gio[".*\\.h"_rr])
            {
                if (!f->skip)
                    c << cmd::in(p);
            }
            c.c->first_response_file_argument = 1;
        }

        gio.Variables["VERSION"] = gio.Variables["PACKAGE_VERSION"];
        gio.configureFile("gio/gdbus-2.0/codegen/config.py.in", gio.SourceDir / "gio/gdbus-2.0/codegen/config.py");

        gio.patch("gio/gdbus-2.0/codegen/codegen.py",
            "'#  include <gio/gunixfdlist.h>\\n'",
            "'#endif\\n'\n"
            "'#if 1\\n'\n"
            "'#   include <gio/gunixfdlist.h>\\n'");

        {
            auto c = gio.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << cmd::in("gio/gdbus-2.0/codegen/gdbus-codegen.in")
                << "--interface-prefix" << "org."
                << "--output-directory" << gio.BinaryDir
                << "--generate-c-code" << "gdbus-daemon-generated"
                << "--c-namespace" << "_G"
                << cmd::in("gio/dbus-daemon.xml")
                << cmd::end()
                << cmd::out("gdbus-daemon-generated.h")
                << cmd::out("gdbus-daemon-generated.c")
                ;
        }

        if (gio.getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            auto c = gio.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << cmd::in("gio/gdbus-2.0/codegen/gdbus-codegen.in")
                << "--interface-prefix" << "org.freedesktop.portal."
                << "--output-directory" << gio.BinaryDir
                << "--generate-c-code" << "xdp-dbus"
                << "--c-namespace" << "GXdp" <<

                "--annotate" << "org.freedesktop.portal.Documents.Add()" <<
                "org.gtk.GDBus.C.UnixFD" << "true" <<
                "--annotate" << "org.freedesktop.portal.Documents.AddNamed()" <<
                "org.gtk.GDBus.C.UnixFD" << "true" <<
                "--annotate" << "org.freedesktop.portal.Documents.AddFull()" <<
                "org.gtk.GDBus.C.UnixFD" << "true" <<
                "--annotate" << "org.freedesktop.portal.OpenURI.OpenFile()" <<
                "org.gtk.GDBus.C.UnixFD" << "true" <<
                "--annotate" << "org.freedesktop.portal.Trash.TrashFile()" <<
                "org.gtk.GDBus.C.UnixFD" << "true"

                << cmd::in("gio/org.freedesktop.portal.Documents.xml")
                << cmd::in("gio/org.freedesktop.portal.OpenURI.xml")
                << cmd::in("gio/org.freedesktop.portal.ProxyResolver.xml")
                << cmd::in("gio/org.freedesktop.portal.Trash.xml")
                << cmd::end()

                << cmd::out("xdp-dbus.h")
                << cmd::out("xdp-dbus.c")
                ;
        }

        gio += "XDG_PREFIX=_gio_xdg"_def;
        gio += "gio/xdgmime/.*"_rr;
        gio += "gio/gvdb/.*"_rr;

        gio.AllowEmptyRegexes = true;
        gio ^= "gio/.*__pycache__.*"_rr;
        gio.AllowEmptyRegexes = false;
    }

    auto &rc = p.addTarget<Executable>("compile_resources");
    {
        rc.PackageDefinitions = true;
        rc +=
            "gio/gvdb/gvdb-builder.c",
            "gio/gvdb/.*\\.h"_r,
            "gio/glib-compile-resources.c";
        rc += "GIO_COMPILATION"_def;
        rc += gio;
        rc.writeFileOnce(rc.BinaryPrivateDir / "config.h");

        {
            auto c = rc.addCommand();
            c << cmd::prog("org.sw.demo.python.exe-3"_dep)
                << cmd::in(rc.getFile(gio, "gio/data-to-c.py"))
                << cmd::in(rc.getFile(glib, "glib/gconstructor.h"))
                << "gconstructor_code"
                << cmd::out("gconstructor_as_data.h");
        }
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("glib");

    // new
    s.checkDeclarationExists("frexpl");
    s.checkDeclarationExists("ldexpl");
    s.checkFunctionExists("isnanf");
    s.checkFunctionExists("isnand");
    s.checkFunctionExists("isnanl");

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
    s.checkTypeSize("long long");
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
