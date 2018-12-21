void build(Solution &s)
{
    auto &zbar = s.addTarget<StaticLibraryTarget>("spadix.zbar", "master");
    zbar += Git("https://github.com/ZBar/ZBar", "", "master");

    zbar.setChecks("zbar");

    zbar +=
        "include/.*\\.h"_rr,
        "zbar/.*\\.c"_rr,
        "zbar/.*\\.h"_rr;

    zbar.Private +=
        "zbar"_id;

    zbar.Public +=
        "include"_id;

    zbar.Public += "DEBUG_SVG"_d;
    zbar.Public += "ENABLE_CODABAR"_d;
    zbar.Public += "ENABLE_CODE128"_d;
    zbar.Public += "ENABLE_CODE39"_d;
    zbar.Public += "ENABLE_CODE93"_d;
    zbar.Public += "ENABLE_DATABAR"_d;
    zbar.Public += "ENABLE_EAN"_d;
    zbar.Public += "ENABLE_I25"_d;
    zbar.Public += "ENABLE_PDF417"_d;
    zbar.Public += "ENABLE_QRCODE"_d;
    zbar.Public += "HAVE_LIBJPEG"_d;
    zbar.Public += "ZBAR_VERSION_MAJOR=0"_d;
    zbar.Public += "ZBAR_VERSION_MINOR=10"_d;
    if (s.Settings.TargetOS.Type != OSType::Windows)
    {
        zbar.Public += "HAVE_LIBPTHREAD"_d;
    }
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        zbar.Public += "Winmm.lib"_lib;
    }

    zbar.Public += "org.sw.demo.gnu.iconv.libiconv-1"_dep;
    zbar.Public += "org.sw.demo.jpeg-9"_dep;
    zbar.Public += "org.sw.demo.glennrp.png-1"_dep;

    zbar.writeFileOnce("config.h");
    if (s.Settings.TargetOS.Type == OSType::Windows)
        zbar.writeFileOnce("unistd.h");

    zbar.patch("zbar/thread.h", "# define ZTHREAD DWORD WINAPI", "# define ZTHREAD DWORD");
    zbar.patch("zbar/thread.h",
        "typedef ZTHREAD (zbar_thread_proc_t)(void*);",
        "typedef ZTHREAD (WINAPI *zbar_thread_proc_t)(void*);");

    zbar.replaceInFileOnce("zbar/processor/win.c", "#include <windows.h>", "#define GWL_USERDATA        (-21)");

    //zbar -= "zbar/svg.c";
    zbar -= "zbar/video/.*"_rr;
    zbar -= "zbar/window/.*"_rr;

    zbar += "zbar/video/null.c";
    zbar += "zbar/window/null.c";

    zbar -= "zbar/processor/null.c";

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        zbar -= "zbar/processor/posix.c";
        zbar -= "zbar/processor/x.c";
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("zbar");
    s.checkFunctionExists("atexit");
    s.checkFunctionExists("memset");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("setenv");
    s.checkIncludeExists("assert.h");
    s.checkIncludeExists("errno.h");
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("features.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("jpeglib.h");
    s.checkIncludeExists("linux/videodev2.h");
    s.checkIncludeExists("linux/videodev.h");
    s.checkIncludeExists("poll.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/ipc.h");
    s.checkIncludeExists("sys/mman.h");
    s.checkIncludeExists("sys/shm.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/times.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("vfw.h");
    s.checkIncludeExists("X11/extensions/XShm.h");
    s.checkIncludeExists("X11/extensions/Xvlib.h");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uint8_t");
    s.checkTypeSize("uintptr_t");
    s.checkTypeSize("void *");
    s.checkLibraryFunctionExists("pthread", "pthread_create");
    s.checkLibraryFunctionExists("Xext", "XShmQueryVersion");
    s.checkLibraryFunctionExists("Xv", "XvQueryExtension");
}
