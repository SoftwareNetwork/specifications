void build(Solution &s)
{
    auto &intl = s.addTarget<LibraryTarget>("gnu.gettext.intl", "0.22.5");
    intl += RemoteFile("https://ftp.gnu.org/pub/gnu/gettext/gettext-{M}.{m}{po}{to}.tar.gz");

    intl += c99;
    intl.setChecks("intl", true);

    intl +=
        "gettext-runtime/intl/.*\\.c"_rr,
        "gettext-runtime/intl/.*\\.h"_rr;

    intl -=
        "gettext-runtime/intl/intl-exports.c",
        "gettext-runtime/intl/os2compat.c",
        //"gettext-runtime/intl/printf.c",
        "gettext-runtime/intl/gnulib-lib/asnprintf.c",
        "gettext-runtime/intl/gnulib-lib/asnwprintf.c",
        "gettext-runtime/intl/gnulib-lib/mbsinit.c",
        //"gettext-runtime/intl/gnulib-lib/setlocale-lo.*"_rr,
        "gettext-runtime/intl/gnulib-lib/mbtowc.*"_rr,
        //"gettext-runtime/intl/gnulib-lib/tsearch.c",
        "gettext-runtime/intl/gnulib-lib/unistd.c",
        "gettext-runtime/intl/gnulib-lib/memchr.c",
        "gettext-runtime/intl/gnulib-lib/isnand.c",
        "gettext-runtime/intl/gnulib-lib/frexpl.c",
        "gettext-runtime/intl/gnulib-lib/frexp.c",
        "gettext-runtime/intl/gnulib-lib/wmemset.c",
        "gettext-runtime/intl/gnulib-lib/wmemcpy.c",
        //"gettext-runtime/intl/gnulib-lib/localename.c",
        "gettext-runtime/intl/gnulib-lib/hard-locale.c",
        "gettext-runtime/intl/gnulib-lib/relocatable.c",
        "gettext-runtime/intl/gnulib-lib/vasnprintf.c",
        "gettext-runtime/intl/gnulib-lib/vasnwprintf.c"
        ;

    //intl -= "gettext-runtime/intl/gnulib-lib/.*"_rr;
    intl += "gettext-runtime/intl/gnulib-lib"_idir;

    intl.Public += "org.sw.demo.gnu.iconv.libiconv"_dep;

    intl.Private += "IN_LIBINTL"_d;
    intl.Private += sw::Shared, "BUILDING_DLL"_d;
    if (intl.getBuildSettings().TargetOS.Type != OSType::Windows)
    {
        intl -= "gettext-runtime/intl/.*windows.*"_rr;
        intl.Public += "LOCALE_ALIAS_PATH=\"/etc/locale.alias\""_d;
    }
    intl.Private += "_GNU_SOURCE"_d;

    //
    intl += "HAVE_STDINT_H_WITH_UINTMAX=1"_d;
    if (intl.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        //intl += "HAVE_POSIX_PRINTF=0"_v;
        intl += "HAVE_POSIX_PRINTF=1"_v;
        intl.Public += "HAVE_POSIX_PRINTF=1"_d;
    }
    else
    {
        intl += "HAVE_VISIBILITY=1"_d;
        intl += "HAVE_POSIX_PRINTF=1"_v;
        intl += "HAVE_PTHREAD_API"_def;
        intl.Public += "HAVE_POSIX_PRINTF=1"_d;
    }
    if (intl.Variables.find("HAVE_NEWLOCALE") == intl.Variables.end())
        intl += "HAVE_NEWLOCALE=0"_v;
    if (intl.Variables.find("HAVE_ASPRINTF") == intl.Variables.end())
        intl += "HAVE_ASPRINTF=0"_v;
    //if (intl.Variables.find("HAVE_SNPRINTF") == intl.Variables.end())
    intl += "HAVE_SNPRINTF=0"_v;
    if (intl.Variables.find("HAVE_WPRINTF") == intl.Variables.end())
        intl += "HAVE_WPRINTF=0"_v;

    intl.Private += "HAVE_CONFIG_H"_d;
    intl.Private += "HAVE_GETCWD"_d;

    if (intl.getBuildSettings().TargetOS.Type == OSType::Windows)
        intl.Public += "Advapi32.lib"_slib;

    if (intl.Variables["HAVE_LANGINFO_H"] == 1 && intl.Variables["HAVE_NL_LANGINFO"] == 1)
        intl.Public.Definitions["HAVE_LANGINFO_CODESET"];

    intl.ApiName = "CPPAN_LIBINTL_DLL_EXPORT";
    intl.replaceInFileOnce("gettext-runtime/intl/libgnuintl.in.h", "extern \"", "myexternc");
    intl.replaceInFileOnce("gettext-runtime/intl/libgnuintl.in.h", "extern ", "extern " + intl.ApiName + " ");
    intl.replaceInFileOnce("gettext-runtime/intl/libgnuintl.in.h", "myexternc", "extern \"");
    intl.replaceInFileOnce("gettext-runtime/intl/libgnuintl.in.h", "IN_LIBGLOCALE", "__VERY_UNDEFINED_DEFINITION__");

    intl.pushFrontToFileOnce("gettext-runtime/intl/gnulib-lib/vasnprintf.c", "#include <wchar.h>");
    intl.pushFrontToFileOnce("gettext-runtime/intl/dcigettext.c", "#include <wchar.h>");

    intl.Variables["HAVE_NAMELESS_LOCALES"] = 0;
    intl.Variables["ENHANCE_LOCALE_FUNCS"] = 0;
    intl.configureFile("gettext-runtime/intl/libgnuintl.in.h", "libgnuintl.h");
    intl.configureFile("gettext-runtime/intl/libgnuintl.in.h", "libintl.h");
    intl.configureFile("gettext-runtime/intl/libgnuintl.in.h", "gettext.h");

    if (intl.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        intl.Variables["GNULIB_MDA_LFIND"] = 1;
        intl.Variables["GNULIB_MDA_LSEARCH"] = 1;
        intl.Variables["HAVE_TYPE_VISIT"] = 0;
        intl.Variables["GNULIB_TSEARCH"] = 1;
        intl.Variables["REPLACE_TSEARCH"] = 0;
        intl.Variables["GNULIB_TWALK"] = 1;
        intl.Variables["REPLACE_TWALK"] = 0;
        intl.configureFile("gettext-runtime/intl/gnulib-lib/search.in.h", "search.h");
    }

    intl.writeFileOnce(intl.BinaryPrivateDir / "config.h", R"xxx(
#pragma once

#include <stdbool.h>
#include <c++defs.h>

#define _GL_CONFIG_H_INCLUDED 1

#define FLEXIBLE_ARRAY_MEMBER 1 // means no support

#define HAVE_ICONV 1
#define HAVE_LC_MESSAGES 1
#define HAVE_PER_THREAD_LOCALE 1

#ifdef _WIN32
#define WIN32
#endif

#ifndef _WIN32
#define HAVE_PTHREAD_RWLOCK 1
#endif

#ifdef _WIN32
#define LC_MESSAGES -1
//#define LC_MESSAGES_COMPAT (-1)
#endif

#if defined(_MSC_VER)
//#define LOCALEDIR "."
#define LOCALE_ALIAS_PATH "."
#define tmp_dirname "%TEMP%"
#endif

#define _GL_ATTRIBUTE_PURE
#define _GL_ATTRIBUTE_FALLTHROUGH
#define _GL_ATTRIBUTE_CONST
#define _GL_ATTRIBUTE_FORMAT(x)
#define _GL_ARG_NONNULL(x)

/* Please see the Gnulib manual for how to use these macros.

Suppress extern inline with HP-UX cc, as it appears to be broken; see
<http://lists.gnu.org/archive/html/bug-texinfo/2013-02/msg00030.html>.

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
http://lists.gnu.org/archive/html/bug-gnulib/2012-12/msg00023.html

DragonFly; see
http://muscles.dragonflybsd.org/bulk/bleeding-edge-potential/latest-per-pkg/ah-tty-0.3.12.log

FreeBSD; see:
http://lists.gnu.org/archive/html/bug-gnulib/2014-07/msg00104.html

OS X 10.9 has a macro __header_inline indicating the bug is fixed for C and
for clang but remains for g++; see <http://trac.macports.org/ticket/41033>.
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
#  define _GL_INLINE extern inline __attribute__ ((__gnu_inline__))
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
#ifndef _GL_UNUSED
# if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
#  define _GL_UNUSED __attribute__ ((__unused__))
# else
#  define _GL_UNUSED
# endif
#endif



#define __libc_lock_t                   gl_lock_t
#define __libc_lock_define              gl_lock_define
#define __libc_lock_define_initialized  gl_lock_define_initialized
#define __libc_lock_init                gl_lock_init
#define __libc_lock_lock                gl_lock_lock
#define __libc_lock_unlock              gl_lock_unlock
#define __libc_lock_recursive_t                   gl_recursive_lock_t
#define __libc_lock_define_recursive              gl_recursive_lock_define
#define __libc_lock_define_initialized_recursive  gl_recursive_lock_define_initialized
#define __libc_lock_init_recursive                gl_recursive_lock_init
#define __libc_lock_lock_recursive                gl_recursive_lock_lock
#define __libc_lock_unlock_recursive              gl_recursive_lock_unlock
#define glthread_in_use  libintl_thread_in_use
#define glthread_lock_init_func     libintl_lock_init_func
#define glthread_lock_lock_func     libintl_lock_lock_func
#define glthread_lock_unlock_func   libintl_lock_unlock_func
#define glthread_lock_destroy_func  libintl_lock_destroy_func
#define glthread_rwlock_init_multithreaded     libintl_rwlock_init_multithreaded
#define glthread_rwlock_init_func              libintl_rwlock_init_func
#define glthread_rwlock_rdlock_multithreaded   libintl_rwlock_rdlock_multithreaded
#define glthread_rwlock_rdlock_func            libintl_rwlock_rdlock_func
#define glthread_rwlock_wrlock_multithreaded   libintl_rwlock_wrlock_multithreaded
#define glthread_rwlock_wrlock_func            libintl_rwlock_wrlock_func
#define glthread_rwlock_unlock_multithreaded   libintl_rwlock_unlock_multithreaded
#define glthread_rwlock_unlock_func            libintl_rwlock_unlock_func
#define glthread_rwlock_destroy_multithreaded  libintl_rwlock_destroy_multithreaded
#define glthread_rwlock_destroy_func           libintl_rwlock_destroy_func
#define glthread_recursive_lock_init_multithreaded     libintl_recursive_lock_init_multithreaded
#define glthread_recursive_lock_init_func              libintl_recursive_lock_init_func
#define glthread_recursive_lock_lock_multithreaded     libintl_recursive_lock_lock_multithreaded
#define glthread_recursive_lock_lock_func              libintl_recursive_lock_lock_func
#define glthread_recursive_lock_unlock_multithreaded   libintl_recursive_lock_unlock_multithreaded
#define glthread_recursive_lock_unlock_func            libintl_recursive_lock_unlock_func
#define glthread_recursive_lock_destroy_multithreaded  libintl_recursive_lock_destroy_multithreaded
#define glthread_recursive_lock_destroy_func           libintl_recursive_lock_destroy_func
#define glthread_once_func            libintl_once_func
#define glthread_once_singlethreaded  libintl_once_singlethreaded
#define glthread_once_multithreaded   libintl_once_multithreaded



/* On Windows, variables that may be in a DLL must be marked specially.  */
#if (defined _MSC_VER && defined _DLL) && !defined IN_RELOCWRAPPER
# define DLL_VARIABLE __declspec (dllimport)
#else
# define DLL_VARIABLE
#endif

)xxx"
);
}

void check(Checker &c)
{
    auto &s = c.addSet("intl");
    s.checkFunctionExists("alloca");
    s.checkFunctionExists("getcwd");
    s.checkFunctionExists("_wgetcwd");
    s.checkFunctionExists("mempcpy");
    s.checkFunctionExists("stpcpy");
    s.checkFunctionExists("tsearch");
    s.checkFunctionExists("wcrtomb");
    s.checkFunctionExists("stpcpy");
    s.checkFunctionExists("twalk");
    s.checkFunctionExists("tsearch");
    s.checkFunctionExists("snprintf");
    s.checkIncludeExists("alloca.h");
    s.checkIncludeExists("argz.h");
    s.checkIncludeExists("direct.h");
    s.checkIncludeExists("features.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("search.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("wchar.h");
    s.checkTypeSize("long long int");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
    s.checkTypeSize("wchar_t");
    {
        auto &c = s.checkSymbolExists("PTHREAD_MUTEX_RECURSIVE");
        c.Parameters.Includes.push_back("pthread.h");
    }
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    {
        auto &c = s.checkSymbolExists("wctype_t");
        c.Parameters.Includes.push_back("wctype.h");
    }
    {
        auto &c = s.checkSymbolExists("wint_t");
        c.Parameters.Includes.push_back("wctype.h");
    }
}
