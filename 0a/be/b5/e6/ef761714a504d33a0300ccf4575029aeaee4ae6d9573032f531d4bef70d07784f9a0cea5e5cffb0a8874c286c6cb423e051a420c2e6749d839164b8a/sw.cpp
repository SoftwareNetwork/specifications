void build(Solution &s)
{
    auto &p = s.addProject("thp.psmove", "master");
    p += Git("https://github.com/thp/psmoveapi");

    auto &api = p.addSharedLibrary("api");
    {
        auto &t = api;

        t += "include/.*"_rr;
        t += "src/.*"_r;
        t += "src/daemon/.*"_r;
        t += "src/math/.*"_r;

        t -= "src/platform/.*"_r;
        t -= "src/daemon/moved_monitor_linux.c";

        t += "BUILDING_SHARED_LIBRARY"_def;
        t += "_USE_MATH_DEFINES"_def;

        t += "org.sw.demo.g_truc.glm"_dep;
        t += "org.sw.demo.signal11.hidapi-master"_dep;

        if (t.getBuildSettings().TargetOS.is(OSType::Windows))
        {
            t += "src/platform/psmove_port_windows.c";
            t.Protected += "WIN32_LEAN_AND_MEAN"_def;
            t += "NOMINMAX"_def;
            t += "advapi32.lib"_slib;
            t += "Bthprops.lib"_slib;
            t += "uuid.lib"_slib;
            t += "ws2_32.lib"_slib;
        }

        t.Variables["PSMOVE_BUILD_TRACKER"] = 1;
        t.Variables["PSMOVE_USE_PSEYE"] = 1;
        t.Variables["PSMOVE_USE_SIXPAIR"] = 1;

        t.Variables["PSMOVEAPI_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
        t.Variables["PSMOVEAPI_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
        t.Variables["PSMOVEAPI_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];

        t.configureFile("include/psmove_config.h.in", "psmove_config.h");
    }

    auto &cli = p.addExecutable("cli");
    {
        cli += "src/utils/psmovecli.cpp";
        cli += api;
        cli += "org.sw.demo.libusb-0"_dep;
        cli += "org.sw.demo.tronkko.dirent-master"_dep;
        if (cli.getBuildSettings().TargetOS.is(OSType::Windows))
            cli += "ws2_32.lib"_slib;
    }
}
