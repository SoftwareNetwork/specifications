struct GawkExecutable : ExecutableTarget
{
    void setupCommand(builder::Command &c) const override
    {
        if (getSettings().TargetOS.Type != OSType::Windows)
        {
            c.program = "gawk";
            return;
        }
        ExecutableTarget::setupCommand(c);
    }
};

void build(Solution &s)
{
    auto &gawk = s.addProject("gnu.gawk", "5.0.0");
    gawk += RemoteFile("https://ftp.gnu.org/gnu/gawk/gawk-{v}.tar.xz");

    auto &getopt = gawk.addTarget<StaticLibraryTarget>("getopt");
    {
        getopt +=
            "support/getopt\\.[hc]"_rr,
            "support/getopt_.*"_rr;

        getopt.Public +=
            "support"_id;

        getopt.Public += "org.sw.demo.gnu.gettext.intl-*"_dep;
    }

    if (gawk.getSettings().TargetOS.Type != OSType::Windows)
    {
        gawk.addTarget<GawkExecutable>("gawk");
        return;
    }

    auto &gawk2 = gawk.addTarget<GawkExecutable>("gawk");
    {
        auto &gawk = gawk2;
        gawk.PackageDefinitions = true;
        gawk.setChecks("gawk", true);

        gawk +=
            "[^/]*\\.[hc]"_rr,
            "missing_d/.*\\.[hc]"_rr,
            "pc/.*\\.[hc]"_rr,
            "pc/.*\\.pc"_rr,
            "posix/.*\\.[hc]"_rr,
            "support/.*\\.[hc]"_rr;

        gawk -=
            "missing_d/.*"_rr,
            "posix/.*"_rr,
            "support/regcomp.c",
            "support/regex_internal.c",
            "support/regexec.c";

        gawk.removeFile("pc/config.h");
        gawk.removeFile("pc/socket.h");

        gawk.Private +=
            "."_id,
            "support"_id,
            "pc"_id;

        gawk += "__USE_GNU"_def;
        gawk += "HAVE_SETENV"_def;
        if (gawk.getSettings().TargetOS.Type == OSType::Windows)
            gawk += "HAVE_USLEEP"_def;

        gawk.Private += "DEFLIBPATH=\"\""_d;
        gawk.Private += "DEFPATH=\"\""_d;
        gawk.Private += "GAWK"_d;
        gawk.Private += "HAVE_CONFIG_H"_d;
        gawk.Private += "HAVE_GETADDRINFO"_d;
        gawk.Private += "HAVE_SOCKADDR_STORAGE"_d;
        gawk.Private += "HAVE_SOCKETS"_d;
        gawk.Private += Definition{ "VERSION=\"" + gawk.Variables["PACKAGE_VERSION"].toString() + "\"" };
        if (gawk.getSettings().TargetOS.Type == OSType::Windows)
        {
            gawk.Private += "HAVE_POPEN_H=1"_d;
            gawk.Private += "P_WAIT=0"_d;
            gawk.Private += "SHLIBEXT=\"dll\""_d;
            gawk.Private += "STDC_HEADERS"_d;
            gawk.Private += "__STDC__"_d;
            gawk.Private += "pclose=_pclose"_d;
            gawk.Private += "popen=_popen"_d;
            gawk.Private += "restrict="_d;
            gawk.Private += "S_IFIFO=_S_IFIFO"_d;
        }

        gawk.replaceInFileOnce("support/regex_internal.c", "__attribute ((pure))", "");
        gawk.replaceInFileOnce("support/regexec.c", "__attribute ((always_inline))", "");

        gawk.patch("support/regex.c", "#ifdef __cplusplus", R"(

#if defined(_WIN32) && !defined(_WIN64) && !defined(SW_STATIC)
#ifndef NDEBUG
inline int __CRTDECL mbsinit(
    mbstate_t const* _P
)
{
    return _P == NULL || _P->_Wchar == 0;
}
#endif
#endif

#ifdef __cplusplus

)");

        gawk.replaceInFileOnce("nonposix.h", "#define setlocale", "//#define setlocale");
        gawk.replaceInFileOnce("gawkmisc.c", "#if defined(__EMX__)", "#include \"cppan_misc.h\"\n#if defined(__EMX__)");

        gawk.replaceInFileOnce("io.c", "#include \"awk.h\"", "#include \"awk.h\"\n#undef param\n#define __MINGW32__");

        gawk.replaceInFileOnce("pc/gawkmisc.pc", "wctob", "wctobb");
        gawk.replaceInFileOnce("pc/gawkmisc.pc", "int execvp", "intptr_t execvp");

        gawk.replaceInFileOnce("pc/popen.c", "#include \"popen.h\"", "#define __MINGW32__\n#include \"popen.h\"");
        gawk.replaceInFileOnce("pc/popen.c", "#undef popen", "//#undef  popen");
        gawk.replaceInFileOnce("pc/popen.c", "#undef pclose", "//#undef  pclose");

        gawk.replaceInFileOnce("pc/popen.c", "#include <stdio.h>", "#define _CRT_DECLARE_NONSTDC_NAMES 1\n#include <stdio.h>");

        if (gawk.getSettings().TargetOS.Type == OSType::Windows)
        {
            gawk.writeFileOnce(gawk.BinaryPrivateDir / "unistd.h");
            gawk.pushFrontToFileOnce("pc/getid.c", "#define __MINGW32__ 1");
            gawk += "ws2_32.lib"_lib;
        }

        gawk.writeFileOnce(gawk.BinaryPrivateDir / "cppan_misc.h",
            R"(
#undef param
#define __MINGW32__
#define FAKE_FD_VALUE 42

#if ! defined(S_ISREG) && defined(S_IFREG)
#define	S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

/* MinGW defines non-trivial macros on pc/socket.h.  */
#ifndef FD_TO_SOCKET
# define FD_TO_SOCKET(fd)	(fd)
# define closemaybesocket(fd)	close(fd)
#endif

#include <stdint.h>
#include <string.h>
#include <stdio.h>
)");

        gawk.writeFileOnce(gawk.BinaryPrivateDir / "in.h");
        gawk.writeFileOnce(gawk.BinaryPrivateDir / "socket.h", R"(
#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
#endif
)");

        gawk.writeFileOnce(gawk.BinaryPrivateDir / "config.h",
            R"(
/* The __pure__ attribute was added in gcc 2.96.  */
#if __GNUC__ > 2 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 96)
# define _GL_ATTRIBUTE_PURE __attribute__ ((__pure__))
#else
# define _GL_ATTRIBUTE_PURE /* empty */
#endif

#ifdef _WIN32
    #define _Noreturn __declspec(noreturn)

    #ifdef _WIN64
    #define ssize_t long long
    #else
    #define ssize_t int
    #endif

    #define _CRT_DECLARE_NONSTDC_NAMES 1
    #include <sys/stat.h>
    #include <sys/types.h>

    #define off_t int

    #  define SIGKILL	9
    #  define SIGHUP	1
    #  define SIGQUIT	3

    #include <io.h>
    //#define _CRT_INTERNAL_NONSTDC_NAMES 1
    #include <process.h>

    #define strcasecmp stricmp
    #define strncasecmp strnicmp

    #if !defined S_ISDIR && defined S_IFDIR
    # define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
    #endif
    #if !S_IRUSR && S_IREAD
    # define S_IRUSR S_IREAD
    #endif
    #if !S_IRUSR
    # define S_IRUSR 00400
    #endif
    #if !S_IWUSR && S_IWRITE
    # define S_IWUSR S_IWRITE
    #endif
    #if !S_IWUSR
    # define S_IWUSR 00200
    #endif
    #if !S_IXUSR && S_IEXEC
    # define S_IXUSR S_IEXEC
    #endif
    #if !S_IXUSR
    # define S_IXUSR 00100
    #endif

    #define _S_IRWXU  S_IRUSR | S_IWUSR | S_IXUSR

    #define __MINGW32__
    #include "nonposix.h"
    #undef __MINGW32__
#endif

#include <stdint.h>
#include <string.h>
#include <stdio.h>
)");
    }
}

void check(Checker &c)
{
    {
        auto &s = c.addSet("regex");
        s.checkIncludeExists("stdbool.h");
        s.checkTypeSize("size_t");
        s.checkTypeSize("void *");
    }
    {
        auto &s = c.addSet("gawk");
        s.checkFunctionExists("atexit");
        s.checkFunctionExists("btowc");
        s.checkFunctionExists("fmod");
        s.checkFunctionExists("fwrite_unlocked");
        s.checkFunctionExists("gai_strerror");
        s.checkFunctionExists("getgrent");
        s.checkFunctionExists("getgroups");
        s.checkFunctionExists("getpgrp");
        s.checkFunctionExists("grantpt");
        s.checkFunctionExists("isascii");
        s.checkFunctionExists("iswctype");
        s.checkFunctionExists("iswlower");
        s.checkFunctionExists("iswupper");
        s.checkFunctionExists("mbrlen");
        s.checkFunctionExists("mbrtowc");
        s.checkFunctionExists("memcmp");
        s.checkFunctionExists("memcpy");
        s.checkFunctionExists("memcpy_ulong");
        s.checkFunctionExists("memmove");
        s.checkFunctionExists("memset");
        s.checkFunctionExists("memset_ulong");
        s.checkFunctionExists("mkstemp");
        s.checkFunctionExists("mktime");
        s.checkFunctionExists("posix_openpt");
        s.checkFunctionExists("setlocale");
        s.checkFunctionExists("setsid");
        s.checkFunctionExists("sigprocmask");
        s.checkFunctionExists("strcasecmp");
        s.checkFunctionExists("strchr");
        s.checkFunctionExists("strcoll");
        s.checkFunctionExists("strerror");
        s.checkFunctionExists("strftime");
        s.checkFunctionExists("strncasecmp");
        s.checkFunctionExists("strtod");
        s.checkFunctionExists("strtoul");
        s.checkFunctionExists("system");
        s.checkFunctionExists("timegm");
        s.checkFunctionExists("tmpfile");
        s.checkFunctionExists("towlower");
        s.checkFunctionExists("towupper");
        s.checkFunctionExists("tzset");
        s.checkFunctionExists("usleep");
        s.checkFunctionExists("waitpid");
        s.checkFunctionExists("wcrtomb");
        s.checkFunctionExists("wcscoll");
        s.checkFunctionExists("wctype");
        s.checkFunctionExists("__etoa_l");
        s.checkIncludeExists("arpa/inet.h");
        s.checkIncludeExists("dlfcn.h");
        s.checkIncludeExists("fcntl.h");
        s.checkIncludeExists("libintl.h");
        s.checkIncludeExists("limits.h");
        s.checkIncludeExists("locale.h");
        s.checkIncludeExists("mcheck.h");
        s.checkIncludeExists("memory.h");
        s.checkIncludeExists("netdb.h");
        s.checkIncludeExists("netinet/in.h");
        s.checkIncludeExists("popen.h");
        s.checkIncludeExists("socket.h");
        s.checkIncludeExists("stdarg.h");
        s.checkIncludeExists("stdbool.h");
        s.checkIncludeExists("stddef.h");
        s.checkIncludeExists("stdint.h");
        s.checkIncludeExists("stdlib.h");
        s.checkIncludeExists("strings.h");
        s.checkIncludeExists("string.h");
        s.checkIncludeExists("stropts.h");
        s.checkIncludeExists("sys/ioctl.h");
        s.checkIncludeExists("sys/param.h");
        s.checkIncludeExists("sys/select.h");
        s.checkIncludeExists("sys/socket.h");
        s.checkIncludeExists("sys/time.h");
        s.checkIncludeExists("sys/wait.h");
        s.checkIncludeExists("termios.h");
        s.checkIncludeExists("time.h");
        s.checkIncludeExists("unistd.h");
        s.checkIncludeExists("wchar.h");
        s.checkIncludeExists("wctype.h");
        s.checkTypeSize("size_t");
        s.checkTypeSize("unsigned int");
        s.checkTypeSize("unsigned long");
        s.checkTypeSize("void *");
        s.checkSymbolExists("snprintf").Parameters.Includes.push_back("stdio.h");
        s.checkSymbolExists("vsnprintf").Parameters.Includes.push_back("stdio.h");
    }
}
