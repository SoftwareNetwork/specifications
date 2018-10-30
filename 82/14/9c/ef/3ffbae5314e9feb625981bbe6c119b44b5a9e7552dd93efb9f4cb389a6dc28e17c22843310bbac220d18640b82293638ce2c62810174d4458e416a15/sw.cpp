void build(Solution &s)
{
    auto &p = s.addProject("kcat.openal", "1.19.1");
    p += Git("https://github.com/kcat/openal-soft", "openal-soft-{v}");

    auto &openal = p.addTarget<LibraryTarget>("kcat.openal");

    openal.ApiName = "SW_AL_API";
    openal.setChecks("openal");

    openal -=
        "Alc/.*"_rr,
        "OpenAL32/.*"_rr,
        "common/.*"_rr,
        "config.h.in",
        "include/.*"_rr,
        "version.h.in";

    openal.Private +=
        "common"_id,
        "OpenAL32"_id,
        "OpenAL32/Include"_id,
        "Alc"_id
        ;

    openal.Public +=
        "include"_id,
        "include/AL"_id;

    openal +=
        "common/.*.c"_rr,
        "OpenAL32/.*.c"_rr,
        "Alc/ALc.c",
        "Alc/ALu.c",
        "Alc/alconfig.c",
        "Alc/bs2b.c",

        "Alc/converter.c",
        "Alc/mastering.c",

        "Alc/effects/.*.c"_rr,
        "Alc/filters/.*.c"_rr,
        "Alc/mixer/.*.c"_rr,

        "Alc/backends/base.c",
        "Alc/backends/loopback.c",
        "Alc/backends/null.c",

        "Alc/helpers.c",
        "Alc/hrtf.c",
        "Alc/panning.c",
        "Alc/mixvoice.c",

        "Alc/ringbuffer.c",
        "Alc/uhjfilter.c",
        "Alc/ambdec.c",
        "Alc/bformatdec.c"
        ;
    openal -=
        "Alc/mixer/hrtf_inc.c",
        "Alc/mixer/mixer_neon.c"
        ;

    openal += "HAVE_STRUCT_TIMESPEC"_def;
    openal += "AL_ALEXT_PROTOTYPES"_def;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        openal +=
            "Alc/backends/winmm.c",
            "Alc/backends/wave.c",
            "Alc/backends/wasapi.c",
            "Alc/backends/dsound.c";
        openal.Variables["HAVE_DSOUND"] = 1;
        openal.Variables["HAVE_WINMM"] = 1;
        openal.Variables["HAVE_WAVE"] = 1;
        openal.Variables["HAVE_WASAPI"] = 1;
        openal.Public +=
            "Winmm.lib"_lib,
            "Ole32.lib"_lib,
            "Shell32.lib"_lib,
            "User32.lib"_lib
            ;
        openal += "strcasecmp=_stricmp"_def;
        openal += "strncasecmp=_strnicmp"_def;
    }

    openal.Variables["HAVE_SSE"] = 1;
    openal.Variables["HAVE_SSE2"] = 1;
    openal.Variables["HAVE_SSE3"] = 1;
    openal.Variables["HAVE_SSE4_1"] = 1;

    openal.Variables["HAVE__CONTROLFP"] = 1;
    //openal.Variables["HAVE___CONTROL87_2"] = 1;

    openal.Variables["HAVE_CPUID_INTRINSIC"] = 1;

    openal.Variables["ASSUME_ALIGNED_DECL"] = "x";

    openal.Variables["EXPORT_DECL"] = "SW_AL_API";
    //openal.Variables["ALIGN_DECL"] = "x";

    openal.configureFile("version.h.in", "version.h");
    openal.configureFile("config.h.in", openal.BinaryPrivateDir / "config.h");

    auto &tools = p.addDirectory("tools");

    auto &bin2h = tools.addExecutable("bin2h");
    bin2h += "native-tools/bin2h.c";

    auto &bsincgen = tools.addExecutable("bsincgen");
    bsincgen += "native-tools/bsincgen.c";
    (bsincgen + openal)->IncludeDirectoriesOnly = true;
    bsincgen += "Shell32.lib"_lib;

    auto c = openal.addCommand();
    c << cmd::prog(bsincgen)
        << cmd::out("bsinc_inc.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("openal");
    s.checkFunctionExists("aligned_alloc");
    s.checkFunctionExists("posix_memalign");
    s.checkFunctionExists("_aligned_malloc");
    s.checkIncludeExists("cpuid.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("fenv.h");
    s.checkIncludeExists("float.h");
    s.checkIncludeExists("guiddef.h");
    s.checkIncludeExists("ieeefp.h");
    s.checkIncludeExists("intrin.h");
    s.checkIncludeExists("malloc.h");
    s.checkIncludeExists("stdalign.h");
    s.checkIncludeExists("stdbool.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("strings.h");
    s.checkIncludeExists("windows.h");
    s.checkIncludeExists("guiddef.h");
    s.checkIncludeExists("initguid.h");
    s.checkIncludeExists("sys/sysconf.h");
    s.checkTypeSize("long");
    s.checkTypeSize("long long");
    s.checkTypeSize("size_t");
    s.checkTypeSize("void *");
}
