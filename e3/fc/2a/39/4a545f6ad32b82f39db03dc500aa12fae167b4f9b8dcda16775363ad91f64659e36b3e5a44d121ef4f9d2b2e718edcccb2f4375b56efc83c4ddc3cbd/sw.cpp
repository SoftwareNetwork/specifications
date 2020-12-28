void build(Solution &s)
{
    auto &p = s.addProject("FluidSynth", "2.1.3");
    p += Git("https://github.com/FluidSynth/fluidsynth", "v{v}");

    auto &gentables = p.addExecutable("gentables");
    {
        gentables += "src/gentables/.*"_rr;
        gentables += "src/rvoice/fluid_rvoice_dsp_tables.h";
        gentables += "src/utils/fluid_conv_tables.h";
        if (gentables.getBuildSettings().TargetOS.Type == OSType::Windows)
            gentables += "_USE_MATH_DEFINES"_def;
    }

    auto &t = p.addLibrary("fluidsynth");
    {
        t += Git("https://github.com/FluidSynth/fluidsynth", "v{v}");

        t.setChecks("fs");

        t += "include/.*"_rr;
        t += "src/.*"_rr;
        t -= "src/.*"_r;
        t -= "src/gentables/.*"_rr;

        t.Public += sw::Static, "FLUIDSYNTH_NOT_A_DLL"_def;
        t += sw::Shared, "FLUIDSYNTH_DLL_EXPORTS"_def;

        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
            t += "WIN32"_def;

        t.Public += "include"_idir;
        t += "src"_idir;
        t += "src/bindings"_idir;
        t += "src/drivers"_idir;
        t += "src/midi"_idir;
        t += "src/rvoice"_idir;
        t += "src/sfloader"_idir;
        t += "src/synth"_idir;
        t += "src/utils"_idir;

        t += "AUFILE_SUPPORT"_def;

        if (t.getBuildSettings().Native.LibrariesType == LibraryType::Shared)
            t.Variables["BUILD_SHARED_LIBS"] = 1;
        t.configureFile("include/fluidsynth.cmake", "fluidsynth.h");
        t.configureFile("src/config.cmake", "config.h");

        t.Variables["FLUIDSYNTH_VERSION"] = "\"" + t.Variables["PACKAGE_VERSION"].toString() + "\"";
        t.Variables["FLUIDSYNTH_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
        t.Variables["FLUIDSYNTH_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
        t.Variables["FLUIDSYNTH_VERSION_MICRO"] = t.Variables["PACKAGE_VERSION_PATCH"];
        t.configureFile("include/fluidsynth/version.h.in", "fluidsynth/version.h");

        t += "org.sw.demo.gnome.glib.gobject"_dep;
        t += "org.sw.demo.swami.libinstpatch"_dep;

        t.addCommand()
            << cmd::prog(gentables)
            << to_printable_string(t.BinaryDir.u8string()) + "\\"
            << cmd::end()
            << cmd::out("fluid_conv_tables.c", cmd::Skip)
            << cmd::out("fluid_rvoice_dsp_tables.c", cmd::Skip)
            ;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("fs");
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
