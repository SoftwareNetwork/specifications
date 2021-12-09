void build(Solution &s)
{
    auto &glfw = s.addTarget<LibraryTarget>("glfw", "3.3.6");
    glfw += Git("https://github.com/glfw/glfw", "{M}.{m}{po}");

    glfw +=
        "deps/glad/.*"_rr,
        "include/.*"_rr,
        "src/.*\\.c"_rr,
        "src/.*\\.h"_rr,
        "src/.*\\.h.in"_rr,
        "src/.*\\.m"_rr;

    glfw.Private +=
        "src"_id;

    glfw.Public +=
        "include"_id,
        "deps"_id;

    glfw.ApiName = "GLAPI";
    glfw.Private += "_GLFW_USE_CONFIG_H"_d;
    glfw.Interface += sw::Shared, "GLFW_DLL"_d;
    glfw.Private += sw::Shared, "_GLFW_BUILD_DLL"_d;

    glfw.Public += "org.sw.demo.khronos.vulkan"_dep;

    if (glfw.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        glfw -= "src/posix.*"_rr;
        glfw -= "src/cocoa.*"_rr;
        glfw -= "src/x11.*"_rr;
        glfw -= "src/linux.*"_rr;
        glfw -= "src/wl.*"_rr;
        glfw -= "src/glx.*"_rr;
        glfw -= "src/null.*"_rr;

        glfw += "User32.lib"_slib;
        glfw += "gdi32.lib"_slib;
        glfw += "Shell32.lib"_slib;

        glfw.Variables["_GLFW_WIN32"] = 1;
    }
    //_GLFW_X11
    //_GLFW_COCOA

    glfw.configureFile("src/glfw_config.h.in", "glfw_config.h");
}
