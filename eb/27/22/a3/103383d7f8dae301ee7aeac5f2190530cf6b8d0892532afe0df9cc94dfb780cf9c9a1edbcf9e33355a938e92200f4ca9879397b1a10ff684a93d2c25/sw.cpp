void build(Solution &s)
{
    auto &t = s.addStaticLibrary("erikd.libsndfile", "master");
    t += Git("https://github.com/erikd/libsndfile");

    //t.ExportAllSymbols = true;
    t.setChecks("snd");

    t += "src/.*"_rr;
    t -= "src/test.*"_rr;

    if (t.Variables["WORDS_BIGENDIAN"] == 1)
        t.Variables["CPU_IS_BIG_ENDIAN"] = 1;
    else
        t.Variables["CPU_IS_LITTLE_ENDIAN"] = 1;

    t.Variables["HAVE_EXTERNAL_XIPH_LIBS"] = 1;
    t.Variables["SIZEOF_SF_COUNT_T_CODE"] = "#define SIZEOF_SF_COUNT_T 8";
    t.Variables["SF_COUNT_MAX"] = "LLONG_MAX";
    t.Variables["TYPEOF_SF_COUNT_T"] = "long long";
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t.Variables["USE_WINDOWS_API"] = 1;
        if (t.getBuildSettings().Native.LibrariesType == LibraryType::Shared)
            t.Variables["WIN32_TARGET_DLL"] = 1;
    }

    t.configureFile("src/config.h.cmake", t.BinaryPrivateDir / "config.h");
    t.configureFile("src/sndfile.h.in", "sndfile.h");

    t += "org.sw.demo.xiph.flac.libflac"_dep;
    t += "org.sw.demo.xiph.opus"_dep;
    t += "org.sw.demo.xiph.vorbis.libvorbis"_dep;
}

void check(Checker &c)
{
    auto &s = c.addSet("snd");
    s.checkFunctionExists("atoll");
    s.checkFunctionExists("dlclose");
    s.checkFunctionExists("dlopen");
    s.checkFunctionExists("dlsym");
    s.checkFunctionExists("ftruncate");
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
    s.checkIncludeExists("fcntl.h");
    s.checkIncludeExists("io.h");
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
    s.checkTypeSize("long long");
    s.checkTypeSize("double");
    s.checkTypeSize("float");
    s.checkTypeSize("short");
    s.checkTypeSize("int");
    s.checkTypeSize("wchar_t");
    s.checkTypeSize("int16_t");
    s.checkTypeSize("int32_t");
    s.checkTypeSize("int64_t");
    s.checkTypeSize("long");
    s.checkTypeSize("off_t");
    s.checkTypeSize("loff_t");
    s.checkTypeSize("off64_t");
    s.checkTypeSize("size_t");
    s.checkTypeSize("ssize_t");
    s.checkTypeSize("uint16_t");
    s.checkTypeSize("uint32_t");
    s.checkTypeSize("uintptr_t");
    s.checkTypeSize("void*");
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
