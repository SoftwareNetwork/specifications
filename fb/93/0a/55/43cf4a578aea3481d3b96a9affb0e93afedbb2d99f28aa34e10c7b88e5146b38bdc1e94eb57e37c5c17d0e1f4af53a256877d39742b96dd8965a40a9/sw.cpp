static int pg_process_errocodes(path i, path o)
{
    String s;
    for (auto &l : read_lines(i))
    {
        if (l[0] == '#')
            continue;
        static auto sect = "Section: "s;
        if (l.find(sect) == 0)
        {
            s += "/* " + l.substr(sect.size()) + " */\n";
            continue;
        }
        static std::regex r(R"x(^([^\s]{5})\s+[EWS]\s+([^\s]+))x");
        std::smatch m;
        if (!std::regex_search(l, m, r))
            throw SW_RUNTIME_ERROR("Bad errorcodes.");
        s += "#define " + m[2].str() + " MAKE_SQLSTATE(" + m[1].str() + ")\n";
    }
    write_file(o, s);
    return 0;
}
SW_DEFINE_VISIBLE_FUNCTION_JUMPPAD2(pg_process_errocodes)

static int pg_create_def_file(path i, path o)
{
    String s = "LIBRARY\nEXPORTS\n";
    for (auto &l : read_lines(i))
    {
        if (l[0] == '#')
            continue;
        static std::regex r(R"x(^(\S+)\s+(\d+))x");
        std::smatch m;
        if (!std::regex_search(l, m, r))
            throw SW_RUNTIME_ERROR("Bad errorcodes.");
        s += m[1].str() + "\n";
    }
    write_file(o, s);
    return 0;
}
SW_DEFINE_VISIBLE_FUNCTION_JUMPPAD2(pg_create_def_file)

void build(Solution &s)
{
    auto &pg = s.addProject("postgres", "17.6.1");
    pg += Git("https://github.com/postgres/postgres", "REL_{M}_{m}");
    // release tarballs have all generated files, so it simplifies task
    // but we should prefer our own way in order to build pg with sw in the future
    // ALSO in this case there will be their generated paths etc.
    //pg += RemoteFile("https://ftp.postgresql.org/pub/source/v{M}.{m}/postgresql-{M}.{m}.tar.bz2");

    auto &includes = pg.addStaticLibrary("includes");
    {
        auto &t = includes;
        t.setChecks("includes");

        t += "src/include/.*\\.h"_rr;
        t += "src/include/.*\\.in"_rr;

        t.Public += "src/include"_idir;

        if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            t.Protected += "WIN32"_def;

            t.Protected += "src/include/port/win32"_idir;
            t.Protected += "src/include/port/win32_msvc"_idir;

            t.Variables["USE_WIN32_RANDOM"] = 1;
            t.Variables["USE_WIN32_SEMAPHORES"] = 1;
            t.Variables["USE_WIN32_SHARED_MEMORY"] = 1;
        }
        else
        {
            t.Protected += "_GNU_SOURCE"_def;

            t.Variables["USE_OPENSSL_RANDOM"] = 1;
        }

        t.Variables["FLEXIBLE_ARRAY_MEMBER"] = "/**/"; // win32

        t.Variables["BLCKSZ"] = "8192";
        t.Variables["XLOG_BLCKSZ"] = "8192";

        t.Variables["HAVE_SSL_GET_CURRENT_COMPRESSION"] = 1;
        t.Variables["HAVE_STRUCT_ADDRINFO"] = 1;
        t.Variables["HAVE_STRUCT_SOCKADDR_STORAGE"] = 1;
        t.Variables["HAVE_STRUCT_SOCKADDR_STORAGE_SS_FAMILY"] = 1;
        t.Variables["HAVE_FSEEKO"] = 1;
        if (t.getBuildSettings().TargetOS.is(OSType::Windows)) {
            t.Variables["HAVE__CPUID"] = 1;
            t.Variables["HAVE_SOCKLEN_T"] = 1;
        }
        t.Variables["USE_SSE42_CRC32C_WITH_RUNTIME_CHECK"] = 1;
        t.Variables["HAVE_LONG_LONG_INT_64"] = 1;
        t.Variables["PG_INT64_TYPE"] = "long long int";
        t.Variables["MEMSET_LOOP_LIMIT"] = 1024;
        t.Variables["ACCEPT_TYPE_ARG3"] = "int";
        t.Variables["INT64_MODIFIER"] = "\"ll\"";
        t.Variables["PG_MAJORVERSION"] = "\"" + t.getPackage().getVersion().toString(1) + "\"";
        t.Variables["PG_VERSION"] = "\"" + t.getPackage().getVersion().toString(1) + "\"";
        t.Variables["PG_VERSION_NUM"] = t.getPackage().getVersion().getMajor() * 10000 + t.getPackage().getVersion().getMinor() * 100;
        if (t.getCompilerType() == CompilerType::MSVC)
            t.Variables["pg_restrict"] = "__restrict";
        else
            t.Variables["pg_restrict"] = "restrict";
        t.Variables["MAXIMUM_ALIGNOF"] = 64;
        t.Variables["DEF_PGPORT"] = 5432;
        t.Variables["DEF_PGPORT_STR"] = "\"5432\"";
        t.Variables["PG_KRB_SRVNAM"] = "\"\"";
        t.Variables["CONFIGURE_ARGS"] = "\"\"";

        t.configureFile("src/include/pg_config.h.in", "pg_config.h", ConfigureFlags::EnableUndefReplacements);
        t.configureFile("src/include/pg_config_ext.h.in", "pg_config_ext.h", ConfigureFlags::EnableUndefReplacements);
        t.writeFileOnce("pg_config_os.h");

        t.writeFileOnce("pg_config_paths.h", R"(
#define PGBINDIR "."
#define PGSHAREDIR "."
#define SYSCONFDIR "."
#define INCLUDEDIR "."
#define PKGINCLUDEDIR "."
#define INCLUDEDIRSERVER "."
#define LIBDIR "."
#define PKGLIBDIR "."
#define LOCALEDIR "."
#define DOCDIR "."
#define HTMLDIR "."
#define MANDIR "."

/*
// original:
#define PGBINDIR "/bin"
#define PGSHAREDIR "/share"
#define SYSCONFDIR "/etc"
#define INCLUDEDIR "/include"
#define PKGINCLUDEDIR "/include"
#define INCLUDEDIRSERVER "/include/server"
#define LIBDIR "/lib"
#define PKGLIBDIR "/lib"
#define LOCALEDIR "/share/locale"
#define DOCDIR "/doc"
#define HTMLDIR "/doc"
#define MANDIR "/man"
*/
)");

        {
            auto c = t.addCommand(SW_VISIBLE_FUNCTION(pg_process_errocodes));
            c
                << cmd::in("src/backend/utils/errcodes.txt")
                << cmd::out("utils/errcodes.h");
        }
    }

    auto &common = pg.addStaticLibrary("common");
    {
        common += "src/common/.*\\.[hc]"_r;

        //common -= "src/common/sha2_openssl.c";
        common -= "src/common/relpath.c"; // not for libpq
        common -= "src/common/keywords.c"; // not for libpq

        common.Protected += "FRONTEND"_def;
        common.Protected += "WIN32_LEAN_AND_MEAN"_def;

        common.Public += includes;
        common.Protected += "org.sw.demo.openssl.crypto"_dep;

        common += "SSL_CTX_set_min_proto_version"_def;
        common += "HAVE_HMAC_CTX_NEW"_def;
        common += "HAVE_HMAC_CTX_FREE"_def;
    }

    auto &port = pg.addStaticLibrary("port");
    {
        port.setChecks("includes");

        port += "src/port/.*\\.[hc]"_r;
        port -= "src/port/.*armv8.*"_rr;
        port -= "src/port/pthread_barrier_wait.c";
        port -= "src/port/pg_popcount_avx512_choose.c";
        port -= "src/port/pg_popcount_avx512.c";
        port -= "src/port/pg_crc32c_loongarch.c";
        if (!port.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            port -= "src/port/.*win32.*"_rr;
            port -= "src/port/dirent.c";
            //port -= "src/port/unsetenv.c";
            //port -= "src/port/gettimeofday.c";
            //port -= "src/port/getrusage.c";
            port -= "src/port/getopt.*"_rr;

            //port -= "src/port/pg_bitutils.c";
            //port -= "src/port/getpeereid.c";
            if (port.getBuildSettings().TargetOS.is(OSType::Linux)) {
                port += "HAVE__GET_CPUID"_def; // not every arch (prob not arm)
                port += "SO_PEERCRED"_def; // linux only! not on freebsd/solaris
            }
            port -= "src/port/pg_crc32c_sse42_choose.c";
            if (port.getBuildSettings().TargetOS.Arch != ArchType::aarch64)
                port.CompileOptions.push_back("-msse4.2");
        }
        if (port.getCompilerType() == CompilerType::Clang || port.getCompilerType() == CompilerType::ClangCl) {
            if (port.getBuildSettings().TargetOS.Arch != ArchType::aarch64) {
                port.CompileOptions.push_back("-msse4.2");
            }
        }
        if (port.getBuildSettings().TargetOS.is(OSType::Linux))
        {
            //port += "bsd"_slib;
        }
        if (port.getBuildSettings().TargetOS.isApple())
        {
            port -= "src/port/preadv.c";
            port -= "src/port/pwritev.c";
            port -= "src/port/strlcat.c";
            port -= "src/port/strlcpy.c";
            port -= "src/port/pg_crc32c_sse42.c";
        }
        port.Protected += "src/port"_idir;

        port += "STRERROR_R_INT"_def;
        port.Protected += "FRONTEND"_def;

        port.Protected += includes;
    }

    auto &pq = pg.addLibrary("pq");
    {
        if (!pq.getBuildSettings().TargetOS.is(OSType::Windows))
            pq.ExportAllSymbols = true;

        pq += "src/interfaces/libpq/.*\\.[hc]"_r;
        //pq += "src/interfaces/libpq/.*\\.def"_r;
        pq -= "src/interfaces/libpq/.*gssapi.*"_r;
        pq -= "src/interfaces/libpq/.*win32.c"_rr;
        pq += "src/common/encnames.c";
        pq += "src/common/wchar.c";

        pq.Public += "src/interfaces/libpq"_idir;

        pq += "USE_OPENSSL"_def;
        pq += "HAVE_BIO_GET_DATA"_def;
        pq += "HAVE_BIO_METH_NEW"_def;
        pq.Public += common;
        pq.Protected += port;
        pq += "org.sw.demo.openssl.ssl"_dep;
        //pq.patch("src/interfaces/libpq/libpqdll.def", "LIBRARY LIBPQ", "LIBRARY");

        if (pq.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            pq += "src/interfaces/libpq/.*win32.c"_rr;
            pq += "Secur32.lib"_slib;
            pq += "Shell32.lib"_slib;
            pq += "ws2_32.lib"_slib;
            pq += "wldap32.lib"_slib;
        }

        {
            auto c = pq.addCommand(SW_VISIBLE_FUNCTION(pg_create_def_file));
            c
                << cmd::in("src/interfaces/libpq/exports.txt")
                << cmd::out("export.def");
        }
    }

    return;

    auto &fe_utils = pg.addStaticLibrary("fe.utils");
    {
        fe_utils += "src/fe_utils/.*\\.[hc]"_r;
        fe_utils.Protected += pq;
    }

    auto &bin = pg.addDirectory("bin");

    auto add_binary = [&bin, &fe_utils](const String &name) -> decltype(auto)
    {
        auto &t = bin.addExecutable(name);
        t.setRootDirectory("src/bin/scripts");
        t += "common.c";
        t += path(name + ".c");
        t += fe_utils;
        return t;
    };

    add_binary("createdb");
    add_binary("clusterdb");
    add_binary("createuser");
    add_binary("dropdb");
    add_binary("dropuser");
    add_binary("reindexdb") += "scripts_parallel.c";
    add_binary("vacuumdb") += "scripts_parallel.c";

    return;

    // pg
    /*{
        pg += "src/backend/.*\\.[hc]"_rr;
        pg += "src/common/.*\\.[hc]"_rr;
        pg += "src/port/.*\\.[hc]"_rr;
        pg += "src/include/.*\\.h"_rr;
        pg += "src/include/.*\\.in"_rr;

        pg.Protected += "src/include"_idir;
        pg.Protected += "src/include/port/win32"_idir;
        pg.Protected += "src/include/port/win32_msvc"_idir;

        pg.Protected += "src/include/port/win32"_idir;
        pg.Protected += "src/include/port/win32_msvc"_idir;

        pg.Protected += "org.sw.demo.openssl.crypto"_dep;
    }*/
}

void check(Checker &c)
{
    auto &s = c.addSet("includes");
    s.checkFunctionExists("accept_argtypes");
    s.checkFunctionExists("append_history");
    s.checkFunctionExists("backtrace_symbols");
    s.checkFunctionExists("cbrt");
    s.checkFunctionExists("class");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("copyfile");
    s.checkFunctionExists("fdatasync");
    s.checkFunctionExists("fpclass");
    s.checkFunctionExists("fp_class");
    s.checkFunctionExists("fp_class_d");
    s.checkFunctionExists("fseeko");
    s.checkFunctionExists("gethostbyname_r");
    s.checkFunctionExists("getifaddrs");
    {
        auto &c = s.checkFunctionExists("getpeerucred");
        c.Parameters.Includes.push_back("ucred.h");
        c.Parameters.Includes.push_back("sys/ucred.h");
    }
    s.checkFunctionExists("getpwuid_r");
    s.checkFunctionExists("getrlimit");
    s.checkFunctionExists("gettimeofday_1arg");
    s.checkFunctionExists("history_truncate_file");
    s.checkFunctionExists("mbstowcs_l");
    s.checkFunctionExists("memmove");
    s.checkFunctionExists("memset_s");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("posix_fadvise");
    s.checkFunctionExists("posix_fallocate");
    s.checkFunctionExists("ppoll");
    s.checkFunctionExists("pstat");
    s.checkFunctionExists("pthread_is_threaded_np");
    s.checkFunctionExists("readlink");
    s.checkFunctionExists("rl_completion_matches");
    s.checkFunctionExists("rl_filename_completion_function");
    s.checkFunctionExists("rl_reset_screen_size");
    s.checkFunctionExists("setproctitle");
    s.checkFunctionExists("setproctitle_fast");
    s.checkFunctionExists("setsid");
    s.checkFunctionExists("shm_open");
    s.checkFunctionExists("strchrnul");
    s.checkFunctionExists("strerror_r");
    //s.checkFunctionExists("strerror_r_int");
    s.checkFunctionExists("strsignal");
    s.checkFunctionExists("strtoll");
    s.checkFunctionExists("strtoq");
    s.checkFunctionExists("strtoull");
    s.checkFunctionExists("strtouq");
    s.checkFunctionExists("symlink");
    //s.checkFunctionExists("sync_file_range");
    s.checkFunctionExists("syslog");
    s.checkFunctionExists("unsetenv");
    s.checkFunctionExists("uselocale");
    s.checkFunctionExists("utime");
    s.checkFunctionExists("utimes");
    s.checkFunctionExists("uuid_generate");
    s.checkFunctionExists("uuid_to_string");
    s.checkFunctionExists("wcstombs_l");
    s.checkFunctionExists("_configthreadlocale");
    s.checkFunctionExists("__strtoll");
    s.checkFunctionExists("__strtoull");
    s.checkIncludeExists("atomic.h");
    s.checkIncludeExists("bsd_auth.h");
    s.checkIncludeExists("copyfile.h");
    s.checkIncludeExists("crtdefs.h");
    s.checkIncludeExists("dns_sd.h");
    s.checkIncludeExists("execinfo.h");
    s.checkIncludeExists("fp_class.h");
    s.checkIncludeExists("getopt.h");
    s.checkIncludeExists("history.h");
    s.checkIncludeExists("ieeefp.h");
    s.checkIncludeExists("ifaddrs.h");
    s.checkIncludeExists("langinfo.h");
    s.checkIncludeExists("ldap.h");
    s.checkIncludeExists("mbarrier.h");
    s.checkIncludeExists("netinet/tcp.h");
    s.checkIncludeExists("net/if.h");
    //s.checkIncludeExists("ossp/uuid.h");
    //s.checkIncludeExists("pam/pam_appl.h");
    s.checkIncludeExists("perl.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("pthread.h");
    //s.checkIncludeExists("security/pam_appl.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("syslog.h");
    s.checkIncludeExists("sys/epoll.h");
    s.checkIncludeExists("sys/ipc.h");
    s.checkIncludeExists("sys/prctl.h");
    s.checkIncludeExists("sys/procctl.h");
    s.checkIncludeExists("sys/pstat.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/sem.h");
    s.checkIncludeExists("sys/shm.h");
    s.checkIncludeExists("sys/sockio.h");
    s.checkIncludeExists("socket.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/tas.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/ucred.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("sys/un.h");
    s.checkIncludeExists("tcl.h");
    s.checkIncludeExists("termios.h");
    s.checkIncludeExists("ucred.h");
    s.checkIncludeExists("utime.h");
    s.checkIncludeExists("unistd.h");
    //s.checkIncludeExists("uuid.h");
    //s.checkIncludeExists("uuid/uuid.h");
    s.checkIncludeExists("wchar.h");
    s.checkIncludeExists("wctype.h");
    s.checkIncludeExists("winldap.h");
    s.checkTypeSize("bool");
    //s.checkTypeSize("cmsgcred");
    s.checkTypeSize("int64");
    s.checkTypeSize("int8");
    s.checkTypeSize("intptr_t");
    s.checkTypeSize("locale_t");
    s.checkTypeSize("long");
    s.checkTypeSize("MINIDUMP_TYPE");
    s.checkTypeSize("off_t");
    s.checkTypeSize("option");
    s.checkTypeSize("size_t");
    s.checkTypeSize("sockaddr_in6");
    s.checkTypeSize("struct");
    s.checkTypeSize("uint64");
    s.checkTypeSize("uint8");
    s.checkTypeSize("uintptr_t");
    s.checkTypeSize("void *");
    s.checkTypeAlignment("double");
    s.checkTypeAlignment("int");
    s.checkTypeAlignment("long");
    s.checkTypeAlignment("long long int");
    s.checkTypeAlignment("short");
    /*s.checkLibraryFunctionExists("crypto", "CRYPTO_new_ex_data");
    s.checkLibraryFunctionExists("z", "inflate");
    s.checkLibraryFunctionExists("ldap", "ldap_bind");
    s.checkLibraryFunctionExists("ldap_r", "ldap_simple_bind");
    s.checkLibraryFunctionExists("m", "main");
    s.checkLibraryFunctionExists("pam", "pam_start");
    s.checkLibraryFunctionExists("selinux", "security_compute_create_name");
    s.checkLibraryFunctionExists("ossp-uuid", "uuid_export");
    s.checkLibraryFunctionExists("uuid", "uuid_generate");
    s.checkLibraryFunctionExists("xml2", "xmlSaveToBuffer");
    s.checkLibraryFunctionExists("xslt", "xsltCleanupGlobals");*/
    {
        auto &c = s.checkSymbolExists("tzname");
        c.Parameters.Includes.push_back("time.h");
    }
    s.checkSourceCompiles("HAVE_ISINF", R"sw_xxx(
    #include <math.h>
    double glob_double;


     int main() {

    return isinf(glob_double) ? 0 : 1;

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_PS_STRINGS", R"sw_xxx(#include <machine/vmparam.h>
    #include <sys/exec.h>


     int main() {

    PS_STRINGS->ps_nargvstr = 1;
    PS_STRINGS->ps_argvstr = "foo";

     ; return 0; })sw_xxx");
    s.checkDeclarationExists("fdatasync");
    s.checkDeclarationExists("F_FULLFSYNC");
    s.checkDeclarationExists("posix_fadvise");
    s.checkDeclarationExists("RTLD_GLOBAL");
    s.checkDeclarationExists("RTLD_NOW");
    s.checkDeclarationExists("strlcat");
    s.checkDeclarationExists("strlcpy");
    s.checkDeclarationExists("strnlen");
    s.checkDeclarationExists("strtoll");
    s.checkDeclarationExists("strtoull");
}
