void build(Solution &s)
{
    auto &sdl = s.addProject("valve.sdl", "2.0.9");
    sdl += RemoteFile("https://www.libsdl.org/release/SDL2-{v}.tar.gz");

    auto &sdl2 = sdl.addTarget<LibraryTarget>("sdl");
    {
        auto &sdl = sdl2;
        sdl.ApiName = "DECLSPEC";
        sdl +=
            "include/.*"_rr,
            "src/.*\\.c"_rr,
            "src/.*\\.h"_rr,
            "src/.*\\.m"_rr;

        sdl -= "src/main/.*"_rr;
        sdl -= "src/test/.*"_rr;

        sdl -= "src/core/.*"_rr;

        sdl -= "src/thread/.*"_rr;
        sdl += "src/thread/[^/]*"_rr;
        //sdl += "src/thread/generic/.*"_rr;
        //sdl += "src/thread/stdcpp/.*"_rr;

        sdl -= "src/video/.*"_rr;
        sdl += "src/video/[^/]*"_rr;
        sdl += "src/video/yuv2rgb/.*"_rr;
        sdl += "src/video/dummy/.*"_rr;

        sdl -= "src/hidapi/.*"_rr;

        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            sdl += "src/core/windows/.*"_rr;
            sdl += "src/hidapi/windows/.*"_rr;
            sdl += "src/thread/windows/.*"_rr;
            sdl += "src/thread/generic/SDL_syscond.c"_rr;
            sdl += "src/video/windows/.*"_rr;
            sdl.Public +=
                "user32.lib"_lib,
                "gdi32.lib"_lib,
                "kernel32.lib"_lib,
                "Ole32.lib"_lib,
                "Advapi32.lib"_lib,
                "Imm32.lib"_lib,
                "Mincore.lib"_lib,
                "winmm.lib"_lib,
                "version.lib"_lib,
                "Shell32.lib"_lib,
                "Setupapi.lib"_lib
                ;
        }
        sdl -= "src/.*\\.def"_rr;
    }

    auto &main = sdl.addTarget<StaticLibraryTarget>("main");
    {
        main -= "src/main/.*"_rr;
        if (s.Settings.TargetOS.Type == OSType::Windows)
            main += "src/main/windows/.*"_rr;
        main += sdl2;
    }
}
