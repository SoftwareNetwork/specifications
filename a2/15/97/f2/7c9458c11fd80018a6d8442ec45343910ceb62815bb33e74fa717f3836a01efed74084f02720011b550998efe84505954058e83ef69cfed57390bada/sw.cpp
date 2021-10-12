void build(Solution &s)
{
    auto &imgui = s.addTarget<LibraryTarget>("ocornut.imgui", "1.85.0");
    imgui += Git("https://github.com/ocornut/imgui", "v{M}.{m}{po}");
    {
        imgui += "IMGUI_API"_api;
        imgui += "im.*"_r;
        //imgui -= "imgui_demo.cpp";

        if (imgui.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            imgui += "imm32.lib"_slib;
            imgui += "User32.lib"_slib;
        }

        imgui.Public += "org.sw.demo.stb.all-master"_dep;
    }

    for (String b : {
        "dx9",
        "dx10",
        "dx11",
        "dx12",
        "win32",
        "sdl",
        "osx",
        "glfw",
        "opengl2",
        "opengl3",
        "allegro5",
        "wgpu",
        "glut",
        "marmalade",
        "android",
        })
    {
        auto &t = imgui.addStaticLibrary("backend." + b);
        t += "IMGUI_API"_api;
        t += FileRegex("backends", "imgui_impl_" + b + ".*", false);
        t.Public += IncludeDirectory("backends"s);
        t.Public += imgui;

        if (b == "dx10" || b == "dx11" || b == "dx12")
        {
            t += "d3dcompiler.lib"_slib;
        }
        else if (b == "win32")
        {
            t += "imm32.lib"_slib;
            t += "User32.lib"_slib;
            t += "gdi32.lib"_slib;
            t += "Dwmapi.lib"_slib;
        }
        else if (b == "opengl2" || b == "opengl3")
        {
            t.Public += "org.sw.demo.find.opengl-master"_dep;
            t.Public += "org.sw.demo.glew"_dep;
        }
        else if (b == "glfw")
        {
            t.Public += "org.sw.demo.glfw"_dep;
        }
        else if (b == "sdl")
        {
            t.Public += "org.sw.demo.valve.sdl"_dep;
        }
        else if (b == "glut")
        {
            t.Public += "org.sw.demo.freeglut"_dep;
        }
        else if (b == "allegro5")
        {
            t.Public += "org.sw.demo.liballeg.allegro5.addons.primitives"_dep;
            // needed for macos, ignored elsewhere
            t.Public += "org.sw.demo.liballeg.allegro5.addons.main"_dep;
        }
    }
}
