void build(Solution &s)
{
    auto &p = s.addProject("gnu.gpg");

    auto &e = p.addProject("error", "1.51.1");
    e += RemoteFile("https://www.gnupg.org/ftp/gcrypt/libgpg-error/libgpg-error-{M}.{m}.tar.bz2");

    auto &gpg_error = e.addTarget<StaticLibraryTarget>("gpg_error");

    auto &mkheader = e.addExecutable("tools.mkheader");
    mkheader += "src/mkheader.c";
    if (mkheader.getBuildSettings().TargetOS.Type == OSType::Windows)
        mkheader += "R_OK=4"_d;
    mkheader.writeFileOnce("unistd.h");

    // for gawk 5.0 see
    // https://dev.gnupg.org/rE7865041c77f4f7005282f10f9b6666b19072fbdf
    auto gawk = "org.sw.demo.gnu.gawk.gawk"_dep;

    auto &mkerrcodes = e.addTarget<ExecutableTarget>("tools.mkerrcodes");
    {
        mkerrcodes += "src/mkerrcodes.c";
        mkerrcodes.writeFileOnce("unistd.h");

        {
            auto c = mkerrcodes.addCommand();
            c << cmd::prog(gawk)
                << "-f"
                << cmd::in("src/mkerrcodes1.awk")
                << cmd::in("src/errnos.in")
                << cmd::std_out("mkerrcodes1.h")
                ;
        }

        {
            // TODO: add include dir to libc
            std::shared_ptr<sw::Program> cc1(mkerrcodes.findProgramByExtension(".cpp")->clone());
            auto cc = std::static_pointer_cast<sw::NativeCompiler>(cc1);
            auto c = cc->getCommand(mkerrcodes);
            if (mkerrcodes.getCompilerType() == CompilerType::MSVC)
                c->arguments.push_back("-E");
            else
                c->arguments.push_back("-P");
            c->arguments.push_back((mkerrcodes.BinaryDir / "mkerrcodes1.h").u8string());
            c->redirectStdout(mkerrcodes.BinaryDir / "mkerrcodes2.h");
            c->addInput(mkerrcodes.BinaryDir / "mkerrcodes1.h");
            mkerrcodes.Storage.push_back(c);
        }

        {
            mkerrcodes.addCommand()
                << cmd::prog("org.sw.demo.gnu.grep.grep"_dep)
                << "GPG_ERR_"
                << cmd::in("mkerrcodes2.h")
                |
                mkerrcodes.addCommand()
                << cmd::prog(gawk)
                << "-f"
                << cmd::in("src/mkerrcodes.awk")
                << cmd::std_out("mkerrcodes.h")
                ;
        }
    }

    // gpg_error
    {
        gpg_error.PackageDefinitions = true;
        gpg_error.setChecks("gpg_error", true);

        gpg_error +=
            "src/.*"_rr;
        gpg_error -=
            "src/.*iconv.*"_rr,
            "src/.*posix.*"_rr,
            "src/gen-.*"_rr,
            "src/gpg-error.c",
            "src/mk.*"_rr;
        gpg_error.Public +=
            "src"_id;

        gpg_error.Private += "BUILD_REVISION=\"\""_d;
        gpg_error.Private += "BUILD_TIMESTAMP=\"\""_d;
        gpg_error.Private += "HAVE_CONFIG_H"_d;
        gpg_error.Private += "MKDIR_TAKES_ONE_ARG"_d;
        gpg_error.Private += "PKGDATADIR=\"\""_d;
        gpg_error.Private += "_GCRYPT_IN_LIBGCRYPT"_d;
        gpg_error.Public += "GPGRT_ENABLE_ARGPARSE_MACROS"_d;
        gpg_error.Public += "GPGRT_ENABLE_ES_MACROS"_d;
        gpg_error.Public += "GPGRT_HAVE_MACRO_FUNCTION"_d;
        if (gpg_error.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            gpg_error.Public += "HAVE_DOSISH_SYSTEM"_d;
            gpg_error.Public += "HAVE_W32_SYSTEM"_d;
            gpg_error.Public += "_CRT_DECLARE_NONSTDC_NAMES"_d;
            gpg_error.Public += "advapi32.lib"_slib;
        }

        gpg_error.writeFileOnce("sys/file.h");
        gpg_error.writeFileOnce("unistd.h");
        gpg_error.Protected += IncludeDirectory(gpg_error.BinaryPrivateDir);
        gpg_error.writeFileOnce(gpg_error.BinaryPrivateDir / "config.h", R"(
#ifdef _WIN32

#include <wchar.h>

#define PATHSEP_C "\\\\"

#ifdef _WIN64
#define ssize_t long long
#else
#define ssize_t int
#endif

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

#define	F_OK		0	/* test for existence of file */
#define	X_OK		0x01	/* test for execute or search permission */
#define	W_OK		0x02	/* test for write permission */
#define	R_OK		0x04	/* test for read permission */

#define _CRT_DECLARE_NONSTDC_NAMES 1
#include <io.h>

#endif

void * memrchr (const void *, int, size_t);

#include <stdint.h>
#include <string.h>
#include <stdio.h>

// must be indented here :)
#define REPLACEMENT_FOR_OFF_T off_t
)");

        gpg_error.pushFrontToFileOnce("src/gpg-error.h.in", R"(
#ifdef _WIN32
#ifdef _WIN64
#define ssize_t long long
#else
#define ssize_t int
#endif

#define strcasecmp stricmp
#define strncasecmp strnicmp
#define _CRT_DECLARE_NONSTDC_NAMES 1
#include <io.h>

#endif

void * memrchr (const void *, int, size_t);

#include <stdint.h>
#include <string.h>
#include <stdio.h>
)");

        gpg_error.patch("src/gpg-error.h.in", "#undef GPGRT_HAVE_MACRO_FUNCTION", "");
        gpg_error.patch("src/gpg-error.h.in", "@define:pid_t@", "@define:pid_t@;");
        gpg_error.patch("src/estream-printf.c", "#ifdef HAVE_W32CE_SYSTEM", "#if defined(HAVE_W32CE_SYSTEM) || defined(HAVE_W32_SYSTEM)");

        {
            auto c = gpg_error.addCommand();
            c << cmd::prog(mkheader)
                << cmd::wdir(gpg_error.SourceDir / "src")
                << "mingw32"
                << cmd::in("src/gpg-error.h.in")
                << cmd::in(gpg_error.BinaryPrivateDir / "config.h")
                << gpg_error.Variables["PACKAGE_VERSION"].toString()
                << gpg_error.Variables["PACKAGE_VERSION_NUM"].toString()
                << cmd::std_out("gpg-error.h")
                ;
        }

        {
            gpg_error.addCommand()
                << cmd::prog(mkerrcodes)
                |
                gpg_error.addCommand()
                << cmd::prog(gawk)
                << "-f"
                << cmd::in("src/mkerrcodes2.awk")
                << cmd::std_out("code-from-errno.h")
                ;
        }

        auto awk = [&gpg_error, &gawk](const auto &f, const auto &s, const Strings &args, const auto &in)
        {
            auto c = gpg_error.addCommand();
            c << cmd::prog(gawk)
                << "-f" << cmd::in(s)
                ;
            for (auto &a : args)
                c << a;
            c << cmd::in(in)
                << cmd::std_out(f)
                ;
        };

        awk("err-sources.h", "src/mkstrtable.awk", {"-v", "textidx=3"}, "src/err-sources.h.in");
        awk("err-sources-sym.h", "src/mkstrtable.awk", {"-v", "textidx=2", "-v", "nogettext=1"}, "src/err-sources.h.in");

        awk("err-codes.h", "src/mkstrtable.awk", { "-v", "textidx=3" }, "src/err-codes.h.in");
        awk("err-codes-sym.h", "src/mkstrtable.awk", { "-v", "textidx=2", "-v", "nogettext=1" }, "src/err-codes.h.in");

        awk("code-to-errno.h", "src/mkerrnos.awk", {}, "src/errnos.in");
        awk("errnos-sym.h", "src/mkstrtable.awk", { "-v", "textidx=2", "-v", "nogettext=1", "-v", "prefix=GPG_ERR_", "-v", "pkg_namespace=errnos_" },
            "src/errnos.in");
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("gpg_error");
    s.checkFunctionExists("flockfile");
    s.checkFunctionExists("fork");
    s.checkFunctionExists("getrlimit");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("rand");
    s.checkFunctionExists("setenv");
    s.checkFunctionExists("stat");
    s.checkFunctionExists("stpcpy");
    s.checkFunctionExists("strerror_r");
    s.checkFunctionExists("strlwr");
    s.checkFunctionExists("vasprintf");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("signal.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("time.h");
    s.checkTypeSize("int");
    s.checkTypeSize("long");
    s.checkTypeSize("pthread_mutex_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("time_t");
    s.checkTypeSize("void *");
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
}
