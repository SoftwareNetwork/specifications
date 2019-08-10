void build(Solution &s)
{
    auto &p = s.addProject("openldap", "2.4.48");
    p += Git("git://git.openldap.org/openldap.git", "OPENLDAP_REL_ENG_{M}_{m}_{p}");

    auto &lber = p.addTarget<Library>("lber");
    //lber.setSourceDirectory("1");
    {
        auto &t = lber;

        t.setChecks("ldap");
        t += "include/.*"_rr;
        t += "libraries/liblber/.*"_rr;
        t -= "libraries/liblber/.*test.*"_rr;
        t -= "libraries/liblber/stdio.c";
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            t += "ws2_32.lib"_slib;
        else
            t -= "libraries/liblber/nt_err.c";

        t.Public += "include"_idir;
        t.Protected += "libraries/liblber"_idir;

        t.ApiName = "LBER_API";
        t += "LDAP_API="_def;
        t.patch("include/ldap_cdefs.h", "LBER_F(type)		extern type", "LBER_F(type)		extern LBER_API type");
        t.patch("include/ldap_cdefs.h", "LBER_V(type)		extern type", "LBER_V(type)		extern LBER_API type");
        t.patch("include/ldap_cdefs.h", "LDAP_F(type)		extern type", "LDAP_F(type)		extern LDAP_API type");
        t.patch("include/ldap_cdefs.h", "LDAP_V(type)		extern type", "LDAP_V(type)		extern LDAP_API type");

        t.Variables["LBER_INT_T"] = "int";
        t.Variables["LBER_TAG_T"] = "int";
        t.Variables["LBER_SOCKET_T"] = "int";
        t.Variables["ber_socklen_t"] = "int";
        t.Variables["LBER_LEN_T"] = "int";
        //
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            t.Variables["uid_t"] = "int";
            t.Variables["gid_t"] = "int";
        }
        //
        t.configureFile("include/lber_types.hin", "lber_types.h",
            (ConfigureFlags)((int)ConfigureFlags::EnableUndefReplacements | (int)ConfigureFlags::ReplaceUndefinedVariablesWithZeros));

        t.Variables["LDAP_VENDOR_VERSION"] = t.Variables["PACKAGE_VERSION_NUM"];
        t.Variables["LDAP_VENDOR_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
        t.Variables["LDAP_VENDOR_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
        t.Variables["LDAP_VENDOR_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];
        //
        t.configureFile("include/portable.hin", "portable.h",
            (ConfigureFlags)((int)ConfigureFlags::EnableUndefReplacements | (int)ConfigureFlags::ReplaceUndefinedVariablesWithZeros));
        t.configureFile("include/ldap_config.hin", "ldap_config.h",
            (ConfigureFlags)((int)ConfigureFlags::EnableUndefReplacements | (int)ConfigureFlags::ReplaceUndefinedVariablesWithZeros));

        t.configureFile("include/ldap_features.hin", "ldap_features.h",
            (ConfigureFlags)((int)ConfigureFlags::EnableUndefReplacements | (int)ConfigureFlags::ReplaceUndefinedVariablesWithZeros));

        t.pushBackToFileOnce("include/ac/string.h", R"(#ifdef _WIN32
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif
)");

        //t.patch("libraries/liblber/debug.c", "#include \"ldap_pvt.h\"", "//#include  \"ldap_pvt.h\"");

        t.patch("include/ldap_pvt_uc.h",
            "#include \"../libraries/liblunicode/ucdata/ucdata.h\"",
            "//#include  \"../libraries/liblunicode/ucdata/ucdata.h\"");
    }

    auto add_ldap = [&lber, &s](auto &t)
    {
        t += "libraries/libldap/.*"_rr;
        t -= "libraries/libldap/.*test.*"_rr;
        t -= "libraries/libldap/t61.c";
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            t += "ws2_32.lib"_slib;

        t.Protected += "libraries/libldap"_idir;

        t.ApiName = "LDAP_API";
        t.patch("libraries/libldap/ldap-int.h", "#include \"../liblber/lber-int.h\"", "#include \"lber-int.h\"");

        t.Public += lber;
    };

    auto &ldap = p.addTarget<Library>("ldap");
    //ldap.setSourceDirectory("2");
    add_ldap(ldap);

    auto &ldap_r = p.addTarget<Library>("ldap_r");
    //ldap_r.setSourceDirectory("3");
    {
        auto &t = ldap_r;
        add_ldap(t);

        t += "LDAP_R_COMPILE"_def;

        t += "libraries/libldap_r/.*"_rr;
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            t += "HAVE_NT_THREADS"_def;
        else
            t += "HAVE_PTHREADS=10"_def;

        t.Public += lber;
    }

    auto &ldapcpp = p.addTarget<StaticLibrary>("ldapcpp");
    {
        auto &t = ldapcpp;
        t.setRootDirectory("contrib/ldapc++");
        t.Public += ldap;
        t.writeFileOnce(t.BinaryPrivateDir / "config.h", R"(#ifdef _WIN32
#include <winsock.h>
#endif
)");

        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            t += "WIN32_LEAN_AND_MEAN"_def;
            t += "NOMINMAX"_def;
            t.writeFileOnce(t.BinaryPrivateDir / "unistd.h", R"(
#include <sys/stat.h>

#if defined (_S_IFDIR) && !defined (S_IFDIR)
#define S_IFDIR _S_IFDIR
#endif

#if !defined S_ISDIR && defined S_IFDIR
# define S_ISDIR(mode) (((mode) & S_IFMT) == S_IFDIR)
#endif
)");

            t.patch("src/LDAPRequest.h", "static const int DELETE=10;", "#undef DELETE\nstatic const int DELETE=10;");
            t.patch("src/LdifReader.cpp", "char outbuf[value.size()];", "std::string outbuf(value.size(), 0);");
            t.patch("src/LdifReader.cpp", "outbuf, value.size()", "(char *)outbuf.data(), value.size()");
        }

        t.Public += "org.sw.demo.cyrus.sasl"_dep;
    }

    return;

    auto &lunicode = p.addTarget<Library>("lunicode");
    //lunicode.setSourceDirectory("4");
    {
        auto &t = lunicode;
        t -= "libraries/liblunicode/.*"_rr;
        t += "libraries/liblunicode/ucstr.c";
        t += "libraries/liblunicode/ucdata/ucdata.c";

        //t.Public += sw::Shared, "LDAP_LIBS_DYNAMIC"_def;
        t += "LDAP_API="_def;

        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            t.writeFileOnce(t.BinaryPrivateDir / "unistd.h", "");

        t.Public += lber, ldap;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("ldap");
    s.checkFunctionExists("bcopy");
    s.checkFunctionExists("chroot");
    s.checkFunctionExists("closesocket");
    s.checkFunctionExists("crypt");
    s.checkFunctionExists("ctime_r");
    s.checkFunctionExists("endgrent");
    s.checkFunctionExists("endpwent");
    s.checkFunctionExists("fcntl");
    s.checkFunctionExists("flock");
    s.checkFunctionExists("fstat");
    s.checkFunctionExists("gai_strerror");
    s.checkFunctionExists("getaddrinfo");
    s.checkFunctionExists("getdtablesize");
    s.checkFunctionExists("geteuid");
    s.checkFunctionExists("getgrgid");
    s.checkFunctionExists("gethostbyaddr_r");
    s.checkFunctionExists("gethostbyname_r");
    s.checkFunctionExists("gethostname");
    s.checkFunctionExists("getnameinfo");
    s.checkFunctionExists("getpassphrase");
    s.checkFunctionExists("getpeerucred");
    s.checkFunctionExists("getpwnam");
    s.checkFunctionExists("getpwuid");
    s.checkFunctionExists("getspnam");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("gmtime_r");
    s.checkFunctionExists("hstrerror");
    s.checkFunctionExists("inet_ntoa_b");
    s.checkFunctionExists("inet_ntop");
    s.checkFunctionExists("initgroups");
    s.checkFunctionExists("ioctl");
    s.checkFunctionExists("localtime_r");
    s.checkFunctionExists("lockf");
    s.checkFunctionExists("memcmp");
    s.checkFunctionExists("memcpy");
    s.checkFunctionExists("memmove");
    s.checkFunctionExists("memrchr");
    s.checkFunctionExists("mkstemp");
    s.checkFunctionExists("mktemp");
    s.checkFunctionExists("nanosleep");
    s.checkFunctionExists("openlog");
    s.checkFunctionExists("pipe");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("pthread_getconcurrency");
    s.checkFunctionExists("pthread_kill");
    s.checkFunctionExists("pthread_setconcurrency");
    s.checkFunctionExists("pthread_yield");
    s.checkFunctionExists("read");
    s.checkFunctionExists("recv");
    s.checkFunctionExists("recvfrom");
    s.checkFunctionExists("sasl_version");
    s.checkFunctionExists("sched_yield");
    s.checkFunctionExists("select");
    s.checkFunctionExists("select_argtypes");
    s.checkFunctionExists("send");
    s.checkFunctionExists("sendmsg");
    s.checkFunctionExists("sendto");
    s.checkFunctionExists("setegid");
    s.checkFunctionExists("seteuid");
    s.checkFunctionExists("setgid");
    s.checkFunctionExists("setproctitle");
    s.checkFunctionExists("setpwfile");
    s.checkFunctionExists("setsid");
    s.checkFunctionExists("setuid");
    s.checkFunctionExists("sigaction");
    s.checkFunctionExists("signal");
    s.checkFunctionExists("sigset");
    s.checkFunctionExists("socket");
    s.checkFunctionExists("strdup");
    s.checkFunctionExists("strftime");
    s.checkFunctionExists("strpbrk");
    s.checkFunctionExists("strrchr");
    s.checkFunctionExists("strsep");
    s.checkFunctionExists("strspn");
    s.checkFunctionExists("strstr");
    s.checkFunctionExists("strtol");
    s.checkFunctionExists("strtoll");
    s.checkFunctionExists("strtoq");
    s.checkFunctionExists("strtoul");
    s.checkFunctionExists("strtoull");
    s.checkFunctionExists("strtouq");
    s.checkFunctionExists("sysconf");
    s.checkFunctionExists("thr_getconcurrency");
    s.checkFunctionExists("thr_setconcurrency");
    s.checkFunctionExists("thr_yield");
    s.checkFunctionExists("vprintf");
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("wait4");
    s.checkFunctionExists("waitpid");
    s.checkFunctionExists("write");
    s.checkFunctionExists("_snprintf");
    s.checkFunctionExists("_spawnlp");
    s.checkFunctionExists("_vsnprintf");
    s.checkIncludeExists("arpa/inet.h");
    s.checkIncludeExists("arpa/nameser.h");
    s.checkIncludeExists("assert.h");
    s.checkIncludeExists("bits/types.h");
    s.checkIncludeExists("conio.h");
    s.checkIncludeExists("crypt.h");
    s.checkIncludeExists("direct.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("filio.h");
    s.checkIncludeExists("getopt.h");
    s.checkIncludeExists("gmp.h");
    s.checkIncludeExists("gnutls/gnutls.h");
    s.checkIncludeExists("grp.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("libutil.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("ltdl.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("netinet/tcp.h");
    s.checkIncludeExists("nssutil.h");
    s.checkIncludeExists("openssl/bn.h");
    s.checkIncludeExists("openssl/crypto.h");
    s.checkIncludeExists("openssl/ssl.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("process.h");
    s.checkIncludeExists("psap.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("pth.h");
    s.checkIncludeExists("pwd.h");
    s.checkIncludeExists("regex.h");
    s.checkIncludeExists("resolv.h");
    s.checkIncludeExists("sasl.h");
    s.checkIncludeExists("sasl/sasl.h");
    s.checkIncludeExists("sched.h");
    s.checkIncludeExists("sgtty.h");
    s.checkIncludeExists("shadow.h");
    s.checkIncludeExists("slp.h");
    s.checkIncludeExists("sqlext.h");
    s.checkIncludeExists("sql.h");
    s.checkIncludeExists("stddef.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("synch.h");
    s.checkIncludeExists("sysexits.h");
    s.checkIncludeExists("syslog.h");
    s.checkIncludeExists("sys/devpoll.h");
    s.checkIncludeExists("sys/epoll.h");
    s.checkIncludeExists("sys/errno.h");
    s.checkIncludeExists("sys/event.h");
    s.checkIncludeExists("sys/file.h");
    s.checkIncludeExists("sys/filio.h");
    s.checkIncludeExists("sys/fstyp.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/poll.h");
    s.checkIncludeExists("sys/privgrp.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/syslog.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/timeb.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/ucred.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("sys/un.h");
    s.checkIncludeExists("sys/uuid.h");
    s.checkIncludeExists("sys/vmount.h");
    s.checkIncludeExists("sys/wait.h");
    s.checkIncludeExists("tcpd.h");
    s.checkIncludeExists("termios.h");
    s.checkIncludeExists("thread.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("utime.h");
    s.checkIncludeExists("uuid/uuid.h");
    {
        auto &c = s.checkIncludeExists("winsock2.h");
        c.Parameters.Includes.push_back("windows.h");
        c.Definitions.insert("HAVE_WINSOCK2");
    }
    {
        auto &c = s.checkIncludeExists("winsock.h");
        c.Parameters.Includes.push_back("windows.h");
        c.Definitions.insert("HAVE_WINSOCK");
    }
    s.checkIncludeExists("windows.h");
    s.checkIncludeExists("wiredtiger.h");
    {
        auto &c = s.checkDeclarationExists("sys_errlist");
        c.Definitions.insert("HAVE_SYS_ERRLIST");
        //c.Parameters.IncludeDirectories.push_back("stdio.h");
        //c.Parameters.IncludeDirectories.push_back("errno.h");
    }
    s.checkTypeSize("int");
    s.checkTypeSize("long");
    s.checkTypeSize("mode_t");
    s.checkTypeSize("off_t");
    s.checkTypeSize("pid_t");
    s.checkTypeSize("ptrdiff_t");
    s.checkTypeSize("short");
    s.checkTypeSize("signal");
    s.checkTypeSize("sig_atomic_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("socklen_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("uid_t");
    s.checkTypeSize("wchar_t");
    s.checkLibraryFunctionExists("s", "afopen");
    s.checkLibraryFunctionExists("be", "be_app");
    s.checkLibraryFunctionExists("crypt", "crypt");
    s.checkLibraryFunctionExists("crypt", "crypt_r");
    s.checkLibraryFunctionExists("gnutls", "gnutls_init");
    s.checkLibraryFunctionExists("ltdl", "lt_dlinit");
    s.checkLibraryFunctionExists("socket", "main");
    s.checkLibraryFunctionExists("ndbclient", "ndb_init");
    s.checkLibraryFunctionExists("nss3", "NSS_Initialize");
    s.checkLibraryFunctionExists("pth", "pth_version");
    s.checkLibraryFunctionExists("sasl2", "sasl_client_init");
    s.checkLibraryFunctionExists("rt", "sched_yield");
    s.checkLibraryFunctionExists("util", "setproctitle");
    s.checkLibraryFunctionExists("V3", "sigset");
    s.checkLibraryFunctionExists("slp", "SLPOpen");
    s.checkLibraryFunctionExists("net", "socket");
    s.checkLibraryFunctionExists("iodbc", "SQLDriverConnect");
    s.checkLibraryFunctionExists("ssl", "ssl3_accept");
    s.checkLibraryFunctionExists("ssl", "SSL_CTX_set_msg_callback");
    s.checkLibraryFunctionExists("thread", "thr_create");
    s.checkLibraryFunctionExists("wiredtiger", "wiredtiger_version");
    s.checkLibraryFunctionExists("gmp", "__gmpz_add_ui");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct msghdr", "msg_accrightslen");
    }
    {
        auto &c = s.checkStructMemberExists("struct msghdr", "msg_control");
    }
    {
        auto &c = s.checkStructMemberExists("struct passwd", "pw_gecos");
    }
    {
        auto &c = s.checkStructMemberExists("struct passwd", "pw_passwd");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_blksize");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_fstype");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkStructMemberExists("struct stat", "st_vfstype");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    s.checkSourceCompiles("HAVE_PTHREAD_RWLOCK_DESTROY", R"sw_xxx(
    #include <pthread.h>
    pthread_rwlock_t rwlock;


     int main() {

    pthread_rwlock_destroy(&rwlock);

     ; return 0; })sw_xxx");
    s.checkSourceCompiles("HAVE_STRUCT_STAT_ST_FSTYPE_CHAR", R"sw_xxx(struct stat st; char *ptr=st.st_fstype;)sw_xxx");
    s.checkSourceCompiles("HAVE_TIME_WITH_SYS_TIME", R"sw_xxx(
    #include <time.h>
    #include <sys/time.h>
    int main() {return 0;}
    )sw_xxx");
    s.checkSourceCompiles("TM_IN_SYS_TIME", R"sw_xxx(
    #include <time.h>
    int main() { struct tm t; return 0; }
    )sw_xxx");
}
