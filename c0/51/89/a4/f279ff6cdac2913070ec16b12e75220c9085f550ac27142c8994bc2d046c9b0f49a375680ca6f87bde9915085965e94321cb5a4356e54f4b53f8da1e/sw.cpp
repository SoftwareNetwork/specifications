void build(Solution &s)
{
    auto &llvm_project = s.addProject("llvm_project", "master");

    auto &llvm = llvm_project.addProject("llvm");
    llvm += Git("https://git.llvm.org/git/llvm.git", "", "{v}");

    auto &llvm_demangle = llvm.addTarget<StaticLibraryTarget>("demangle");
    {
        llvm_demangle +=
            "include/llvm/Demangle/.*"_rr,
            "lib/Demangle/.*\\.cpp"_rr,
            "lib/Demangle/.*\\.h"_rr;
    }

    auto &llvm_support_lite = llvm.addTarget<StaticLibraryTarget>("support_lite");
    {
        llvm_support_lite.setChecks("support_lite");
        llvm_support_lite +=
            "include/llvm-c/.*Types\\.h"_rr,
            "include/llvm-c/ErrorHandling.h",
            "include/llvm-c/Support.h",
            "include/llvm/ADT/.*\\.h"_rr,
            "include/llvm/Config/.*\\.cmake"_rr,
            "include/llvm/Support/.*"_rr,
            "lib/Support/.*\\.c"_rr,
            "lib/Support/.*\\.cpp"_rr,
            "lib/Support/.*\\.h"_rr,
            "lib/Support/.*\\.inc"_rr;
        llvm_support_lite -=
            "include/llvm/Support/.*def"_rr;
        llvm_support_lite.Private +=
            "lib"_id;
        llvm_support_lite.Public +=
            "include"_id;
        if (s.Settings.TargetOS.Type != OSType::Windows)
            llvm_support_lite.Private += "HAVE_PTHREAD_GETSPECIFIC"_d;
        llvm_support_lite.Public += llvm_demangle;

        llvm_support_lite += "LLVM_ENABLE_THREADS=1"_v;
        llvm_support_lite += "LLVM_HAS_ATOMICS=1"_v;
        if (s.Settings.TargetOS.Type == OSType::Windows)
            llvm_support_lite += "LLVM_HOST_TRIPLE=\"unknown-unknown-windows\""_v;
        else
        {
            llvm_support_lite += "LLVM_HOST_TRIPLE=\"unknown-unknown-unknown\""_v;
            llvm_support_lite += "LLVM_ON_UNIX=1"_v;
        }
        llvm_support_lite += "RETSIGTYPE=void"_v;

        llvm_support_lite += "LLVM_VERSION_MAJOR=0"_v;
        llvm_support_lite += "LLVM_VERSION_MINOR=0"_v;
        llvm_support_lite += "LLVM_VERSION_PATCH=1"_v;

        llvm_support_lite.configureFile("include/llvm/Config/config.h.cmake", "llvm/Config/config.h");
        llvm_support_lite.configureFile("include/llvm/Config/llvm-config.h.cmake", "llvm/Config/llvm-config.h");
        llvm_support_lite.configureFile("include/llvm/Config/abi-breaking.h.cmake", "llvm/Config/abi-breaking.h");
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("support_lite");
    s.checkFunctionExists("_alloca");
    s.checkFunctionExists("__alloca");
    s.checkFunctionExists("__ashldi3");
    s.checkFunctionExists("__ashrdi3");
    s.checkFunctionExists("__chkstk");
    s.checkFunctionExists("__chkstk_ms");
    s.checkFunctionExists("__cmpdi2");
    s.checkFunctionExists("__divdi3");
    s.checkFunctionExists("__fixdfdi");
    s.checkFunctionExists("__fixsfdi");
    s.checkFunctionExists("__floatdidf");
    s.checkFunctionExists("__lshrdi3");
    s.checkFunctionExists("__main");
    s.checkFunctionExists("__moddi3");
    s.checkFunctionExists("__udivdi3");
    s.checkFunctionExists("__umoddi3");
    s.checkFunctionExists("___chkstk");
    s.checkFunctionExists("___chkstk_ms");
    s.checkIncludeExists("CrashReporterClient.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("fenv.h");
    s.checkIncludeExists("histedit.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("link.h");
    s.checkIncludeExists("linux/magic.h");
    s.checkIncludeExists("linux/nfs_fs.h");
    s.checkIncludeExists("linux/smb.h");
    s.checkIncludeExists("mach/mach.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("malloc/malloc.h");
    s.checkIncludeExists("ndir.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("signal.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("sys/dir.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/ndir.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/uio.h");
    s.checkIncludeExists("termios.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("unwind.h");
    s.checkIncludeExists("valgrind/valgrind.h");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("uint64_t");
    s.checkTypeSize("u_int64_t");
    s.checkTypeSize("void *");
    {
        auto &c = s.checkSymbolExists("dladdr");
        c.Parameters.Includes.push_back("dlfcn.h");
    }
    {
        auto &c = s.checkSymbolExists("dlopen");
        c.Parameters.Includes.push_back("dlfcn.h");
    }
    {
        auto &c = s.checkSymbolExists("futimens");
        c.Parameters.Includes.push_back("sys/stat.h");
    }
    {
        auto &c = s.checkSymbolExists("futimes");
        c.Parameters.Includes.push_back("sys/time.h");
    }
    {
        auto &c = s.checkSymbolExists("getcwd");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("getpagesize");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("getrlimit");
        c.Parameters.Includes.push_back("sys/types.h");
        c.Parameters.Includes.push_back("sys/time.h");
        c.Parameters.Includes.push_back("sys/resource.h");
    }
    {
        auto &c = s.checkSymbolExists("getrusage");
        c.Parameters.Includes.push_back("sys/resource.h");
    }
    {
        auto &c = s.checkSymbolExists("gettimeofday");
        c.Parameters.Includes.push_back("sys/time.h");
    }
    {
        auto &c = s.checkSymbolExists("isatty");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("lseek64");
        c.Parameters.Includes.push_back("sys/types.h");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("mallctl");
        c.Parameters.Includes.push_back("malloc_np.h");
    }
    {
        auto &c = s.checkSymbolExists("mallinfo");
        c.Parameters.Includes.push_back("malloc.h");
    }
    {
        auto &c = s.checkSymbolExists("malloc_zone_statistics");
        c.Parameters.Includes.push_back("malloc/malloc.h");
    }
    {
        auto &c = s.checkSymbolExists("mkdtemp");
        c.Parameters.Includes.push_back("stdlib.h");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("mkstemp");
        c.Parameters.Includes.push_back("stdlib.h");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("mktemp");
        c.Parameters.Includes.push_back("stdlib.h");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("posix_fallocate");
        c.Parameters.Includes.push_back("fcntl.h");
    }
    {
        auto &c = s.checkSymbolExists("posix_spawn");
        c.Parameters.Includes.push_back("spawn.h");
    }
    {
        auto &c = s.checkSymbolExists("pread");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("realpath");
        c.Parameters.Includes.push_back("stdlib.h");
    }
    {
        auto &c = s.checkSymbolExists("sbrk");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("setenv");
        c.Parameters.Includes.push_back("stdlib.h");
    }
    {
        auto &c = s.checkSymbolExists("setrlimit");
        c.Parameters.Includes.push_back("sys/resource.h");
    }
    {
        auto &c = s.checkSymbolExists("sigaltstack");
        c.Parameters.Includes.push_back("signal.h");
    }
    {
        auto &c = s.checkSymbolExists("strerror");
        c.Parameters.Includes.push_back("string.h");
    }
    {
        auto &c = s.checkSymbolExists("strerror_r");
        c.Parameters.Includes.push_back("string.h");
    }
    {
        auto &c = s.checkSymbolExists("strtoll");
        c.Parameters.Includes.push_back("stdlib.h");
    }
    {
        auto &c = s.checkSymbolExists("sysconf");
        c.Parameters.Includes.push_back("unistd.h");
    }
    {
        auto &c = s.checkSymbolExists("writev");
        c.Parameters.Includes.push_back("sys/uio.h");
    }
    {
        auto &c = s.checkSymbolExists("_chsize_s");
        c.Parameters.Includes.push_back("io.h");
    }
    {
        auto &c = s.checkSymbolExists("_Unwind_Backtrace");
        c.Parameters.Includes.push_back("unwind.h");
    }
    {
        auto &c = s.checkSymbolExists("__GLIBC__");
        c.Parameters.Includes.push_back("stdio.h");
    }
}
