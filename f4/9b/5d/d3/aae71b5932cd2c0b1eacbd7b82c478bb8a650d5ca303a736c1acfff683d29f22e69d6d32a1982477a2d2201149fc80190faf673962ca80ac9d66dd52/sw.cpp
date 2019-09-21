struct PythonExecutable : ExecutableTarget
{
    void setupCommand(builder::Command &c) const override
    {
        ExecutableTarget::setupCommand(c);
        c.environment["PYTHONPATH"] = (SourceDir / "Lib").u8string();
    }

    void setupCommandForRun(builder::Command &c) const override
    {
        setupCommand(c);
        c.create_new_console = true;
    }
};

void build(Solution &s)
{
    auto &python = s.addProject("python", "2.7.15");
    python += Git("https://github.com/python/cpython", "v{v}");

    auto &lib = python.addTarget<SharedLibraryTarget>("lib");
    {
        lib.setChecks("lib");
        lib +=
            "Include/.*"_rr,
            "Modules/.*\\.h"_rr,
            "Modules/_bisectmodule.c",
            "Modules/_codecsmodule.c",
            "Modules/_collectionsmodule.c",
            "Modules/_csv.c",
            "Modules/_elementtree.c",
            "Modules/_functoolsmodule.c",
            "Modules/_heapqmodule.c",
            "Modules/_hotshot.c",
            "Modules/_io/.*\\.c"_rr,
            "Modules/_io/.*\\.h"_rr,
            "Modules/_json.c",
            "Modules/_localemodule.c",
            "Modules/_lsprof.c",
            "Modules/_math.c",
            "Modules/_randommodule.c",
            "Modules/_sre.c",
            "Modules/_struct.c",
            "Modules/_weakref.c",
            "Modules/arraymodule.c",
            //"Modules/atexitmodule.c",
            "Modules/audioop.c",
            "Modules/binascii.c",
            "Modules/cjkcodecs/_codecs_cn.c",
            "Modules/cjkcodecs/_codecs_hk.c",
            "Modules/cjkcodecs/_codecs_iso2022.c",
            "Modules/cjkcodecs/_codecs_jp.c",
            "Modules/cjkcodecs/_codecs_kr.c",
            "Modules/cjkcodecs/_codecs_tw.c",
            "Modules/cjkcodecs/multibytecodec.c",
            "Modules/cmathmodule.c",
            "Modules/config.c.in",
            "Modules/cPickle.c",
            "Modules/cStringIO.c",
            "Modules/datetimemodule.c",
            "Modules/errnomodule.c",
            "Modules/future_builtins.c",
            "Modules/gcmodule.c",
            "Modules/getbuildinfo.c",
            "Modules/getpath.c",
            "Modules/imageop.c",
            "Modules/itertoolsmodule.c",
            "Modules/main.c",
            "Modules/mathmodule.c",
            "Modules/md5.*"_rr,
            "Modules/mmapmodule.c",
            "Modules/operator.c",
            "Modules/parsermodule.c",
            "Modules/posixmodule.c",
            "Modules/pwdmodule.c",
            "Modules/pyexpat.c",
            "Modules/rotatingtree.c",
            "Modules/sha.*"_rr,
            "Modules/signalmodule.c",
            "Modules/stropmodule.c",
            "Modules/symtablemodule.c",
            "Modules/timemodule.c",
            "Modules/threadmodule.c",
            "Modules/xxsubtype.c",
            "Modules/zipimport.c",
            "Modules/zlibmodule.c",
            "Objects/.*\\.h"_rr,
            "Objects/abstract.c",
            "Objects/boolobject.c",
            "Objects/bufferobject.c",
            "Objects/bytearrayobject.c",
            //"Objects/bytesobject.c",
            "Objects/bytes_methods.c",
            "Objects/capsule.c",
            "Objects/cellobject.c",
            "Objects/classobject.c",
            "Objects/cobject.c",
            "Objects/codeobject.c",
            "Objects/complexobject.c",
            "Objects/descrobject.c",
            "Objects/dictobject.c",
            "Objects/enumobject.c",
            "Objects/exceptions.c",
            "Objects/fileobject.c",
            "Objects/floatobject.c",
            "Objects/frameobject.c",
            "Objects/funcobject.c",
            "Objects/genobject.c",
            "Objects/intobject.c",
            "Objects/iterobject.c",
            "Objects/listobject.c",
            "Objects/longobject.c",
            "Objects/memoryobject.c",
            "Objects/methodobject.c",
            "Objects/moduleobject.c",
            "Objects/object.c",
            "Objects/obmalloc.c",
            "Objects/rangeobject.c",
            "Objects/setobject.c",
            "Objects/sliceobject.c",
            "Objects/stringobject.c",
            "Objects/structseq.c",
            "Objects/tupleobject.c",
            "Objects/typeobject.c",
            "Objects/unicodectype.c",
            "Objects/unicodeobject.c",
            "Objects/weakrefobject.c",
            "PC/.*\\.h"_rr,
            "PC/_subprocess.c",
            "PC/config.c",
            "PC/getpathp.c",
            "PC/import_nt.c",
            "PC/msvcrtmodule.c",
            "Parser/.*\\.h"_rr,
            "Parser/acceler.c",
            "Parser/bitset.c",
            "Parser/firstsets.c",
            "Parser/grammar.c",
            "Parser/grammar1.c",
            "Parser/listnode.c",
            "Parser/metagrammar.c",
            "Parser/myreadline.c",
            "Parser/node.c",
            "Parser/parser.c",
            "Parser/parsetok.c",
            "Parser/tokenizer.c",
            "Python/.*\\.h"_rr,
            "Python/Python-ast.c",
            "Python/_warnings.c",
            "Python/asdl.c",
            "Python/ast.c",
            "Python/bltinmodule.c",
            "Python/ceval.c",
            "Python/codecs.c",
            "Python/compile.c",
            "Python/dtoa.c",
            "Python/dynload_.*"_rr,
            "Python/errors.c",
            "Python/formatter_string.c",
            "Python/formatter_unicode.c",
            "Python/frozen.c",
            "Python/future.c",
            "Python/getargs.c",
            "Python/getcompiler.c",
            "Python/getcopyright.c",
            "Python/getopt.c",
            "Python/getplatform.c",
            "Python/getversion.c",
            "Python/graminit.c",
            "Python/import.c",
            "Python/importdl.c",
            "Python/marshal.c",
            "Python/modsupport.c",
            "Python/mysnprintf.c",
            "Python/mystrtoul.c",
            "Python/peephole.c",
            "Python/pyarena.c",
            "Python/pyctype.c",
            "Python/pyfpe.c",
            "Python/pymath.c",
            "Python/pystate.c",
            "Python/pystrcmp.c",
            "Python/pystrtod.c",
            "Python/pythonrun.c",
            "Python/random.c",
            "Python/structmember.c",
            "Python/symtable.c",
            "Python/sysmodule.c",
            "Python/thread.c",
            "Python/traceback.c";

        lib -= "Python/dynload_.*"_rr;

        lib.Public +=
            "Python"_id;
        lib.Public +=
            "PC"_id;
        lib.Public +=
            "Include"_id;

        lib.Private += "EXEC_PREFIX=\"\""_d;
        lib.Private += "PREFIX=\"\""_d;
        lib.Private += "Py_BUILD_CORE"_d;
        lib.Private += "VERSION=\"${PACKAGE_VERSION_MAJOR}.${PACKAGE_VERSION_MINOR}\""_d;
        lib.Private += "VPATH=\"\""_d;
        lib.Public += "ABIFLAGS=\"\""_d;
        lib.Public += "ENABLE_IPV6"_d;
        lib.Public += sw::Shared, "HAVE_DYNAMIC_LOADING"_d;
        lib.Public += "HAVE_GCC_UINT128_T=HAVE___UINT128_T"_d;
        lib.Public += "HAVE_STDARG_PROTOTYPES"_d;
        lib.Public += "HAVE_ZLIB_COPY"_d;
        lib.Public += "PLATFORM=\"\""_d;
        lib.Public += "RETSIGTYPE=void"_d;
        lib.Public += "SOABI=\"\""_d;
        lib.Public += "STDC_HEADERS=1"_d;
        lib.Public += "USE_INLINE"_d;
        lib.Public += "WITH_DOC_STRINGS=1"_d;
        lib.Public += "WITH_PYMALLOC"_d;
        lib += "_Py_HAVE_ZLIB"_d;
        if (lib.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            lib -= "Modules/pwdmodule.c";
            lib -= "Modules/getpath.c";
            lib += "Python/dynload_win.c";
            lib += "PC/_winreg.c";
            lib += "PC/dl_nt.c";

            //lib += "timezone=_timezone"_d;
            lib += "tzname=_tzname"_d;
            lib += "daylight=_daylight"_d;

            lib.Private += "PYTHONPATH=L\"\""_d;
            lib.Public += "MS_WINDOWS"_d;
            lib.Public += "NT_THREADS"_d;
            lib.Public += "WIN32"_d;
            if (lib.getBuildSettings().TargetOS.Arch == ArchType::x86_64)
                lib.Public += "MS_WINI64"_d;

            lib.Public += "advapi32.lib"_lib;
            lib.Public += "Mincore.lib"_lib;
            lib.Public += "Shlwapi.lib"_lib;
            lib.Public += "Shell32.lib"_lib;
            lib.Public += "User32.lib"_lib;

            lib.replaceInFileOnce("Modules/timemodule.c", "timezone);", "_timezone );");
            lib.replaceInFileOnce("Modules/timemodule.c", "timezone-3600", "_timezone -3600");

            lib.replaceInFileOnce("Modules/posixmodule.c", "const int i1 = fd >> IOINFO_L2E;", "return 1;\n}\n#if 0\nconst int i1 = fd >> IOINFO_L2E;");
            lib.replaceInFileOnce("Modules/posixmodule.c", "/* the special case of checking dup2.", "#endif\n/* the special case of checking dup2.");
        }
        else
        {
            lib.Public += "PY_FORMAT_LONG_LONG=\"ll\""_d;
            lib.Public += "PY_FORMAT_SIZE_T=\"z\""_d;
            lib.Public += "WITH_THREAD"_d;
            lib.Public += "_ALL_SOURCE=1"_d;
            lib.Public += "_BSD_TYPES"_d;
            lib.Public += "_DARWIN_C_SOURCE"_d;
            lib.Public += "_FILE_OFFSET_BITS=64"_d;
            lib.Public += "_GNU_SOURCE=1"_d;
            lib.Public += "_LARGEFILE_SOURCE=1"_d;
            lib.Public += "_NETBSD_SOURCE"_d;
            lib.Public += "_POSIX_PTHREAD_SEMANTICS=1"_d;
            lib.Public += "_POSIX_THREADS"_d;
            lib.Public += "_TANDEM_SOURCE=1"_d;
            lib.Public += "__BSD_VISIBLE"_d;
            lib.Public += "__EXTENSIONS__=1"_d;
        }
        lib.Public += sw::Static, "Py_NO_ENABLE_SHARED"_d;
        lib.Public += sw::Shared, "Py_ENABLE_SHARED"_d;

        lib.Public += "org.sw.demo.madler.zlib-1"_dep;
        lib.Public += "org.sw.demo.expat-2"_dep;

        lib.replaceInFileOnce("PC/pyconfig.h", "#ifdef MS_COREDLL", "#if 0");

        lib.replaceInFileOnce("PC/config.c", "/* -- ADDMODULE MARKER 1 -- */", R"xxx(
            extern void initpyexpat(void);
)xxx");
        lib.replaceInFileOnce("PC/config.c", "/* -- ADDMODULE MARKER 2 -- */", R"xxx(
            {"pyexpat", initpyexpat},
)xxx");

    }

    auto &exe = python.addTarget<PythonExecutable>("exe");
    exe += "Lib/.*"_rr;
    exe -= ".*\\.def"_rr;
    exe += "Modules/python.c";
    exe.Public += lib;
}

void check(Checker &c)
{
    auto &s = c.addSet("lib");
    s.checkFunctionExists("accept4");
    s.checkFunctionExists("acosh");
    s.checkFunctionExists("alarm");
    s.checkFunctionExists("asinh");
    s.checkFunctionExists("atanh");
    s.checkFunctionExists("bind_textdomain_codeset");
    s.checkFunctionExists("chflags");
    s.checkFunctionExists("chown");
    s.checkFunctionExists("chroot");
    s.checkFunctionExists("clock");
    s.checkFunctionExists("clock_getres");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("confstr");
    s.checkFunctionExists("copysign");
    s.checkFunctionExists("ctermid");
    s.checkFunctionExists("ctermid_r");
    s.checkFunctionExists("dirfd");
    s.checkFunctionExists("dup2");
    s.checkFunctionExists("dup3");
    s.checkFunctionExists("epoll");
    s.checkFunctionExists("epoll_create1");
    s.checkFunctionExists("erf");
    s.checkFunctionExists("erfc");
    s.checkFunctionExists("execv");
    s.checkFunctionExists("expm1");
    s.checkFunctionExists("faccessat");
    s.checkFunctionExists("fchdir");
    s.checkFunctionExists("fchmod");
    s.checkFunctionExists("fchmodat");
    s.checkFunctionExists("fchown");
    s.checkFunctionExists("fchownat");
    s.checkFunctionExists("fdopendir");
    s.checkFunctionExists("fexecve");
    s.checkFunctionExists("finite");
    s.checkFunctionExists("flock");
    s.checkFunctionExists("flockfile");
    s.checkFunctionExists("fork");
    s.checkFunctionExists("forkpty");
    s.checkFunctionExists("fpathconf");
    s.checkFunctionExists("fseek64");
    s.checkFunctionExists("fseeko");
    s.checkFunctionExists("fstatat");
    s.checkFunctionExists("fstatvfs");
    s.checkFunctionExists("fsync");
    s.checkFunctionExists("ftell64");
    s.checkFunctionExists("ftello");
    s.checkFunctionExists("ftime");
    s.checkFunctionExists("ftruncate");
    s.checkFunctionExists("funlockfile");
    s.checkFunctionExists("futimens");
    s.checkFunctionExists("futimes");
    s.checkFunctionExists("futimesat");
    s.checkFunctionExists("gai_strerror");
    s.checkFunctionExists("gamma");
    s.checkFunctionExists("getaddrinfo");
    s.checkFunctionExists("getc_unlocked");
    s.checkFunctionExists("getentropy");
    s.checkFunctionExists("getgrouplist");
    s.checkFunctionExists("getgroups");
    s.checkFunctionExists("gethostbyname");
    s.checkFunctionExists("gethostbyname_r");
    s.checkFunctionExists("getitimer");
    s.checkFunctionExists("getloadavg");
    s.checkFunctionExists("getlogin");
    s.checkFunctionExists("getnameinfo");
    s.checkFunctionExists("getpagesize");
    s.checkFunctionExists("getpeername");
    s.checkFunctionExists("getpgid");
    s.checkFunctionExists("getpgrp");
    s.checkFunctionExists("getpid");
    s.checkFunctionExists("getpriority");
    s.checkFunctionExists("getpwent");
    s.checkFunctionExists("getrandom");
    s.checkFunctionExists("getresgid");
    s.checkFunctionExists("getresuid");
    s.checkFunctionExists("getsid");
    s.checkFunctionExists("getspent");
    s.checkFunctionExists("getspnam");
    s.checkFunctionExists("gettimeofday");
    s.checkFunctionExists("getwd");
    s.checkFunctionExists("hstrerror");
    s.checkFunctionExists("htole64");
    s.checkFunctionExists("hypot");
    s.checkFunctionExists("if_nameindex");
    s.checkFunctionExists("inet_aton");
    s.checkFunctionExists("inet_pton");
    s.checkFunctionExists("initgroups");
    s.checkFunctionExists("isfinite");
    s.checkFunctionExists("isinf");
    s.checkFunctionExists("isnan");
    s.checkFunctionExists("is_term_resized");
    s.checkFunctionExists("kill");
    s.checkFunctionExists("killpg");
    s.checkFunctionExists("kqueue");
    s.checkFunctionExists("lchflags");
    s.checkFunctionExists("lchmod");
    s.checkFunctionExists("lchown");
    s.checkFunctionExists("le64toh");
    s.checkFunctionExists("lgamma");
    s.checkFunctionExists("link");
    s.checkFunctionExists("linkat");
    s.checkFunctionExists("lockf");
    s.checkFunctionExists("log1p");
    s.checkFunctionExists("log2");
    s.checkFunctionExists("lstat");
    s.checkFunctionExists("lutimes");
    s.checkFunctionExists("mbrtowc");
    s.checkFunctionExists("memmove");
    s.checkFunctionExists("memrchr");
    s.checkFunctionExists("mkdirat");
    s.checkFunctionExists("mkfifo");
    s.checkFunctionExists("mkfifoat");
    s.checkFunctionExists("mknod");
    s.checkFunctionExists("mknodat");
    s.checkFunctionExists("mktime");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("mremap");
    s.checkFunctionExists("nice");
    s.checkFunctionExists("openat");
    s.checkFunctionExists("openpty");
    s.checkFunctionExists("pathconf");
    s.checkFunctionExists("pause");
    s.checkFunctionExists("pipe2");
    s.checkFunctionExists("plock");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("posix_fadvise");
    s.checkFunctionExists("posix_fallocate");
    s.checkFunctionExists("pread");
    s.checkFunctionExists("prlimit");
    s.checkFunctionExists("pthread_atfork");
    s.checkFunctionExists("pthread_init");
    s.checkFunctionExists("pthread_kill");
    s.checkFunctionExists("pthread_sigmask");
    s.checkFunctionExists("putenv");
    s.checkFunctionExists("pwrite");
    s.checkFunctionExists("readlink");
    s.checkFunctionExists("readlinkat");
    s.checkFunctionExists("readv");
    s.checkFunctionExists("realpath");
    s.checkFunctionExists("renameat");
    s.checkFunctionExists("resizeterm");
    s.checkFunctionExists("resize_term");
    s.checkFunctionExists("round");
    s.checkFunctionExists("sched_get_priority_max");
    s.checkFunctionExists("sched_rr_get_interval");
    s.checkFunctionExists("sched_setaffinity");
    s.checkFunctionExists("sched_setparam");
    s.checkFunctionExists("sched_setscheduler");
    s.checkFunctionExists("select");
    s.checkFunctionExists("sem_getvalue");
    s.checkFunctionExists("sem_open");
    s.checkFunctionExists("sem_timedwait");
    s.checkFunctionExists("sem_unlink");
    s.checkFunctionExists("sendfile");
    s.checkFunctionExists("setegid");
    s.checkFunctionExists("seteuid");
    s.checkFunctionExists("setgid");
    s.checkFunctionExists("setgroups");
    s.checkFunctionExists("sethostname");
    s.checkFunctionExists("setitimer");
    s.checkFunctionExists("setlocale");
    s.checkFunctionExists("setpgid");
    s.checkFunctionExists("setpgrp");
    s.checkFunctionExists("setpriority");
    s.checkFunctionExists("setregid");
    s.checkFunctionExists("setresgid");
    s.checkFunctionExists("setresuid");
    s.checkFunctionExists("setreuid");
    s.checkFunctionExists("setsid");
    s.checkFunctionExists("setuid");
    s.checkFunctionExists("setvbuf");
    s.checkFunctionExists("sigaction");
    s.checkFunctionExists("sigaltstack");
    s.checkFunctionExists("siginterrupt");
    s.checkFunctionExists("sigpending");
    s.checkFunctionExists("sigrelse");
    s.checkFunctionExists("sigtimedwait");
    s.checkFunctionExists("sigwait");
    s.checkFunctionExists("sigwaitinfo");
    s.checkFunctionExists("socketpair");
    s.checkFunctionExists("statvfs");
    s.checkFunctionExists("strdup");
    s.checkFunctionExists("strftime");
    s.checkFunctionExists("strlcpy");
    s.checkFunctionExists("symlink");
    s.checkFunctionExists("symlinkat");
    s.checkFunctionExists("sync");
    s.checkFunctionExists("sysconf");
    s.checkFunctionExists("tcgetpgrp");
    s.checkFunctionExists("tcsetpgrp");
    s.checkFunctionExists("tempnam");
    s.checkFunctionExists("tgamma");
    s.checkFunctionExists("timegm");
    s.checkFunctionExists("times");
    s.checkFunctionExists("tmpfile");
    s.checkFunctionExists("tmpnam");
    s.checkFunctionExists("tmpnam_r");
    s.checkFunctionExists("truncate");
    s.checkFunctionExists("tzname");
    s.checkFunctionExists("uname");
    s.checkFunctionExists("unlinkat");
    s.checkFunctionExists("unsetenv");
    s.checkFunctionExists("utimensat");
    s.checkFunctionExists("utimes");
    s.checkFunctionExists("wait3");
    s.checkFunctionExists("wait4");
    s.checkFunctionExists("waitid");
    s.checkFunctionExists("waitpid");
    s.checkFunctionExists("wcscoll");
    s.checkFunctionExists("wcsftime");
    s.checkFunctionExists("wcsxfrm");
    s.checkFunctionExists("wmemcmp");
    s.checkFunctionExists("writev");
    s.checkFunctionExists("_getpty");
    s.checkIncludeExists("alloca.h");
    s.checkIncludeExists("asm/types.h");
    s.checkIncludeExists("bluetooth/bluetooth.h");
    s.checkIncludeExists("bluetooth.h");
    s.checkIncludeExists("conio.h");
    s.checkIncludeExists("curses.h");
    s.checkIncludeExists("direct.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("endian.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("grp.h");
    s.checkIncludeExists("ieeefp.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("io.h");
    s.checkIncludeExists("langinfo.h");
    s.checkIncludeExists("libintl.h");
    s.checkIncludeExists("libutil.h");
    s.checkIncludeExists("linux/can/bcm.h");
    s.checkIncludeExists("linux/can.h");
    s.checkIncludeExists("linux/can/raw.h");
    s.checkIncludeExists("linux/netlink.h");
    s.checkIncludeExists("linux/random.h");
    s.checkIncludeExists("linux/tipc.h");
    s.checkIncludeExists("memory.h");
    s.checkIncludeExists("ncurses.h");
    s.checkIncludeExists("ndir.h");
    s.checkIncludeExists("netpacket/packet.h");
    s.checkIncludeExists("net/if.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("process.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("pty.h");
    s.checkIncludeExists("sched.h");
    s.checkIncludeExists("shadow.h");
    s.checkIncludeExists("signal.h");
    s.checkIncludeExists("spawn.h");
    s.checkIncludeExists("stdatomic.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("stropts.h");
    s.checkIncludeExists("sysexits.h");
    s.checkIncludeExists("sys/audioio.h");
    s.checkIncludeExists("sys/bsdtty.h");
    s.checkIncludeExists("sys/devpoll.h");
    s.checkIncludeExists("sys/dir.h");
    s.checkIncludeExists("sys/endian.h");
    s.checkIncludeExists("sys/epoll.h");
    s.checkIncludeExists("sys/event.h");
    s.checkIncludeExists("sys/file.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/kern_control.h");
    s.checkIncludeExists("sys/loadavg.h");
    s.checkIncludeExists("sys/lock.h");
    s.checkIncludeExists("sys/mkdev.h");
    s.checkIncludeExists("sys/modem.h");
    s.checkIncludeExists("sys/ndir.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/poll.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/sendfile.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/statvfs.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/syscall.h");
    s.checkIncludeExists("sys/sys_domain.h");
    s.checkIncludeExists("sys/termio.h");
    s.checkIncludeExists("sys/times.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("sys/un.h");
    s.checkIncludeExists("sys/utsname.h");
    s.checkIncludeExists("sys/wait.h");
    s.checkIncludeExists("sys/xattr.h");
    s.checkIncludeExists("termios.h");
    s.checkIncludeExists("term.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("util.h");
    s.checkIncludeExists("utime.h");
    s.checkIncludeExists("wchar.h");
    s.checkTypeSize("double");
    s.checkTypeSize("float");
    s.checkTypeSize("fpos_t");
    s.checkTypeSize("int");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("long");
    s.checkTypeSize("long double");
    s.checkTypeSize("long long");
    s.checkTypeSize("off_t");
    s.checkTypeSize("pid_t");
    s.checkTypeSize("pthread_t");
    s.checkTypeSize("short");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("time_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("uintptr_t");
    s.checkTypeSize("void *");
    s.checkTypeSize("wchar_t");
    s.checkTypeSize("_Bool");
    s.checkTypeSize("__uint128_t");
    {
        auto &c = s.checkSymbolExists("snprintf");
        c.Parameters.Includes.push_back("stdio.h");
    }
    s.checkSourceCompiles("GETPGRP_HAVE_ARG", R"sw_xxx(#include <unistd.h>
    int main()
    {
        getpgrp(0);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("GETTIMEOFDAY_NO_TZ", R"sw_xxx(#include <sys/time.h>
    int main()
    {
        gettimeofday((struct timeval*)0,(struct timezone*)0);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_ADDRINFO", R"sw_xxx(#include <netdb.h>
    int main()
    {
        struct addrinfo a;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_ALTZONE", R"sw_xxx(#include <time.h>
    int main()
    {
        return altzone;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_BROKEN_UNSETENV", R"sw_xxx(#include <stdlib.h>
    int main()
    {
        int res = unsetenv("DUMMY");
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_BUILTIN_ATOMIC", R"sw_xxx(volatile int val = 1;
    int main() {
      __atomic_load_n(&val, __ATOMIC_SEQ_CST);
      return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_C99_BOOL", R"sw_xxx(int main()
    {
        _Bool x;
        x = (_Bool)0;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_DEVICE_MACROS", R"sw_xxx(#if defined(MAJOR_IN_MKDEV)
    #include <sys/mkdev.h>
    #elif defined(MAJOR_IN_SYSMACROS)
    #include <sys/sysmacros.h>
    #else
    #include <sys/types.h>
    #endif
    int main()
    {
        makedev(major(0),minor(0));
        return 0;
    }
    #ifdef _WIN32
    cannot compile
    #endif
    )sw_xxx");
    s.checkSourceCompiles("HAVE_DIRENT_D_TYPE", R"sw_xxx(#include <dirent.h>

    int main() {
      struct dirent entry;
      return entry.d_type == DT_UNKNOWN;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_FDATASYNC", R"sw_xxx(#include <unistd.h>
    int main()
    {
        void *x=fdatasync;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_GCC_ASM_FOR_MC68881", R"sw_xxx(int main()
    {  unsigned int fpcr;
      __asm__ __volatile__ ("fmove.l %%fpcr,%0" : "=g" (fpcr));
      __asm__ __volatile__ ("fmove.l %0,%%fpcr" : : "g" (fpcr));
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_GCC_ASM_FOR_X64", R"sw_xxx(int main()
    {
        __asm__ __volatile__ ("movq %rcx, %rax");
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_GCC_ASM_FOR_X87", R"sw_xxx(int main()
    {
          unsigned short cw;
      __asm__ __volatile__ ("fnstcw %0" : "=m" (cw));
      __asm__ __volatile__ ("fldcw %0" : : "m" (cw));
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_GETRANDOM_SYSCALL", R"sw_xxx(#include <unistd.h>
    #include <sys/syscall.h>
    #include <linux/random.h>

    int main() {
        char buffer[1];
        const size_t buflen = sizeof(buffer);
        const int flags = GRND_NONBLOCK;
        /* ignore the result, Python checks for ENOSYS and EAGAIN at runtime */
        (void)syscall(SYS_getrandom, buffer, buflen, flags);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_LINUX_CAN_RAW_FD_FRAMES", R"sw_xxx(#include <linux/can/raw.h>
    int main()
    {
        int can_raw_fd_frames = CAN_RAW_FD_FRAMES;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_MAKEDEV", R"sw_xxx(#if defined(MAJOR_IN_MKDEV)
    #include <sys/mkdev.h>
    #elif defined(MAJOR_IN_SYSMACROS)
    #include <sys/sysmacros.h>
    #else
    #include <sys/types.h>
    #endif
    int main()
    {
        makedev(0, 0);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_OSX105_SDK", R"sw_xxx(#include <Carbon/Carbon.h>
    int main()
    {
        FSIORefNum fRef = 0;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_PROTOTYPES", R"sw_xxx(int foo(int x) { return 0; }
    int main()
    {
        return foo(10);
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_SOCKADDR_SA_LEN", R"sw_xxx(#include <sys/types.h>
    #include <sys/socket.h>
    int main()
    {
        struct sockaddr x;
        x.sa_len = 0;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_SOCKADDR_STORAGE", R"sw_xxx(#           include <sys/types.h>
    #           include <sys/socket.h>
    int main()
    {
        struct sockaddr_storage s;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_STAT_TV_NSEC", R"sw_xxx(#include <sys/stat.h>
    int main()
    {
        struct stat st;
        st.st_mtim.tv_nsec = 1;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_STAT_TV_NSEC2", R"sw_xxx(#include <sys/stat.h>
    int main()
    {
        struct stat st;
        st.st_mtimespec.tv_nsec = 1;
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_STDARG_PROTOTYPES", R"sw_xxx(#include <stdarg.h>
    int foo(int x, ...) {
    va_list va;
    va_start(va, x);
    va_arg(va, int);
    va_arg(va, char *);
    va_arg(va, double);
    return 0;
    }
    int main()
    {
        return foo(10, "", 3.14);
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_STD_ATOMIC", R"sw_xxx(#include <stdatomic.h>
    atomic_int value = ATOMIC_VAR_INIT(1);
    _Atomic void *py_atomic_address = (void*) &value;
    int main() {
      int loaded_value = atomic_load(&value);
      return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_UCS4_TCL", R"sw_xxx(#include <tcl.h>
    #if TCL_UTF_MAX != 6
    # error "NOT UCS4_TCL"
    #endif
    int main()
    {
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("HAVE_WCHAR_T_SIGNED", R"sw_xxx(#include <wchar.h>
    int main()
    {
    /* Success: exit code 0 */
        exit((((wchar_t) -1) < ((wchar_t) 0)) ? 0 : 1);
    }
    )sw_xxx");
    s.checkSourceCompiles("MVWDELCH_IS_EXPRESSION", R"sw_xxx(#include <curses.h>
    int main()
    {
          int rtn;
        rtn = mvwdelch(0,0,0);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("SETPGRP_HAVE_ARG", R"sw_xxx(#include <unistd.h>
    int main()
    {
        setpgrp(0,0);
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("SYS_SELECT_WITH_SYS_TIME", R"sw_xxx(#include <sys/types.h>
    #include <sys/select.h>
    #include <sys/time.h>
    int main()
    {
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("TIME_WITH_SYS_TIME", R"sw_xxx(#include <time.h>
    #include <sys/time.h>
    int main()
    {
        return 0;
    }
    )sw_xxx");
    s.checkSourceCompiles("TM_IN_SYS_TIME", R"sw_xxx(
    #include <time.h>
    int main() { struct tm t; return 0; }
    )sw_xxx");

    s.checkSourceCompiles("VA_LIST_IS_ARRAY", R"sw_xxx(#if ${HAVE_STDARG_PROTOTYPES}
    #include <stdarg.h>
    #else
    #include <varargs.h>
    #endif
    int main()
    {
        va_list list1, list2; list1 = list2;
        return 0;
    }
    )sw_xxx");

    s.checkSourceCompiles("WINDOW_HAS_FLAGS", R"sw_xxx(#include <curses.h>
    int main()
    {
        WINDOW *w;
        w->_flags = 0;
        return 0;
    }
    )sw_xxx");
}
