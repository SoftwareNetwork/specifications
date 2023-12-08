void build(Solution &s)
{
    // sdl cannot load vk as static lib
    auto &t = s.addTarget<SharedLibrary>("khronos.vulkan", "1.3.272");
    t += Git("https://github.com/KhronosGroup/Vulkan-Loader", "v{v}");

    t.setChecks("vk");

    t += "loader/.*"_r;
    t += "scripts/.*"_r;
    t -= "loader/generated/.*"_rr;
    t -= "loader/asm_offset.c";
    t -= "loader/.*\\.asm"_r;

    t += "loader"_idir;
    t += "loader/generated"_idir;

    t += "API_NAME=\"Vulkan\""_def;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "VK_USE_PLATFORM_WIN32_KHR"_def;

        t += "Cfgmgr32.lib"_slib;
        t += "advapi32.lib"_slib;

        t.patch("loader/vulkan-1.def", "LIBRARY vulkan-1.dll", "");

        // only when shared
        t.Interface += Definition("SW_VK_LIBRARY_NAME=\"" + to_printable_string(t.getOutputFile().filename()) + "\"");
    }

    auto hdrs = "org.sw.demo.khronos.vulkan.headers"_dep;
    hdrs->package.range = t.getPackage().getVersion();
    t += hdrs; // add to pvt because dummy dep is addded below
    t.Public += hdrs;

    //t.configureFile("loader/loader_cmake_config.h.in", "loader_cmake_config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("vk");
    s.checkFunctionExists("secure_getenv");
    s.checkFunctionExists("__secure_getenv");
}
