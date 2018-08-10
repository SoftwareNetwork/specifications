void build(Solution &s)
{
    auto &p = s.addProject("gnu.iconv", "1.15.0");
    p += RemoteFile("https://ftp.gnu.org/pub/gnu/libiconv/libiconv-{M}.{m}.tar.gz");

    auto &libcharset = p.addTarget<LibraryTarget>("libcharset");

    libcharset.setChecks("libcharset");

    libcharset +=
        "libcharset/include/localcharset.h.build.in",
        "libcharset/lib/localcharset.c";

    libcharset += "HAVE_VISIBILITY=0"_v;
    libcharset.ApiName = "LIBCHARSET_DLL_EXPORTED";
    libcharset.fileWriteOnce(libcharset.BinaryPrivateDir / "config.h", "", true);
    libcharset.replaceInFileOnce("libcharset/include/localcharset.h.build.in",
        "#define LIBCHARSET_DLL_EXPORTED",
        "//#define LIBCHARSET_DLL_EXPORTED");
    libcharset.configureFile(libcharset.SourceDir / "libcharset/include/localcharset.h.build.in", libcharset.BinaryDir / "localcharset.h");

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        libcharset.Public.Definitions["LIBDIR"] = "\".\"";
        libcharset.Public.Definitions["LOCALEDIR"] = "\"./locale\"";
    }
    else
    {
        libcharset.Public.Definitions["HAVE_WORKING_O_NOFOLLOW"];
        libcharset.Public.Definitions["LIBDIR"] = "\"/usr/local/lib\"";
        libcharset.Public.Definitions["LOCALEDIR"] = "\"/usr/share/locale\"";
    }

    if (libcharset.Variables["HAVE_LANGINFO_H"] == "1" && libcharset.Variables["HAVE_NL_LANGINFO"] == "1")
        libcharset.Public.Definitions["HAVE_LANGINFO_CODESET"];

    auto &libiconv = p.addTarget<LibraryTarget>("libiconv");
    libiconv.setChecks("libiconv");

    libiconv +=
        "include/iconv.h.build.in",
        "lib/.*\\.def"_rr,
        "lib/.*\\.gperf"_rr,
        "lib/.*\\.h"_rr,
        "lib/iconv.c";
    // prevent adding to linker
    libiconv -= "lib/.*\\.def"_rr;

    libiconv.Public += libcharset;
    libiconv.Public.Definitions["ICONV_CONST"] = "const";
    libiconv += "HAVE_VISIBILITY=0"_v;
    libiconv += "USE_MBSTATE_T=0"_v;
    libiconv += "BROKEN_WCHAR_H=0"_v;
    libiconv += "ICONV_CONST=const"_v;
    libiconv.ApiName = "LIBICONV_DLL_EXPORTED";
    libiconv.replaceInFileOnce("include/iconv.h.build.in",
        "#define LIBICONV_DLL_EXPORTED",
        "//#define LIBICONV_DLL_EXPORTED");
    libiconv.replaceInFileOnce("include/iconv.h.build.in",
        "dummy1[28]",
        "dummy1[40]");
    libiconv.configureFile(libiconv.SourceDir / "include/iconv.h.build.in", libiconv.BinaryDir / "iconv.h");
    libiconv.fileWriteOnce(libiconv.BinaryPrivateDir / "config.h",
        R"(
#define ENABLE_NLS 1
#define WORDS_LITTLEENDIAN ( ! ( WORDS_BIGENDIAN ) )

/* The _Noreturn keyword of draft C1X.  */
#ifndef _Noreturn
# if (3 <= __GNUC__ || (__GNUC__ == 2 && 8 <= __GNUC_MINOR__) \
        || 0x5110 <= __SUNPRO_C)
#  define _Noreturn __attribute__ ((__noreturn__))
# elif 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn
# endif
#endif



/* Define to the equivalent of the C99 'restrict' keyword, or to
    nothing if this is not supported.  Do not define if restrict is
    supported directly.  */
#define restrict __restrict
/* Work around a bug in Sun C++: it does not support _Restrict or
    __restrict__, even though the corresponding Sun C compiler ends up with
    \"#define restrict _Restrict\" or \"#define restrict __restrict__\" in the
    previous line.  Perhaps some future version of Sun C++ will work with
    restrict; if so, hopefully it defines __RESTRICT like Sun C does.  */
#if defined __SUNPRO_CC && !defined __RESTRICT
# define _Restrict
# define __restrict__
#endif

/* Define as a marker that can be attached to declarations that might not
    be used.  This helps to reduce warnings, such as from
    GCC -Wunused-parameter.  */
#if __GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 7)
# define _GL_UNUSED __attribute__ ((__unused__))
#else
# define _GL_UNUSED
#endif
/* The name _UNUSED_PARAMETER_ is an earlier spelling, although the name
    is a misnomer outside of parameter lists.  */
#define _UNUSED_PARAMETER_ _GL_UNUSED

/* The __pure__ attribute was added in gcc 2.96.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
#else
# define _GL_ATTRIBUTE_PURE /* empty */
#endif

/* The __const__ attribute was added in gcc 2.95.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95)
# define _GL_ATTRIBUTE_CONST __attribute__ ((__const__))
#else
# define _GL_ATTRIBUTE_CONST /* empty */
#endif

/* On Windows, variables that may be in a DLL must be marked specially.  */
//#if defined _MSC_VER && defined _DLL
//# define DLL_VARIABLE __declspec (dllimport)
//#else
//# define DLL_VARIABLE
//#endif
)",
true
);

}

void check(Checker &c)
{
    {
        auto &s = c.addSet("libcharset");
        s.checkFunctionExists("nl_langinfo");
        s.checkFunctionExists("setlocale");
        s.checkIncludeExists("dlfcn.h");
        s.checkIncludeExists("inttypes.h");
        s.checkIncludeExists("langinfo.h");
        s.checkIncludeExists("memory.h");
        s.checkIncludeExists("stdint.h");
        s.checkIncludeExists("stdlib.h");
        s.checkIncludeExists("strings.h");
        s.checkIncludeExists("string.h");
        s.checkIncludeExists("sys/stat.h");
        s.checkIncludeExists("sys/types.h");
        s.checkIncludeExists("unistd.h");
        s.checkTypeSize("size_t");
        s.checkTypeSize("void *");
    }

    {
        auto &s = c.addSet("libiconv");
        s.checkFunctionExists("alloca");
        s.checkFunctionExists("atoll");
        s.checkFunctionExists("canonicalize_file_name");
        s.checkFunctionExists("CFLocaleCopyCurrent");
        s.checkFunctionExists("CFPreferencesCopyAppValue");
        s.checkFunctionExists("dcgettext");
        s.checkFunctionExists("getcwd");
        s.checkFunctionExists("getc_unlocked");
        s.checkFunctionExists("lstat");
        s.checkFunctionExists("mbrtowc");
        s.checkFunctionExists("mbsinit");
        s.checkFunctionExists("memmove");
        s.checkFunctionExists("nl_langinfo");
        s.checkFunctionExists("readlink");
        s.checkFunctionExists("readlinkat");
        s.checkFunctionExists("realpath");
        s.checkFunctionExists("setenv");
        s.checkFunctionExists("setlocale");
        s.checkFunctionExists("strerror_r");
        s.checkFunctionExists("tsearch");
        s.checkFunctionExists("wcrtomb");
        s.checkFunctionExists("_NSGetExecutablePath");
        s.checkIncludeExists("alloca.h");
        s.checkIncludeExists("dlfcn.h");
        s.checkIncludeExists("inttypes.h");
        s.checkIncludeExists("langinfo.h");
        s.checkIncludeExists("mach-o/dyld.h");
        s.checkIncludeExists("memory.h");
        s.checkIncludeExists("search.h");
        s.checkIncludeExists("stdint.h");
        s.checkIncludeExists("stdlib.h");
        s.checkIncludeExists("strings.h");
        s.checkIncludeExists("string.h");
        s.checkIncludeExists("sys/bitypes.h");
        s.checkIncludeExists("sys/inttypes.h");
        s.checkIncludeExists("sys/stat.h");
        s.checkIncludeExists("sys/time.h");
        s.checkIncludeExists("sys/types.h");
        s.checkIncludeExists("unistd.h");
        s.checkIncludeExists("wchar.h");
        s.checkTypeSize("long long int");
        s.checkTypeSize("sigset_t");
        s.checkTypeSize("size_t");
        s.checkTypeSize("unsigned long long int");
        s.checkTypeSize("void *");
        s.checkTypeSize("wchar_t");
        s.checkTypeSize("_Bool");
        {
            auto &c = s.checkSymbolExists("mbstate_t");
            c.Parameters.Includes.push_back("wchar_t.h");
        }
    }
}
