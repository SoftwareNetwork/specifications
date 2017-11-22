void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &magick = s.addTarget<LibraryTarget>("graphicsmagick.magick", "1.3.26");
    magick.Source = RemoteFile("https://downloads.sourceforge.net/project/graphicsmagick/graphicsmagick/{v}/GraphicsMagick-{v}-windows-source.7z");
    //magick.fetch();

    magick.setChecks("magick");

    magick +=
        "coders/.*\\.[hc]"_rr,
        "filters/.*\\.[hc]"_rr,
        "magick/.*\\.[hc]"_rr,
        "magick/.*\\.in"_rr;

    magick.Public += "QuantumDepth=16"_d;
    magick.Public += "_MAGICKLIB_"_d;
    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
        magick += "restrict=__restrict"_d;
    if (s.Settings.TargetOS.Type != OSType::Windows)
    {
        magick.Public += "HAVE_PTHREAD"_d;
    }
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        magick.Public += "HAVE_CRYPTGENRANDOM"_d;
        magick.Public += "HAVE_WINCRYPT_H"_d;
        magick.Public += "MSWINDOWS"_d;
        magick.Public += "WIN32"_d;
        if (s.Settings.TargetOS.Arch == ArchType::x86_64)
            magick.Public += "WIN64"_d;
        magick.Public += "Advapi32.lib"_lib;
        magick.Public += "Gdi32.lib"_lib;
        magick.Public += "User32.lib"_lib;
    }
    magick.Public += sw::Shared, "_MAGICKDLL_"_d;

    magick.writeFileOnce("magick/magick_config.h", true);

    //#configure_file(${SDIR}/magick/magick_config.h.in ${BDIR}/magick/magick_config.h)
    magick.configureFile("magick/magick_config_api.h.in", "magick/magick_config_api.h");
    magick.configureFile("magick/magick_types.h.in", "magick/magick_types.h");
    magick.configureFile("magick/version.h.in", "magick/version.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("magick");
    s.checkFunctionExists("atoll");
    s.checkFunctionExists("clock_getres");
    s.checkFunctionExists("clock_gettime");
    s.checkFunctionExists("ctime_r");
    s.checkFunctionExists("fcntl");
    s.checkFunctionExists("fseeko");
    s.checkFunctionExists("fstatvfs");
    s.checkFunctionExists("ftime");
    s.checkFunctionExists("getc_unlocked");
    s.checkFunctionExists("getexecname");
    s.checkFunctionExists("getpagesize");
    s.checkFunctionExists("getpid");
    s.checkFunctionExists("getrlimit");
    s.checkFunctionExists("lltostr");
    s.checkFunctionExists("localtime_r");
    s.checkFunctionExists("madvise");
    s.checkFunctionExists("pclose");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("popen");
    s.checkFunctionExists("posix_fadvise");
    s.checkFunctionExists("posix_fallocate");
    s.checkFunctionExists("posix_madvise");
    s.checkFunctionExists("posix_memalign");
    s.checkFunctionExists("posix_spawnp");
    s.checkFunctionExists("pread");
    s.checkFunctionExists("putc_unlocked");
    s.checkFunctionExists("pwrite");
    s.checkFunctionExists("qsort_r");
    s.checkFunctionExists("raise");
    s.checkFunctionExists("rand_r");
    s.checkFunctionExists("readdir_r");
    s.checkFunctionExists("readlink");
    s.checkFunctionExists("realpath");
    s.checkFunctionExists("seekdir");
    s.checkFunctionExists("select");
    s.checkFunctionExists("setrlimit");
    s.checkFunctionExists("shmctl");
    s.checkFunctionExists("sigaction");
    s.checkFunctionExists("sigemptyset");
    s.checkFunctionExists("spawnvp");
    s.checkFunctionExists("strerror");
    s.checkFunctionExists("strerror_r");
    s.checkFunctionExists("strlcat");
    s.checkFunctionExists("strlcpy");
    s.checkFunctionExists("strtoll");
    s.checkFunctionExists("sysconf");
    s.checkFunctionExists("telldir");
    s.checkFunctionExists("times");
    s.checkFunctionExists("ulltostr");
    s.checkFunctionExists("vsnprintf");
    s.checkFunctionExists("vsprintf");
    s.checkFunctionExists("_exit");
    s.checkFunctionExists("_NSGetExecutablePath");
    s.checkFunctionExists("_pclose");
    s.checkFunctionExists("_popen");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("machine/param.h");
    s.checkIncludeExists("mach-o/dyld.h");
    s.checkIncludeExists("process.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("sun_prefetch.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/times.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("tiffconf.h");
    s.checkIncludeExists("wincrypt.h");
    s.checkTypeSize("int");
    s.checkTypeSize("int*");
    s.checkTypeSize("long");
    s.checkTypeSize("long_double_wider");
    s.checkTypeSize("mode_t");
    s.checkTypeSize("off_t");
    s.checkTypeSize("pid_t");
    s.checkTypeSize("short");
    s.checkTypeSize("signal");
    s.checkTypeSize("signed");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("unsigned");
    s.checkTypeSize("void *");
    s.checkLibraryFunctionExists("lcms2", "cmsSetLogErrorHandler");
    s.checkLibraryFunctionExists("dl", "dlopen");
    s.checkLibraryFunctionExists("dps", "DPSInitialize");
    s.checkLibraryFunctionExists("fpx", "FPX_OpenImageByFilename");
    s.checkLibraryFunctionExists("omp", "GOMP_parallel_start");
    s.checkLibraryFunctionExists("gs", "gsapi_new_instance");
    s.checkLibraryFunctionExists("ltdl", "lt_dlinit");
    s.checkLibraryFunctionExists("mpeg2", "mpeg2_decode_data");
    s.checkLibraryFunctionExists("mp", "mp_destroy");
    s.checkLibraryFunctionExists("openmp", "omp_get_num_procs");
    s.checkLibraryFunctionExists("mtsk", "sunw_mp_register_warn");
    s.checkLibraryFunctionExists("trio", "trio_vsnprintf");
    s.checkLibraryFunctionExists("umem", "umem_alloc");
    s.checkLibraryFunctionExists("umem", "umem_free");
    s.checkLibraryFunctionExists("webp", "WebPDecodeRGB");
    s.checkLibraryFunctionExists("wmflite", "wmf_lite_create");
    s.checkLibraryFunctionExists("dpstk", "XDPSPixelsPerPoint");
    s.checkLibraryFunctionExists("Xext", "XShapeCombineMask");
    s.checkLibraryFunctionExists("Xext", "XShmAttach");
    s.checkLibraryFunctionExists("xlsmp", "_xlsmpFlush");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
}
