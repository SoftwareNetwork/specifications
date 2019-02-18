void build(Solution &s)
{
    auto &p = s.addProject("gnu.gpg");

    auto cd_local = [](auto &t, auto &d)
    {
        if (t.Local && fs::exists(t.SourceDir / d))
            t.setRootDirectory(d);
    };

    auto &e = p.addProject("error", "1.35.0");
    e += RemoteFile("https://www.gnupg.org/ftp/gcrypt/libgpg-error/libgpg-error-{M}.{m}.tar.bz2");
    cd_local(e, "libgpg-error");

    auto &gpg_error = e.addTarget<StaticLibraryTarget>("gpg_error");
    {
        auto &mkheader = e.addExecutable("tools.mkheader");
        mkheader += "src/mkheader.c";
        if (s.Settings.TargetOS.Type == OSType::Windows)
            mkheader += "R_OK=4"_d;
        mkheader.writeFileOnce("unistd.h");

        auto &mkerrcodes = e.addTarget<ExecutableTarget>("tools.mkerrcodes");
        {
            mkerrcodes += "src/mkerrcodes.c";
            mkerrcodes.writeFileOnce("unistd.h");

            {
                auto c = mkerrcodes.addCommand();
                c << cmd::prog("org.sw.demo.gnu.gawk.gawk"_dep)
                    << "-f"
                    << cmd::in("src/mkerrcodes1.awk")
                    << cmd::in("src/errnos.in")
                    << cmd::std_out("mkerrcodes1.h")
                    ;
            }

            {
                auto cc = std::static_pointer_cast<Compiler>(s.findProgramByExtension(".cpp")->clone());
                auto c = cc->getCommand(mkerrcodes);
                if (s.Settings.Native.CompilerType == CompilerType::MSVC)
                    c->args.push_back("-E");
                else
                    c->args.push_back("-P");
                c->args.push_back((mkerrcodes.BinaryDir / "mkerrcodes1.h").u8string());
                c->redirectStdout(mkerrcodes.BinaryDir / "mkerrcodes2.h");
                c->addInput(mkerrcodes.BinaryDir / "mkerrcodes1.h");
                mkerrcodes.Storage.push_back(c);
            }

            {
                auto c = mkerrcodes.addCommand();
                c << cmd::prog("org.sw.demo.gnu.grep.grep"_dep)
                    << "GPG_ERR_"
                    << cmd::in("mkerrcodes2.h")
                    << cmd::std_out("mkerrcodes3.h")
                    ;
            }

            {
                auto c = mkerrcodes.addCommand();
                c << cmd::prog("org.sw.demo.gnu.gawk.gawk"_dep)
                    << "-f"
                    << cmd::in("src/mkerrcodes.awk")
                    << cmd::in("mkerrcodes3.h")
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
            if (s.Settings.TargetOS.Type == OSType::Windows)
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
                auto c = gpg_error.addCommand();
                c << cmd::prog(mkerrcodes)
                    << cmd::std_out("code-from-errno1.h")
                    ;
            }

            {
                auto c = gpg_error.addCommand();
                c << cmd::prog("org.sw.demo.gnu.gawk.gawk"_dep)
                    << "-f"
                    << cmd::in("src/mkerrcodes2.awk")
                    << cmd::in("code-from-errno1.h")
                    << cmd::std_out("code-from-errno.h")
                    ;
            }

            auto awk = [&gpg_error](const auto &f, const auto &s, const Strings &args, const auto &in)
            {
                auto c = gpg_error.addCommand();
                c << cmd::prog("org.sw.demo.gnu.gawk.gawk"_dep)
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
            awk("errnos-sym.h", "src/mkstrtable.awk", { "-v", "textidx=2", "-v", "nogettext=1", "-v", "prefix=GPG_ERR_", "-v", "namespace=errnos_" },
                "src/errnos.in");
        }
    }

    auto &a = p.addProject("assuan", "2.5.3");
    a += RemoteFile("https://www.gnupg.org/ftp/gcrypt/libassuan/libassuan-{v}.tar.bz2");
    cd_local(a, "libassuan-2.5.3");

    auto &assuan = a.addTarget<LibraryTarget>("assuan");
    {
        auto &mkheader = a.addExecutable("tools.mkheader");
        mkheader += "src/mkheader.c";

        assuan.PackageDefinitions = true;
        assuan.setChecks("assuan", true);

        assuan += "src/.*"_rr;
        assuan -=
            "src/funopen.c",
            "src/gpgce.*"_rr,
            "src/mkheader.c",
            "src/system-posix.c",
            "src/system-w32ce.c";
        assuan.Public +=
            "src"_id;

        assuan.Private += "BUILD_REVISION=\"\""_d;
        assuan.Private += "BUILD_TIMESTAMP=\"\""_d;
        assuan.Private += "HAVE_CONFIG_H"_d;

        assuan.Public += gpg_error;

        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            assuan.patch("src/w32-types.inc.h", "typedef long ssize_t;", R"(
    #ifdef _WIN64
    #define ssize_t long long
    #else
    #define ssize_t int
    #endif
)");
            assuan.patch("src/assuan-handler.c","{ } };", "};");
            assuan.Public += "ws2_32.lib"_slib;
        }

        {
            auto c = assuan.addCommand();
            c << cmd::prog(mkheader)
                << cmd::wdir(assuan.SourceDir / "src")
                << "mingw32"
                << cmd::in("src/assuan.h.in")
                << assuan.Variables["PACKAGE_VERSION"].toString()
                << assuan.Variables["PACKAGE_VERSION_NUM"].toString()
                << cmd::std_out("assuan.h")
                ;
        }
    }

    auto &m = p.addProject("gpgme", "1.12.0");
    m += RemoteFile("https://www.gnupg.org/ftp/gcrypt/gpgme/gpgme-{v}.tar.bz2");
    cd_local(m, "gpgme-1.12.0");

    {
        auto &gpgme = m.addTarget<LibraryTarget>("gpgme");

        gpgme.PackageDefinitions = true;
        gpgme.setChecks("gpgme", true);

        gpgme += "src/.*"_rr;
        gpgme -=
            "src/setenv.*"_rr,
            "src/gpgme-.*"_rr,
            "src/posix-io.c",
            "src/posix-util.c",
            "src/w32-ce.c",
            "src/w32-glib-io.c";
        gpgme.Public += "src"_id;

        gpgme.Private += "BUILD_REVISION=\"\""_d;
        gpgme.Private += "BUILD_TIMESTAMP=\"\""_d;
        gpgme.Private += "CRIGHTBLURB=\"\""_d;
        gpgme.Private += "HAVE_CONFIG_H"_d;
        gpgme.Private += Definition("VERSION=\"" + gpgme.Variables["PACKAGE_VERSION"].toString() + "\"");
        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            gpgme.Private += "REPLACE_TTYNAME_R"_d;
            gpgme.Public += "user32.lib"_slib;
            gpgme.Public += "Shell32.lib"_slib;
        }

        gpgme.Public += assuan;

        gpgme.Variables["NEED__FILE_OFFSET_BITS"] = 0;
        gpgme.Variables["API__OFF_T"] = "off_t";
        gpgme.Variables["API__SSIZE_T"] = "ssize_t";

        gpgme.pushBackToFileOnce("src/gpgme.h.in", R"(
#define gpgme_ssize_t ssize_t
#define gpgme_off_t off_t
#define GPGME_GCC_A_PURE
)");

        gpgme.patch("src/cJSON.c",
            "# include <gpgrt.h>",
            "# include <gpg-error.h>"
        );

        gpgme.configureFile("src/gpgme.h.in", "gpgme.h");

        //
        auto &gpgme_w32spawn = m.addTarget<ExecutableTarget>("gpgme_w32spawn");
        gpgme_w32spawn += "src/gpgme-w32spawn.c";
        gpgme_w32spawn += assuan;
        if (s.Settings.TargetOS.Type == OSType::Windows)
            gpgme_w32spawn += "pid_t=int"_def;

        //
        auto &gpgmecpp = m.addTarget<LibraryTarget>("gpgmecpp");
        gpgmecpp.ApiNames.insert("GPGMEPP_EXPORT");
        gpgmecpp.setRootDirectory("lang/cpp");
        gpgmecpp += "src/.*"_rr;
        gpgmecpp -=
            "src/context_glib.cpp",
            "src/context_qt.cpp";
        gpgmecpp.Private += "HAVE_CONFIG_H"_d;
        if (s.Settings.TargetOS.Type == OSType::Windows)
            gpgmecpp.Public += "WIN32"_d;
        gpgmecpp.Public += gpgme;

        gpgmecpp.writeFileOnce("strings.h");
        gpgmecpp.writeFileOnce("sys/time.h");
        gpgmecpp.writeFileOnce("config.h", R"(
        #ifdef _WIN32
            #ifdef _WIN64
            #define ssize_t long long
            #else
            #define ssize_t int
            #endif

            #define GPGMEPP_DEPRECATED __declspec(deprecated)
        #endif
)");
    }
}

void check(Checker &c)
{
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

    {
        auto &s = c.addSet("assuan");
        s.checkFunctionExists("flockfile");
        s.checkFunctionExists("fopencookie");
        s.checkFunctionExists("funlockfile");
        s.checkFunctionExists("funopen");
        s.checkFunctionExists("getaddrinfo");
        s.checkFunctionExists("getpeereid");
        s.checkFunctionExists("getpeerucred");
        s.checkFunctionExists("getrlimit");
        s.checkFunctionExists("inet_pton");
        s.checkFunctionExists("setsockopt");
        s.checkFunctionExists("stat");
        s.checkIncludeExists("fcntl.h");
        s.checkIncludeExists("inttypes.h");
        s.checkIncludeExists("locale.h");
        s.checkIncludeExists("stdint.h");
        s.checkIncludeExists("stdlib.h");
        s.checkIncludeExists("string.h");
        s.checkIncludeExists("sys/select.h");
        s.checkIncludeExists("sys/stat.h");
        s.checkIncludeExists("sys/time.h");
        s.checkIncludeExists("sys/types.h");
        s.checkIncludeExists("sys/ucred.h");
        s.checkIncludeExists("sys/uio.h");
        s.checkIncludeExists("ucred.h");
        s.checkIncludeExists("unistd.h");
        s.checkIncludeExists("winsock2.h");
        s.checkTypeSize("signal");
        s.checkTypeSize("size_t");
        s.checkTypeSize("uint16_t");
        s.checkTypeSize("uintptr_t");
        s.checkTypeSize("void *");
        s.checkLibraryFunctionExists("socket", "setsockopt");
        {
            auto &c = s.checkStructMemberExists("struct cmsghdr", "cmsg_len");
        }
        {
            auto &c = s.checkStructMemberExists("struct sockpeercred", "pid");
        }
        s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
    }

    {
        auto &s = c.addSet("gpgme");
        s.checkFunctionExists("closefrom");
        s.checkFunctionExists("fseeko");
        s.checkFunctionExists("getegid");
        s.checkFunctionExists("getenv_r");
        s.checkFunctionExists("getgid");
        s.checkFunctionExists("setlocale");
        s.checkFunctionExists("timegm");
        s.checkIncludeExists("argp.h");
        s.checkIncludeExists("locale.h");
        s.checkIncludeExists("stdint.h");
        s.checkIncludeExists("sys/select.h");
        s.checkIncludeExists("sys/stat.h");
        s.checkIncludeExists("sys/time.h");
        s.checkIncludeExists("sys/types.h");
        s.checkIncludeExists("sys/uio.h");
        s.checkIncludeExists("unistd.h");
        s.checkTypeSize("error_t");
        s.checkTypeSize("int");
        s.checkTypeSize("off_t");
        s.checkTypeSize("size_t");
        s.checkTypeSize("uintptr_t");
        s.checkTypeSize("unsigned");
        s.checkTypeSize("void *");
        {
            auto &c = s.checkStructMemberExists("struct cmsghdr", "cmsg_len");
        }
        s.checkSourceCompiles("HAVE_TLS", R"sw_xxx(__thread int foo;
     int main() {
    )
     ; return 0; })sw_xxx");
    }
}
