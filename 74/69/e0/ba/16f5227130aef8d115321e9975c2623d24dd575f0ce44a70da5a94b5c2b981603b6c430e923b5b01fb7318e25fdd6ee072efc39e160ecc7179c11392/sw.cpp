void build(Solution &s)
{
    auto &sdl = s.addProject("valve.sdl", "2.0.12");
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

        if (sdl.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            sdl += "src/core/windows/.*"_rr;
            sdl += "src/hidapi/windows/.*"_rr;
            sdl += "src/thread/windows/.*"_rr;
            sdl += "src/thread/generic/SDL_syscond.c"_rr;
            sdl += "src/video/windows/.*"_rr;
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
                "uuid.lib"_slib
                ;
        }
        sdl -= "src/.*\\.def"_rr;

        // install
        if (!sdl.DryRun)
        {
            auto d = sdl.BinaryDir / "SDL2";
            if (!fs::exists(d))
            {
                fs::create_directories(d);
                for (auto &[p, sf] : sdl["include/.*"_rr])
                    fs::copy_file(p, d / p.filename(), fs::copy_options::update_existing);
            }
        }
    }

    auto &main = sdl.addTarget<StaticLibraryTarget>("main");
    {
        main -= "src/main/.*"_rr;
        if (main.getBuildSettings().TargetOS.Type == OSType::Windows)
            main += "src/main/windows/.*"_rr;
        main += sdl2;
    }
}