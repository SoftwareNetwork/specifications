// glib does not work with py3.12 yet (uses removed distutils module)
#define PYTHON_DEP "org.sw.demo.python.exe-3.10"_dep

void build(Solution &s)
{
    auto &p = s.addProject("gnome.glib", "2.79.2");
    p += RemoteFile("https://ftp.gnome.org/pub/gnome/sources/glib/{M}.{m}/glib-{v}.tar.xz");

    auto visibility_macro = [](auto &&t) {
        auto n = t.getPackage().getPath().back();
        auto N = boost::to_upper_copy(n);
        auto c = t.addCommand();
        c << cmd::prog(PYTHON_DEP)
            << cmd::in("tools/gen-visibility-macros.py")
            << t.getPackage().getVersion().toString()
            << "visibility-macros"
            << N
            << cmd::out(n + "/" + n + "-visibility.h")
            ;


        //t.Private += sw::Shared, "DLL_EXPORT"_d;
        t += Definition(N + "_COMPILATION");
        t.Public += sw::Static, Definition(N + "_STATIC_COMPILATION");
    };

    auto &glib = p.addTarget<LibraryTarget>("glib");
    {
        glib.ExportAllSymbols = true;
        glib.setChecks("glib", true);
        visibility_macro(glib);

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
            "glib/gnulib/isnan.c",
            "glib/gstdio-private.c",
            "glib/gwin32-private.c",
            "glib/.*helper.*"_rr
            ;

        if (glib.getBuildSettings().TargetOS.Type != OSType::Windows &&
            glib.getBuildSettings().TargetOS.Type != OSType::Mingw)
        {
            glib -=
                "glib/dirent/.*"_rr;
            glib.Public +=
                "glib/gnulib"_id;
        }
        else
            glib -= "glib/gspawn.c";
        if (glib.getBuildSettings().TargetOS.Type == OSType::Mingw)
        {
            glib += "glib/gnulib"_id;
        }
        glib.Public +=
            "."_id,
            "glib"_id;

        glib ^= "glib/glib-object.h";

        glib += "STRERROR_R_CHAR_P"_def;
        glib.Public += "GETTEXT_PACKAGE=\"\""_d;
        glib.Public += "GLIB_BINARY_AGE=2"_d;
        glib.Public += "GLIB_INTERFACE_AGE=2"_d;
        glib.Public += "GLIB_LOCALE_DIR=\"\""_d;
        glib.Public += "USE_SYSTEM_PCRE"_d;
        glib.Protected += "_GNU_SOURCE"_def;
        glib.Public.Definitions["ALIGNOF_GUINT32"] += glib.Variables["ALIGNOF_UINT32_T"];
        glib.Public.Definitions["ALIGNOF_GUINT64"] += glib.Variables["ALIGNOF_UINT64_T"];

        glib.Public += "ENABLE_NLS"_def;
        glib.Public += "org.sw.demo.gnu.gettext.intl"_dep;
        glib.Public += "org.sw.demo.pcre.pcre8"_dep;

        {
            auto c = glib.addCommand();
            c << cmd::prog(PYTHON_DEP)
                << cmd::in("tools/gen-visibility-macros.py")
                << glib.getPackage().getVersion().toString()
                << "versions-macros"
                << cmd::in("glib/gversionmacros.h.in")
                << cmd::out("glib/gversionmacros.h")
                ;
        }

        glib.Variables["GLIB_MAJOR_VERSION"] = glib.Variables["PACKAGE_VERSION_MAJOR"];
        glib.Variables["GLIB_MINOR_VERSION"] = glib.Variables["PACKAGE_VERSION_MINOR"];
        glib.Variables["GLIB_MICRO_VERSION"] = glib.Variables["PACKAGE_VERSION_PATCH"];

        //if (glib.getBuildSettings().Native.LibrariesType == LibraryType::Static)
            //glib.Variables["GLIB_WIN32_STATIC_COMPILATION_DEFINE"] = "#define GLIB_STATIC_COMPILATION 1";

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
        glib.Variables["glib_ssize_t"] = glib.Variables["SIZEOF_SIZE_T"]; // SIZEOF_SSIZE_T is not always available

        glib.Variables["gint16_modifier"] = "\"h\"";
        glib.Variables["gint16_format"] = "\"hi\"";
        glib.Variables["guint16_format"] = "\"hu\"";

        glib.Variables["gint32_modifier"] = "\"\"";
        glib.Variables["gint32_format"] = "\"i\"";
        glib.Variables["guint32_format"] = "\"u\"";

        glib.Variables["gint64_modifier"] = "\"ll\"";
        glib.Variables["gint64_format"] = "\"lli\"";
        glib.Variables["guint64_format"] = "\"llu\"";

        glib.Variables["gint64_constant"] = "(val##LL)";
        glib.Variables["guint64_constant"] = "(val##ULL)";

        glib.Variables["g_pollin"] = "1";
        glib.Variables["g_pollout"] = "4";
        glib.Variables["g_pollpri"] = "2";
        glib.Variables["g_pollhup"] = "16";
        glib.Variables["g_pollerr"] = "8";
        glib.Variables["g_pollnval"] = "32";

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
        glib.Variables["glib_msize_type"] = "INT64";
        glib.Variables["gintbits"] = "32";

        if (glib.getBuildSettings().TargetOS.Type != OSType::Windows &&
            glib.getBuildSettings().TargetOS.Type != OSType::Mingw)
        {
            glib.Variables["g_pid_type"] = "int";
            glib.Variables["glongbits"] = "64";
            glib.Variables["glib_size_type_define"] = "long";
            glib.Variables["glib_intptr_type_define"] = "long";

            glib.Variables["gintptr_modifier"] = "G_GINT64_MODIFIER";
            glib.Variables["gintptr_format"] = "G_GINT64_FORMAT";
            glib.Variables["guintptr_format"] = "G_GUINT64_FORMAT";

            glib.Variables["glib_os"] = "#define G_OS_UNIX";
            glib.Variables["g_threads_impl_def"] = "POSIX";
            glib.Public += "THREADS_POSIX"_d;
            if (glib.getBuildSettings().TargetOS.Type == OSType::Macos)
                glib += "HAVE_PTHREAD_COND_TIMEDWAIT_RELATIVE_NP"_d;
            else
                glib += "HAVE_PTHREAD_CONDATTR_SETCLOCK"_d;

            glib.Variables["g_module_suffix"] = "so";
            glib.Variables["g_dir_separator"] = "/";
            glib.Variables["g_searchpath_separator"] = ":";

            glib -= "glib/g.*win\\d\\d.*"_r;

            glib.Variables["GLIB_HAVE_ALLOCA_H"] = 1;
            glib += "USE_LIBICONV_GNU"_d;
            glib += "_INTL_REDIRECT_MACROS"_d;

            glib +=
                "pthread"_slib
                ;
        }
        else // win32
        {
            if (glib.getBuildSettings().TargetOS.Arch == ArchType::x86) {
                glib.Variables["glib_msize_type"] = "INT32";
            }
            glib.Variables["g_pid_type"] = "void*";
            glib.CompileOptions.insert("/utf-8");
            glib.Variables["glongbits"] = "32";
            if (glib.getBuildSettings().TargetOS.Arch == ArchType::x86_64 || glib.getBuildSettings().TargetOS.Arch == ArchType::aarch64)
            {
                glib.Variables["glib_intptr_type_define"] = "long long";
                glib.Variables["glib_size_type_define"] = "long long";

                glib.Variables["gintptr_modifier"] = "G_GINT64_MODIFIER";
                glib.Variables["gintptr_format"] = "G_GINT64_FORMAT";
                glib.Variables["guintptr_format"] = "G_GUINT64_FORMAT";
            }
            else
            {
                glib.Variables["glib_intptr_type_define"] = "long";
                glib.Variables["glib_size_type_define"] = "long";

                glib.Variables["gintptr_modifier"] = "G_GINT32_MODIFIER";
                glib.Variables["gintptr_format"] = "G_GINT32_FORMAT";
                glib.Variables["guintptr_format"] = "G_GUINT32_FORMAT";
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

        glib.pushFrontToFileOnce("glib/gwakeup.c", R"xxx(#include <stdint.h>)xxx");

        glib.writeFileOnce(glib.BinaryPrivateDir / "config.h", R"(
#define _GL_INLINE_HEADER_BEGIN
#define _GL_INLINE_HEADER_END
#define _GL_INLINE inline

/* Please see the Gnulib manual for how to use these macros.

   Suppress extern inline with HP-UX cc, as it appears to be broken; see
   <https://lists.gnu.org/r/bug-texinfo/2013-02/msg00030.html>.

   Suppress extern inline with Sun C in standards-conformance mode, as it
   mishandles inline functions that call each other.  E.g., for 'inline void f
   (void) { } inline void g (void) { f (); }', c99 incorrectly complains
   'reference to static identifier "f" in extern inline function'.
   This bug was observed with Sun C 5.12 SunOS_i386 2011/11/16.

   Suppress extern inline (with or without __attribute__ ((__gnu_inline__)))
   on configurations that mistakenly use 'static inline' to implement
   functions or macros in standard C headers like <ctype.h>.  For example,
   if isdigit is mistakenly implemented via a static inline function,
   a program containing an extern inline function that calls isdigit
   may not work since the C standard prohibits extern inline functions
   from calling static functions.  This bug is known to occur on:

     OS X 10.8 and earlier; see:
     https://lists.gnu.org/r/bug-gnulib/2012-12/msg00023.html

     DragonFly; see
     http://muscles.dragonflybsd.org/bulk/bleeding-edge-potential/latest-per-pkg/ah-tty-0.3.12.log

     FreeBSD; see:
     https://lists.gnu.org/r/bug-gnulib/2014-07/msg00104.html

   OS X 10.9 has a macro __header_inline indicating the bug is fixed for C and
   for clang but remains for g++; see <https://trac.macports.org/ticket/41033>.
   Assume DragonFly and FreeBSD will be similar.  */
#if (((defined __APPLE__ && defined __MACH__) \
      || defined __DragonFly__ || defined __FreeBSD__) \
     && (defined __header_inline \
         ? (defined __cplusplus && defined __GNUC_STDC_INLINE__ \
            && ! defined __clang__) \
         : ((! defined _DONT_USE_CTYPE_INLINE_ \
             && (defined __GNUC__ || defined __cplusplus)) \
            || (defined _FORTIFY_SOURCE && 0 < _FORTIFY_SOURCE \
                && defined __GNUC__ && ! defined __cplusplus))))
# define _GL_EXTERN_INLINE_STDHEADER_BUG
#endif
#if ((__GNUC__ \
      ? defined __GNUC_STDC_INLINE__ && __GNUC_STDC_INLINE__ \
      : (199901L <= __STDC_VERSION__ \
         && !defined __HP_cc \
         && !defined __PGI \
         && !(defined __SUNPRO_C && __STDC__))) \
     && !defined _GL_EXTERN_INLINE_STDHEADER_BUG)
# define _GL_INLINE inline
# define _GL_EXTERN_INLINE extern inline
# define _GL_EXTERN_INLINE_IN_USE
#elif (2 < __GNUC__ + (7 <= __GNUC_MINOR__) && !defined __STRICT_ANSI__ \
       && !defined _GL_EXTERN_INLINE_STDHEADER_BUG)
# if defined __GNUC_GNU_INLINE__ && __GNUC_GNU_INLINE__
   /* __gnu_inline__ suppresses a GCC 4.2 diagnostic.  */
#  define _GL_INLINE extern inline __attribute__ ((__gnu_inline__)) __attribute__ ((always_inline))
# else
#  define _GL_INLINE extern inline
# endif
# define _GL_EXTERN_INLINE extern
# define _GL_EXTERN_INLINE_IN_USE
#else
# define _GL_INLINE static _GL_UNUSED
# define _GL_EXTERN_INLINE static _GL_UNUSED
#endif

/* In GCC 4.6 (inclusive) to 5.1 (exclusive),
   suppress bogus "no previous prototype for 'FOO'"
   and "no previous declaration for 'FOO'" diagnostics,
   when FOO is an inline function in the header; see
   <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=54113> and
   <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63877>.  */
#if __GNUC__ == 4 && 6 <= __GNUC_MINOR__
# if defined __GNUC_STDC_INLINE__ && __GNUC_STDC_INLINE__
#  define _GL_INLINE_HEADER_CONST_PRAGMA
# else
#  define _GL_INLINE_HEADER_CONST_PRAGMA \
     _Pragma ("GCC diagnostic ignored \"-Wsuggest-attribute=const\"")
# endif
# define _GL_INLINE_HEADER_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wmissing-prototypes\"") \
    _Pragma ("GCC diagnostic ignored \"-Wmissing-declarations\"") \
    _GL_INLINE_HEADER_CONST_PRAGMA
# define _GL_INLINE_HEADER_END \
    _Pragma ("GCC diagnostic pop")
#else
# define _GL_INLINE_HEADER_BEGIN
# define _GL_INLINE_HEADER_END
#endif

/* Define as a marker that can be attached to declarations that might not
    be used.  This helps to reduce warnings, such as from
    GCC -Wunused-parameter.  */
#if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
# define _GL_UNUSED __attribute__ ((__unused__))
#else
# define _GL_UNUSED
#endif
)");

        if (1/* && glib.getBuildSettings().TargetOS.Type != OSType::Macos*/) {
            glib.replaceInFileOnce("glib/glib-init.c", "!strcasecmp", "!g_strcasecmp");
            glib.replaceInFileOnce("glib/glib-init.c", "#include \"glib-init.h\"", R"(
                #include  "glib-init.h"
                #include "gstrfuncs.h"
            )");
        }

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
        visibility_macro(gobject);

        gobject +=
            "gobject/[^/]*\\.in"_rr, // for glib-mkenums.in
            "gobject/[^/]*\\.c"_rr,
            "gobject/[^/]*\\.h"_rr;
        gobject -= "gobject/gobject-query.c";

        gobject.Public +=
            "."_id,
            "glib"_id,
            "gobject"_id
            ;

        gobject += "glib/glib-object.h";

        gobject.Public += "org.sw.demo.libffi"_dep;
        gobject.Public += glib;
        gobject.writeFileOnce(gobject.BinaryPrivateDir / "config.h");

        for (auto ext : {"h", "c"})
        {
            // glib.mkenums
            auto c = gobject.addCommand();
            c << cmd::prog(PYTHON_DEP)
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

        // special library to init gobject/glib on windows with static linking
        /*auto &wi = gobject.addStaticLibrary("wininit");
        {
            wi.AutoDetectOptions = false;
            wi.WholeArchive = true;
            wi.writeFileOnce("gobject_init.cpp", R"(
extern "C" void gobject_init(void);
struct SW_GOBJECT_INITIALIZER { SW_GOBJECT_INITIALIZER() { gobject_init(); } };
static SW_GOBJECT_INITIALIZER ___________SW_GOBJECT_INITIALIZER;
)");
            wi += "gobject_init.cpp";
        }
        gobject -= wi;
        gobject.patch("gobject/gtype.c", "static void\ngobject_init", "void\ngobject_init");
        if (gobject.getBuildSettings().TargetOS.Type == OSType::Windows
            && gobject.getBuildSettings().Native.LibrariesType == LibraryType::Static
            && gobject.getCompilerType() == CompilerType::MSVC
            )
        {
            gobject += wi;
        }*/
    }

    auto &gmodule = p.addTarget<LibraryTarget>("gmodule");
    {
        gmodule.ExportAllSymbols = true;
        gmodule.setChecks("glib");
        visibility_macro(gmodule);

        gmodule += "gmodule/.*"_rr;
        gmodule -= "gmodule/gmodule-.*"_rr;
        gmodule -= "gmodule/tests/.*"_rr;

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

        if (gmodule.getBuildSettings().TargetOS.Type != OSType::Windows)
        {
            gmodule += "gmodule/gmodule-deprecated.c";
            gmodule += "dl"_slib;
        }

        gmodule.Variables["G_MODULE_HAVE_DLERROR"] = 1;
        gmodule.Variables["G_MODULE_NEED_USCORE"] = 0;
        gmodule.Variables["G_MODULE_BROKEN_RTLD_GLOBAL"] = 0;

        gmodule.configureFile("gmodule/gmoduleconf.h.in", "gmoduleconf.h");

        gmodule.writeFileOnce(gmodule.BinaryPrivateDir / "config.h");
    }

    auto &gio = p.addTarget<LibraryTarget>("gio");
    /*auto &gvdb = p.addTarget<StaticLibraryTarget>("subprojects.gvdb");
    {
        gvdb.setRootDirectory("subprojects/gvdb");
        gvdb += "gvdb/.*"_rr;
        gvdb.Public += gobject;
        (gvdb + gio)->IncludeDirectoriesOnly = true;
    }*/

    // gio
    {
        gio.ExportAllSymbols = true;
        gio.PackageDefinitions = true;
        gio.setChecks("glib", true);
        visibility_macro(gio);

        gio -= "gio/.*"_rr;

        gio.Public +=
            "."_id,
            "gio"_id;
        gio += IncludeDirectory(gio.BinaryDir / "gio");

        gio += "subprojects/gvdb/.*"_rr;
        gio += "subprojects/gvdb"_idir;

        gio += "gio/.*\\.[hcm]"_r;
        gio -= "gio/inotify/.*\\.[hc]"_r;
        gio += "GLIB_RUNSTATEDIR=\".\""_def;

        gio -= "gio/gio-querymodules.c";
        gio -= "gio/glib-compile-resources.c";
        gio -= "gio/glib-compile-.*.c"_rr;
        gio -= "gio/glocalfileinfo.*\\.h"_rr;
        gio -= "gio/.*tool.*"_rr;
        gio -= "gio/.*-win32.*"_rr;
        gio -= "gio/.*win32.*"_rr;

        gio -= "gio/gdbusauthmechanism.h";
        gio -= "gio/gwin32registrykey.h";

        //gio -= "com.Microsoft.Windows.SDK.winrt"_dep;
        if (gio.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            gio -= "gio/gdesktopappinfo.c";
            gio -= "gio/giowin32-private.c";
            gio -= "gio/gio-launch-desktop.c";

            gio -= "gio/.*portal.*\\.c"_rr;

            //gio -= "gio/.*unix.*\\.c"_rr;
            gio -= "gio/gunixvolume.c";
            gio -= "gio/gunixvolumemonitor.c";
            gio -= "gio/gunixinputstream.c";
            gio -= "gio/gunixoutputstream.c";
            gio -= "gio/gunixmount.c";
            gio -= "gio/gunixmounts.c";
            gio -= "gio/giounix-private.c";
            gio -= "gio/gunixfdlist.c";
            gio -= "gio/gunixfdmessage.c";
            gio += "gio/gmemorymonitorwin32.c";

            gio -= "gio/gsandbox.c";
            gio -= "gio/gsandbox.h";

            gio -= "gio/.*networkmonitornetlink.*\\.c"_rr;
            gio -= "gio/.*networkmonitornm.*\\.c"_rr;

            gio += "gio/win32/.*"_rr;
            gio += "gio/gwin32.*\\.c"_rr;

            gio.Variables["WSPIAPI_INCLUDE"] = "#include <wspiapi.h>";

            gio += "ws2_32.lib"_slib;
            gio += "Iphlpapi.lib"_slib;
            gio += "shell32.lib"_slib;
            gio += "Dnsapi.lib"_slib;
            gio += "User32.lib"_slib;
            gio += "Shlwapi.lib"_slib;
            gio += "ole32.lib"_slib;
            gio += "uuid.lib"_slib;

            gio += "com.Microsoft.Windows.SDK.winrt"_dep;

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
        {
            gio += "GLIB_LOCALSTATEDIR=\".\""_def;
            if (!glib.getBuildSettings().TargetOS.isApple())
            {
                gio += "gio/inotify/.*\\.[hc]"_r;
            }
            else
            {
                gio += "gio/kqueue/.*"_rr;
                gio -= "gio/gcontenttype.c";
                gio -= "gio/gdesktopappinfo.c";
                gio -= "gio/.*networkmonitornetlink.*\\.c"_rr;
                gio -= "gio/.*networkmonitornm.*\\.c"_rr;
                gio += "CoreFoundation"_framework;
                gio += "Cocoa"_framework;
                gio.Variables["NAMESER_COMPAT_INCLUDE"] = "#include <arpa/nameser_compat.h>";
            }
            gio -= "gio/.*win32.*"_rr;
            gio -= "gio/gregistrysettingsbackend.c";
            gio += "USE_STATFS"_def;
            gio += "GIO_MODULE_DIR=\".\""_def;
            gio += "LOCALSTATEDIR=\"/var\""_def;
            gio += "GIO_LAUNCH_DESKTOP=\"gio-launch-desktop\""_def;
            gio += "resolv"_slib;
        }

        //gio.CompileOptions.push_back("/W0");

        //gio.Protected += gvdb;
        gio.Public += gobject, gmodule;
        gio.Public += "org.sw.demo.madler.zlib"_dep;
        gio.Public -= "org.sw.demo.tronkko.dirent-master"_dep;
        if (gio.getBuildSettings().TargetOS.Type == OSType::Windows)
            gio.Public += "org.sw.demo.tronkko.dirent-master"_dep;

        gio.writeFileOnce(gio.BinaryPrivateDir / "config.h");
        gio.pushBackToFileOnce("gio/gio.h", "#include <gio/gsandbox.h>");

        gio.configureFile("gio/gnetworking.h.in", "gio/gnetworking.h");

        for (auto ext : {"h", "c"})
        {
            // glib.mkenums
            auto c = gio.addCommand();
            c << cmd::prog(PYTHON_DEP)
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
            c->first_response_file_argument = 1;
        }

        gio.Variables["VERSION"] = gio.Variables["PACKAGE_VERSION"];
        gio.Variables["MAJOR_VERSION"] = gio.Variables["PACKAGE_VERSION_MAJOR"];
        gio.Variables["MINOR_VERSION"] = gio.Variables["PACKAGE_VERSION_MINOR"];
        gio.configureFile("gio/gdbus-2.0/codegen/config.py.in", gio.SourceDir / "gio/gdbus-2.0/codegen/config.py");

        gio.patch("gio/gdbus-2.0/codegen/codegen.py",
            "'#  include <gio/gunixfdlist.h>\\n'",
            "'#endif\\n'\n"
            "'#if 1\\n'\n"
            "'#   include <gio/gunixfdlist.h>\\n'");
        gio.patch("gio/gio-launch-desktop.c", "G_STATIC_ASSERT (LOG_", "//G_STATIC_ASSERT  (LOG_");

        {
            auto c = gio.addCommand();
            c << cmd::prog(PYTHON_DEP)
                << cmd::dep("org.sw.demo.python.pypi.packaging"_dep)
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
            c << cmd::prog(PYTHON_DEP)
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

        gio.AllowEmptyRegexes = true;
        gio ^= "gio/.*__pycache__.*"_rr;
        gio.AllowEmptyRegexes = false;

        for (auto &&f : {"gio/xdgmime/xdgmime.c", "gio/xdgmime/xdgmimecache.c"})
        {
            gio.pushFrontToFileOnce(f, R"xxx(#define _CRT_INTERNAL_NONSTDC_NAMES 1
#include <sys/stat.h>
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
  #define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif
#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
  #define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif)xxx");
        }
        gio.patch("gio/xdgmime/xdgmime.c", "__attribute__((__unused__))", "");
        gio.patch("gio/xdgmime/xdgmimeint.c", "__attribute__((unused))", "");
    }

    auto &rc = p.addTarget<Executable>("compile_resources");
    {
        rc.PackageDefinitions = true;
        rc +=
            "gio/glib-compile-resources.c";
        rc += "GIO_COMPILATION"_def;
        rc += gio;
        rc += "subprojects/gvdb/.*"_rr;
        rc += "subprojects/gvdb"_idir;
        rc.writeFileOnce(rc.BinaryPrivateDir / "config.h");

        {
            auto c = rc.addCommand();
            c << cmd::prog(PYTHON_DEP)
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
    s.checkDeclarationExists("frexpl");
    s.checkDeclarationExists("ldexpl");
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
    s.checkFunctionExists("isnanf");
    s.checkFunctionExists("isnand");
    s.checkFunctionExists("isnanl");
    s.checkFunctionExists("issetugid");
    s.checkFunctionExists("kevent");
    s.checkFunctionExists("kqueue");
    s.checkFunctionExists("lchmod");
    s.checkFunctionExists("lchown");
    s.checkFunctionExists("localtime_r");
    s.checkFunctionExists("lstat");
    s.checkFunctionExists("mbrtowc");
    s.checkFunctionExists("memalign");
    s.checkFunctionExists("aligned_alloc");
    s.checkFunctionExists("_aligned_malloc");
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
    s.checkFunctionExists("statx");
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
    s.checkFunctionExists("utimensat");
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
