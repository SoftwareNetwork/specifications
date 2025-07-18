void build(Solution &s)
{
    auto &sdl = s.addTarget<LibraryTarget>("valve.sdl", "3.2.16");
    sdl += Git("https://github.com/libsdl-org/SDL", "release-{v}");
    {
        sdl.ApiName = "DECLSPEC";
        //sdl.Public += sw::Shared, "DLL_EXPORT";
        sdl.Public += "SDL_DYNAMIC_API=0"_def;
        sdl.patch("src/dynapi/SDL_dynapi.h", "#error", "//# error");

        sdl -=
            "include/.*"_rr,
            "src/.*"_rr
            ;
        sdl.Public += "include"_idir;
        sdl.Protected += "src"_idir;
        sdl.Protected += "include/build_config"_idir;

        sdl.AllowEmptyRegexes = true;
        sdl +=
            "src/atomic/.*"_r,
            "src/audio/.*"_r,
            "src/camera/.*"_r,
            "src/core/.*"_r,
            "src/cpuinfo/.*"_r,
            //"src/dynapi/.*"_r,
            "src/events/.*"_r,
            "src/file/.*"_r,
            "src/filesystem/.*"_r,
            "src/haptic/.*"_r,
            "src/hidapi/.*"_r,
            "src/io/.*"_r,
            "src/joystick/.*"_r,
            "src/libm/.*"_r,
            "src/loadso/.*"_r,
            "src/locale/.*"_r,
            //"src/main/.*"_r,
            //"src/misc/.*"_r,
            "src/power/.*"_r,
            "src/render/.*"_r,
            "src/sensor/.*"_r,
            "src/stdlib/.*"_r,
            "src/thread/.*"_r,
            "src/timer/.*"_r,
            "src/tray/.*"_r,
            "src/video/.*"_r
            ;
        sdl += "src/.*"_r;
        sdl += "src/.*/dummy/.*"_rr;
        sdl += "src/audio/disk/.*"_rr;
        sdl += "src/joystick/virtual/.*"_rr;
        sdl += "src/render/opengl/.*"_rr;
        sdl += "src/render/opengles/.*"_rr;
        sdl += "src/render/opengles2/.*"_rr;
        sdl += "src/render/software/.*"_rr;
        sdl += "src/thread/generic/SDL_syscond.*"_rr;
        sdl += "src/video/yuv2rgb/.*"_rr;
        sdl.AllowEmptyRegexes = false;

        if (sdl.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            sdl += "src/.*/windows/.*"_rr;
            sdl += "src/audio/directsound/.*"_rr;
            sdl += "src/audio/wasapi/.*"_rr;
            //sdl += "src/audio/winmm/.*"_rr;
            sdl += "src/joystick/hidapi/.*"_rr;
            sdl += "src/render/direct3d/.*"_rr;
            sdl += "src/render/direct3d11/.*"_rr;
            sdl += "src/render/direct3d12/.*"_rr;

            //sdl -= "src/render/.*winrt.*"_rr;
            sdl -= "src/render/.*xbox.*"_rr;
            sdl -= "src/thread/pthread/.*"_rr;

            //sdl -= ".*/unix/.*"_rr;
            //sdl -= ".*/linux/.*"_rr;
            //sdl -= ".*/haiku/.*"_rr;
            //sdl -= ".*/n3ds/.*"_rr;
            //sdl -= ".*/qnx/.*"_rr;
            sdl -= ".*/dummy/.*"_rr;
            sdl += ".*/windows/.*"_rr;
            sdl += ".*/generic/.*"_rr;
            sdl += "src/tray/windows/.*"_rr;

            sdl.Public +=
                "user32.lib"_slib,
                "gdi32.lib"_slib,
                "kernel32.lib"_slib,
                "Ole32.lib"_slib,
                "Advapi32.lib"_slib,
                "Imm32.lib"_slib,
                "Mincore.lib"_slib,
                "winmm.lib"_slib,
                "version.lib"_slib,
                "Shell32.lib"_slib,
                "Setupapi.lib"_slib,
                "uuid.lib"_slib//,
                //"d3d12.lib"_slib
                ;
            sdl += "com.Microsoft.Windows.SDK.winrt"_dep;
        }
        sdl -= "src/hidapi/.+/.*"_rr;
        sdl -= "src/hidapi/hidtest/.*"_rr;
        sdl -= "src/main/.*"_rr;
        sdl -= "src/locale/dummy/.*"_rr;
        sdl -= "src/misc/dummy/.*"_rr;
        //sdl -= "src/.*\\.def"_rr;

        // install
        /*if (!sdl.DryRun)
        {
            auto d = sdl.BinaryDir / "SDL2";
            if (!fs::exists(d))
            {
                fs::create_directories(d);
                for (auto &[p, sf] : sdl["include/.*"_rr])
                    fs::copy_file(p, d / p.filename(), fs::copy_options::update_existing);
            }
        }*/
    }
    return;

    auto &main = sdl.addTarget<StaticLibraryTarget>("main");
    {
        main -= "src/main/.*"_rr;
        if (main.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            main += "src/main/.*"_rr;
            //main.patch("src/main/windows/SDL_windows_main.c", "../../core", "core");
        }
        main += sdl;
    }
}
