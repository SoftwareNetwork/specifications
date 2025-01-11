void build(Solution &s)
{
    auto &p = s.addProject("kcat", "1.24.2");
    p += Git("https://github.com/kcat/openal-soft", "{v}");

    auto &openal = p.addTarget<LibraryTarget>("openal");
    {
        openal += cpp17;
        openal += "AL_API"_api;
        openal += "ALC_API"_api;
        openal.setChecks("openal");

        openal -=
            "al/.*"_rr,
            "alc/.*"_rr,
            "common/.*"_rr,
            "router/.*"_rr,
            "config.h.in",
            "include/.*"_rr,
            "version.h.in";

        openal.Private +=
            "common"_id,
            "al"_id,
            "alc"_id,
            "."_id
            ;

        openal.Public +=
            "include"_id,
            "include/AL"_id;

        openal +=
            "common/.*"_rr,
            "core/.*"_rr,
            //"router/.*"_rr,
            "al/.*"_rr,
            "alc/.*"_rr
            ;
        openal -=
            "alc/backends/.*"_rr;
        openal +=
            "alc/backends/base.cpp",
            "alc/backends/loopback.cpp",
            "alc/backends/null.cpp"
            ;
        openal -= "core/dbus_wrap.cpp";
        openal -= "core/rtkit.cpp";
        openal -= "core/mixer/mixer_neon.cpp";

        openal += "HAVE_STRUCT_TIMESPEC"_def;
        openal += "AL_ALEXT_PROTOTYPES"_def;
        openal += "RESTRICT="_def;
        openal.Public += "ALSOFT_EAX"_def;

        openal.patch("core/ambdec.cpp", "auto &gains", "auto &&gains");
        openal.patch("core/ambdec.cpp", "al::size(gains)", "MaxAmbiOrder+1");

        if (openal.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            openal +=
                "alc/backends/winmm.cpp",
                "alc/backends/wave.cpp",
                "alc/backends/wasapi.cpp",
                "alc/backends/dsound.cpp";
            openal.Variables["HAVE_DSOUND"] = 1;
            openal.Variables["HAVE_WINMM"] = 1;
            openal.Variables["HAVE_WAVE"] = 1;
            openal.Variables["HAVE_WASAPI"] = 1;
            openal += "NOMINMAX"_def;
            openal.Public +=
                "Winmm.lib"_slib,
                "Ole32.lib"_slib,
                "Shell32.lib"_slib,
                "User32.lib"_slib
                ;
            openal += "restrict="_def;
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

        //openal.Variables["ASSUME_ALIGNED_DECL"] = "x";

        //openal.Variables["EXPORT_DECL"] = "SW_AL_API";
        //openal.Variables["ALIGN_DECL"] = "x";

        openal.configureFile("version.h.in", "version.h");
        openal.configureFile("config.h.in", openal.BinaryPrivateDir / "config.h");
    }

    auto &tools = p.addDirectory("tools");
    //tools.Scope = TargetScope::Tool;

    /*auto &bin2h = tools.addExecutable("bin2h");
    {
        bin2h.SourceDir = openal.SourceDir;
        bin2h += "native-tools/bin2h.c";
    }*/

    /*auto &bsincgen = tools.addExecutable("bsincgen");
    {
        bsincgen.SourceDir = openal.SourceDir;
        bsincgen += "native-tools/bsincgen.c";
        if (bsincgen.getBuildSettings().TargetOS.Type == OSType::Windows)
            bsincgen += "Shell32.lib"_slib;

        auto c = openal.addCommand();
        c << cmd::prog(bsincgen)
            << cmd::out("bsinc_inc.h");
    }*/
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
