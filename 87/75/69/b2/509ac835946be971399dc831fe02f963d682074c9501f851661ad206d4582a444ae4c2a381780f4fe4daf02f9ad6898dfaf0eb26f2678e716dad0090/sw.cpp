void build(Solution &s)
{
    // sdl cannot load vk as static lib
    auto &t = s.addTarget<SharedLibrary>("khronos.vulkan", "1.1.106");
    t += Git("https://github.com/KhronosGroup/Vulkan-Loader", "v{v}");

    t.setChecks("vk");

    t += "loader/.*"_r;
    t += "scripts/.*"_r;
    t -= "loader/vk_loader_extensions.c";
    t -= "loader/asm_offset.c";
    t -= "loader/.*\\.asm"_r;

    t += "loader"_idir;

    t += "API_NAME=\"Vulkan\""_def;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        t += "VK_USE_PLATFORM_WIN32_KHR"_def;

        t += "Cfgmgr32.lib"_slib;
        t += "advapi32.lib"_slib;

        t.patch("loader/vulkan-1.def", "LIBRARY vulkan-1.dll", "");

        // only when shared
        t.Interface += Definition("SW_VK_LIBRARY_NAME=\"" + t.getOutputFile().filename().u8string() + "\"");
    }

    auto hdrs = "org.sw.demo.khronos.vulkan.headers"_dep;
    hdrs->package.range = t.getPackage().version;
    t.Public += hdrs;

    auto GenerateFromVkXml = [&t, &hdrs](const path &output, const path &dep)
    {
        auto c = t.addCommand();
        c << cmd::prog("org.sw.demo.python.exe-3"_dep)
            << cmd::wdir(t.BinaryDir)
            << cmd::in("scripts/loader_genvk.py")
            << "-registry"
            << cmd::in(t.getFile(hdrs, "registry/vk.xml"))
            << "-scripts"
            << t.getFile(hdrs, "registry")
            << output
            << cmd::end()
            << cmd::in(path("scripts") / dep)
            << cmd::out(output)
            // deps:
            // ${VulkanRegistry_DIR}/generator.py
            // ${VulkanRegistry_DIR}/reg.py
            ;
    };

    GenerateFromVkXml("vk_layer_dispatch_table.h", "loader_extension_generator.py");
    GenerateFromVkXml("vk_dispatch_table_helper.h", "dispatch_table_helper_generator.py");
    GenerateFromVkXml("vk_safe_struct.h", "helper_file_generator.py");
    GenerateFromVkXml("vk_safe_struct.cpp", "helper_file_generator.py");
    GenerateFromVkXml("vk_enum_string_helper.h", "helper_file_generator.py");
    GenerateFromVkXml("vk_object_types.h", "helper_file_generator.py");
    GenerateFromVkXml("vk_extension_helper.h", "helper_file_generator.py");
    GenerateFromVkXml("vk_typemap_helper.h", "helper_file_generator.py");

    GenerateFromVkXml("vk_loader_extensions.h", "loader_extension_generator.py");
    GenerateFromVkXml("vk_loader_extensions.c", "loader_extension_generator.py");
    t -= "vk_loader_extensions.c";

    t.configureFile("loader/loader_cmake_config.h.in", "loader_cmake_config.h");
}

void check(Checker &c)
{
    auto &s = c.addSet("vk");
    s.checkFunctionExists("secure_getenv");
    s.checkFunctionExists("__secure_getenv");
}
