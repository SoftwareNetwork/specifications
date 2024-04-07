void build(Solution &s)
{
    auto &p = s.addProject("xorg.X11", "1.8.9");
    p += Git("https://gitlab.freedesktop.org/xorg/lib/libx11", "libX11-{M}.{m}{po}");

    auto &xlib = p.addLibrary("Xlib");

    auto &mk = xlib.addExecutable("util.makekeys");
    mk += "src/util/makekeys.c";

    // xlib
    {
        auto &t = xlib;
        t.ExportAllSymbols = true;

        t.setChecks("x", true);

        t += "include/.*"_rr;
        t += "src/.*"_r;

        t -= "src/os2Stubs.c";
        //t -= "src/UIThrStubs.c";

        t.Public += "include"_idir;
        t += "include/X11"_idir;
        t += "src"_idir;

        // xcms
        t += "src/xcms/.*"_rr;
        t += Definition("XCMSDIR=\"" + to_string(normalize_path(t.SourceDir / "src/xcms")) + "\"");
        t += "src/xcms"_idir;

        // xlibi18n
        t += "src/xlibi18n/.*"_r;
        t -= "src/xlibi18n/lcUniConv/.*"_rr;
        t += "XLOCALELIBDIR=\".\""_def;
        t += "src/xlibi18n"_idir;
        //

        //
        t += "modules/im/.*\\.[hc]"_rr;
        t += "modules/lc/.*\\.[hc]"_rr;
        t += "modules/om/.*\\.[hc]"_rr;
        t -= "modules/im/ximcp/imTrans.c";
        //

        //
        t.Public += "org.sw.demo.xorg.xcb"_dep;
        t += "org.sw.demo.xorg.xtrans"_dep;

        t.Variables["XTHREADS"] = 1;
        t.Variables["XUSE_MTSAFE_API"] = 1;
        t.configureFile("include/X11/XlibConf.h.in", "X11/XlibConf.h", ConfigureFlags::EnableUndefReplacements);

        if (t.getCompilerType() == CompilerType::GNU)
            t.LinkOptions.push_back("-Wno-undef");
        else
            t.LinkOptions.push_back("--no-undefined");
        t += "dl"_slib;

        auto c = t.addCommand();
        c << cmd::prog(mk);
        for (auto &f : {
            "keysymdef.h"s,
            "XF86keysym.h"s,
            "Sunkeysym.h"s,
            "DECkeysym.h"s,
            "HPkeysym.h"s,
            })
        {
            auto f2 = t.getFile("org.sw.demo.xorg.proto"_dep, "include/X11/" + f);
            if (fs::exists(f2))
            {
                c << cmd::in(f2);
            }
        }
        c << cmd::std_out("ks_tables.h");

        auto win_or_mingw =
            t.getBuildSettings().TargetOS.Type == OSType::Windows ||
            t.getBuildSettings().TargetOS.Type == OSType::Mingw
            ;
        if (win_or_mingw) {
            t.Public += "WIN32"_def;
        }
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("x");
    s.checkIncludeExists("dl.h");
    s.checkIncludeExists("dlfcn.h");
    s.checkIncludeExists("sys/filio.h");
    s.checkIncludeExists("sys/select.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sys/socket.h");
    s.checkIncludeExists("unistd.h");
    s.checkFunctionExists("strtol");
    s.checkFunctionExists("seteuid");
    s.checkDeclarationExists("issetugid");
    s.checkFunctionExists("issetugid");
    s.checkFunctionExists("getresuid");
    s.checkFunctionExists("shmat");
    //s.checkLibraryFunctionExists("getpwuid_r", "getpwuid_r.c");
    s.checkLibraryFunctionExists("pthread", "pthread_self");
    s.checkFunctionExists("poll");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("nl_langinfo");
}
