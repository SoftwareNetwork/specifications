void build(Solution &s)
{
    auto &p = s.addProject("gnu.gpg");

    auto &m = p.addProject("gpgme", "1.13.0");
    m += RemoteFile("https://www.gnupg.org/ftp/gcrypt/gpgme/gpgme-{v}.tar.bz2");

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

    gpgme.Public += "org.sw.demo.gnu.gpg.assuan.assuan"_dep;

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
    gpgme_w32spawn += "org.sw.demo.gnu.gpg.assuan.assuan"_dep;
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

void check(Checker &c)
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
