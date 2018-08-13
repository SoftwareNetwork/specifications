void build(Solution &s)
{
    auto &magick = s.addTarget<LibraryTarget>("graphicsmagick.magick", "1.3.29");
    magick += RemoteFile("https://downloads.sourceforge.net/project/graphicsmagick/graphicsmagick/{v}/GraphicsMagick-{v}-windows-source.7z");

    magick.setChecks("magick");

    magick +=
        "coders/.*\\.[hc]"_rr,
        "filters/.*\\.[hc]"_rr,
        "magick/.*\\.[hc]"_rr,
        "magick/.*\\.in"_rr;

    magick.Private += "MagickBinPath=\"\""_d;
    magick.Private += "MagickCoderModulesPath=\"\""_d;
    magick.Private += "MagickCoderModulesSubdir=\"\""_d;
    magick.Private += "MagickFilterModulesPath=\"\""_d;
    magick.Private += "MagickFilterModulesSubdir=\"\""_d;
    magick.Private += "MagickLibConfigPath=\"\""_d;
    magick.Private += "MagickLibConfigSubDir=\"\""_d;
    magick.Private += "MagickLibPath=\"\""_d;
    magick.Private += "MagickLibSubdir=\"\""_d;
    magick.Private += "MagickShareConfigPath=\"\""_d;
    magick.Private += "MagickShareConfigSubDir=\"\""_d;
    magick.Private += "MagickSharePath=\"\""_d;
    magick.Public += "HasWEBP"_d;
    magick.Public += "QuantumDepth=16"_d;
    magick.Public += "_MAGICKLIB_"_d;
    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
    {
        magick.Private += "restrict=__restrict"_d;
    }
    if (s.Settings.TargetOS.Type != OSType::Windows)
    {
        magick.Public += "HAVE_PREAD=1"_d;
        magick.Public += "HAVE_PTHREAD"_d;
        magick.Public += "HAVE_PWRITE=1"_d;
    }
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        magick.Public += "HAVE_CRYPTGENRANDOM"_d;
        magick.Public += "HAVE_WINCRYPT_H"_d;
        magick.Public += "MSWINDOWS"_d;
        magick.Public += "User32.lib"_lib, "Advapi32.lib"_lib, "Gdi32.lib"_lib;
    }
    magick.Public += "_MAGICKDLL_"_d;

    magick.Public += "org.sw.demo.webmproject.webp-*"_dep;

    magick.Variables["INT8_T"] = "int8_t";
    magick.Variables["UINT8_T"] = "uint8_t";
    magick.Variables["INT16_T"] = "int16_t";
    magick.Variables["UINT16_T"] = "uint16_t";
    magick.Variables["INT32_T"] = "int32_t";
    magick.Variables["UINT32_T"] = "uint32_t";
    magick.Variables["INT64_T"] = "int64_t";
    magick.Variables["UINT64_T"] = "uint64_t";

    magick.Variables["INT32_F"] = "\"i\"";
    magick.Variables["UINT32_F"] = "\"u\"";
    magick.Variables["INT64_F"] = "\"ll\"";
    magick.Variables["UINT64_F"] = "\"ull\"";

    magick.Variables["UINTMAX_T"] = "uintmax_t";
    magick.Variables["UINTMAX_F"] = "\"j\"";

    magick.Variables["UINTPTR_T"] = "uintptr_t";
    magick.Variables["UINTPTR_F"] = "\"t\"";

    magick.Variables["MAGICK_SIZE_T"] = "size_t";
    magick.Variables["MAGICK_SIZE_T_F"] = "\"u\"";

    magick.Variables["MAGICK_SSIZE_T"] = "ssize_t";
    magick.Variables["MAGICK_SSIZE_T_F"] = "\"z\"";

    magick.Variables["MAGICK_LIB_VERSION"] = 1033000;
    magick.Variables["MAGICK_LIB_VERSION_TEXT"] = "1.3.30";
    magick.Variables["MAGICK_LIB_VERSION_NUMBER"] = 1033000;

    magick.Variables["PACKAGE_CHANGE_DATE"] = 0;
    magick.Variables["PACKAGE_RELEASE_DATE"] = 0;

    magick.Variables["MAGICK_LIB_INTERFACE_NEWEST"] = 1;
    magick.Variables["MAGICK_LIB_INTERFACE_OLDEST"] = 1;

    magick.pushFrontToFileOnce("magick/magick_types.h.in", "#include <stdint.h>");
    magick.writeFileOnce("magick/magick_config.h");
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
    s.checkIncludeExists("sys/ndir.h");
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