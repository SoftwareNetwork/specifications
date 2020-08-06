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

        t.Public += "include"_idir;
        t += "src"_idir;

        t += "org.sw.demo.g_truc.glm"_dep;
        t += "org.sw.demo.libusb.hidapi-master"_dep;

        if (t.getBuildSettings().TargetOS.is(OSType::Windows) || t.getBuildSettings().TargetOS.is(OSType::Mingw))
        {
            t += "src/platform/psmove_port_windows.c";
            t.Protected += "WIN32_LEAN_AND_MEAN"_def;
            t += "NOMINMAX"_def;
            t += "advapi32.lib"_slib;
            t += "Bthprops.lib"_slib;
            t += "uuid.lib"_slib;
            t += "ws2_32.lib"_slib;
        }
        else if (t.getBuildSettings().TargetOS.isApple())
            t += "src/platform/psmove_port_osx.mm";
        else
        {
            t.ExportAllSymbols = true;
            t += "src/platform/psmove_port_linux.cpp";
            t += "src/daemon/moved_monitor_linux.c";
            t += "/usr/include/dbus-1.0"_idir;
            t += "/usr/lib64/dbus-1.0/include"_idir;
            t += "dbus-1"_slib;
            t += "udev"_slib;
            t += "bluetooth"_slib;
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
        cli -= "org.sw.demo.tronkko.dirent-master"_dep;
        if (cli.getBuildSettings().TargetOS.is(OSType::Windows) || cli.getBuildSettings().TargetOS.is(OSType::Mingw))
        {
            cli += "ws2_32.lib"_slib;
            cli += "org.sw.demo.tronkko.dirent-master"_dep;
        }
    }
}
