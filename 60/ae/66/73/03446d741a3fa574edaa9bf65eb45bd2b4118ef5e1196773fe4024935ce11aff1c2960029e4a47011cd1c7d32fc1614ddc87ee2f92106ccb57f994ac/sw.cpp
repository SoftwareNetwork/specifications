void build(Solution &s)
{
    auto &p = s.addProject("mpg123", "1.25.12");
    p += RemoteFile("https://sourceforge.net/projects/mpg123/files/mpg123/{v}/mpg123-{v}.tar.bz2");

    auto &compat = p.addStaticLibrary("compat");
    {
        compat.setChecks("mpg123");

        compat += "src/compat/.*"_rr;
        compat += "src/libmpg123/abi_align.h";
        compat += "src/libmpg123/debug.h";
        compat += "src/libmpg123/fmt123.h";
        compat += "src/libmpg123/true.h";
        compat += "src/intsym.h";

        compat.Protected += "src"_idir;
        compat.Protected += "src/compat"_idir;
        compat.Protected += "src/libmpg123"_idir;

        compat.Protected += "USE_MODULES"_def;

        if (compat.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            compat.Protected += "WIN32"_def;
            compat.Protected += "WANT_WIN32_UNICODE"_def;
        }

        compat.configureFile("src/config.h.in", compat.BinaryPrivateDir / "config.h", ConfigureFlags::EnableUndefReplacements);
        compat.Protected += IncludeDirectory(compat.BinaryPrivateDir);
    }

    auto &out123 = p.addLibrary("out123");
    {
        auto &t = out123;

        t.setSourceDirectory("src/libout123");

        t.ApiName = "MPG123_EXPORT";
        //t.ApiName = "SW_OUT123_EXPORT";
        //t += "MPG123_EXPORT=SW_OUT123_EXPORT"_def;

        t -= ".*"_rr;
        t += ".*"_r;

        t -= "legacy_module.c";

        if (compat.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            t += "STDOUT_FILENO=1"_def;
            t += "DEFAULT_OUTPUT_MODULE=\"win32\""_def;

            t -= "buffer.c";
            t -= "xfermem.c";
            t += "NOXFERMEM"_def;

            t += "SIGCHLD=1"_def; // hup
            t += "SIGKILL=9"_def;

            t.Protected += "strcasecmp=_strcmpi"_def;
            t.Protected += "strncasecmp=_strnicmp"_def;

            t.Protected += "Shlwapi.lib"_slib;
        }

        t += "PKGLIBDIR=\"\""_def;
        t += "LT_MODULE_EXT=\"\""_def;

        t.Variables["OUTAPI_VERSION"] = 2;
        t.configureFile("out123.h.in", "out123.h");

        out123.Protected += compat;
    }

    auto &mpg123 = p.addLibrary("mpg123");
    {
        auto &t = mpg123;

        t.setSourceDirectory("src/libmpg123");

        t.ApiName = "MPG123_EXPORT";

        t -= ".*"_rr;
        t +=
            "parse.c",
            "frame.c",
            "format.c",
            "dct64.c",
            "equalizer.c",
            "id3.c",
            "optimize.c",
            "readers.c",
            "tabinit.c",
            "libmpg123.c",
            "index.c",
            "icy.c",
            "icy2utf8.c",
            "ntom.c",
            "layer1.c",
            "layer2.c",
            "layer3.c",
            "synth.c",
            "synth_8bit.c",
            "synth_s32.c",
            "synth_real.c",
            "stringbuf.c"
            ;

        t += "OPT_GENERIC"_def;

        t.Variables["API_VERSION"] = 44;
        t.configureFile("mpg123.h.in", "mpg123.h");

        t += out123;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("mpg123");
    s.checkFunctionExists("atoll");
    s.checkFunctionExists("dlclose");
    s.checkFunctionExists("dlopen");
    s.checkFunctionExists("dlsym");
    s.checkFunctionExists("getaddrinfo");
    s.checkFunctionExists("getuid");
    s.checkFunctionExists("mkfifo");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("nl_langinfo");
    s.checkFunctionExists("random");
    s.checkFunctionExists("sched_setscheduler");
    s.checkFunctionExists("setlocale");
    s.checkFunctionExists("setpriority");
    s.checkFunctionExists("setuid");
    s.checkFunctionExists("sigaction");
    s.checkFunctionExists("sigaddset");
    s.checkFunctionExists("sigemptyset");
    s.checkFunctionExists("sigprocmask");
    s.checkFunctionExists("strerror");
    s.checkFunctionExists("_wopen");
    s.checkIncludeExists("alc.h");
    s.checkIncludeExists("Alib.h");
    s.checkIncludeExists("alsa/asoundlib.h");
    s.checkIncludeExists("AL/alc.h");
    s.checkIncludeExists("AL/al.h");
    s.checkIncludeExists("al.h");
    s.checkIncludeExists("arpa/inet.h");
    s.checkIncludeExists("asm/audioio.h");
    s.checkIncludeExists("audios.h");
    s.checkIncludeExists("AudioToolbox/AudioToolbox.h");
    s.checkIncludeExists("AudioUnit/AudioUnit.h");
    s.checkIncludeExists("audio/audiolib.h");
    s.checkIncludeExists("CoreServices/CoreServices.h");
    s.checkIncludeExists("CUlib.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("dmedia/audio.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("langinfo.h");
    s.checkIncludeExists("limits.h");
    s.checkIncludeExists("linux/soundcard.h");
    s.checkIncludeExists("locale.h");
    s.checkIncludeExists("machine/soundcard.h");
    s.checkIncludeExists("netdb.h");
    s.checkIncludeExists("netinet/in.h");
    s.checkIncludeExists("netinet/tcp.h");
    s.checkIncludeExists("OpenAL/alc.h");
    s.checkIncludeExists("OpenAL/al.h");
    s.checkIncludeExists("os2me.h");
    s.checkIncludeExists("os2.h");
    s.checkIncludeExists("portaudio.h");
    s.checkIncludeExists("sched.h");
    s.checkIncludeExists("signal.h");
    s.checkIncludeExists("sndio.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("stdio.h");
    s.checkIncludeExists("stdlib.h");
    s.checkIncludeExists("string.h");
    s.checkIncludeExists("sun/audioio.h");
    s.checkIncludeExists("sys/asoundlib.h");
    s.checkIncludeExists("sys/audioio.h");
    s.checkIncludeExists("sys/audio.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/param.h");
    s.checkIncludeExists("sys/resource.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/signal.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("sys/soundcard.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/wait.h");
    s.checkIncludeExists("tinyalsa/asoundlib.h");
    s.checkIncludeExists("unistd.h");
    s.checkIncludeExists("windows.h");
    s.checkIncludeExists("ws2tcpip.h");
    s.checkTypeSize("int16_t");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("long");
    s.checkTypeSize("off_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uintptr_t");
    //s.checkLibraryFunctionExists("avrt", "");
    s.checkLibraryFunctionExists("audio", "AuOpenServer");
    s.checkLibraryFunctionExists("portaudio", "Pa_GetVersion");
    s.checkLibraryFunctionExists("portaudio", "Pa_Initialize");
    s.checkLibraryFunctionExists("tinyalsa", "pcm_open");
    s.checkLibraryFunctionExists("mx", "powf");
    s.checkLibraryFunctionExists("pthread", "sem_post");
    s.checkLibraryFunctionExists("sndio", "sio_open");
    s.checkLibraryFunctionExists("asound", "snd_pcm_open");
    s.checkLibraryFunctionExists("m", "sqrt");
    s.checkLibraryFunctionExists("os2term", "tcsetattr");
    s.checkLibraryFunctionExists("winmm", "waveOutOpen");
    s.checkSourceCompiles("STDC_HEADERS", R"sw_xxx(
    #include <stdlib.h>
    #include <stdarg.h>
    #include <string.h>
    #include <float.h>
    int main() {return 0;}
    )sw_xxx");
}
