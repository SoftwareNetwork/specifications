void build(Solution &s)
{
    auto &imgui = s.addTarget<LibraryTarget>("ocornut.imgui", "1.82.0");
    imgui += Git("https://github.com/ocornut/imgui", "v{M}.{m}");
    {
        imgui += "IMGUI_API"_api;
        imgui += "im.*"_r;
        imgui -= "imgui_demo.cpp";

        if (imgui.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            imgui += "imm32.lib"_slib;
            imgui += "User32.lib"_slib;
        }

        imgui.Public += "org.sw.demo.stb.all-master"_dep;
        //imgui.Public += "org.sw.demo.glfw"_dep;
        //imgui.Public += "org.sw.demo.gl3w-master"_dep;
    }

    for (String b : {
        "dx9",
        "dx10",
        "dx11",
        "dx12",
        "win32",
        "sdl",
        "osx",

        /*
            //"glfw.*"_rr,
            //"opengl3.*"_rr,

            wgpu.cpp",
            glut.cpp",
            marmalade.cpp",
            allegro5.cpp",
            android.cpp"
            ;*/
        })
    {
        auto &t = imgui.addLibrary("backend." + b);
        t += "IMGUI_API"_api;
        t += FileRegex("backends", "imgui_impl_" + b + ".*", false);
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
        else if (b == "sdl")
        {
            t += "org.sw.demo.valve.sdl"_dep;
        }
    }
}
