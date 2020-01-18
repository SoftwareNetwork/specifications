void build(Solution &s)
{
    auto &imgui = s.addTarget<LibraryTarget>("ocornut.imgui", "1.71.0");
    imgui += Git("https://github.com/ocornut/imgui", "v{M}.{m}");

    imgui.ApiNames.insert("IMGUI_API");
    imgui +=
        "examples/imgui_impl_glfw.*"_rr,
        "examples/imgui_impl_opengl3.*"_rr,
        "examples/imgui_impl_win32.*"_rr,
        "im.*"_rr;

    imgui -=
        "examples/.*"_rr;
    imgui.Public +=
        "examples"_id;

    imgui.Public += "org.sw.demo.stb.all-master"_dep;
    imgui.Public += "org.sw.demo.glfw"_dep;
    //imgui.Public += "org.sw.demo.gl3w-master"_dep;
}
