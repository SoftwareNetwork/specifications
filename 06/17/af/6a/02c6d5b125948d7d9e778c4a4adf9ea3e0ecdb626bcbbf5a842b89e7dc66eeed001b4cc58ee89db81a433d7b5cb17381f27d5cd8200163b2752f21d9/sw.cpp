void build(Solution &s)
{
    auto &allegro5 = s.addTarget<LibraryTarget>("liballeg.allegro5", "5.2.9.1");
    allegro5 += Git("https://github.com/liballeg/allegro5");
    {
        auto &t = allegro5;
        if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
            t.ExportAllSymbols = true;
        t.setChecks("x");

        t += "include/.*"_rr;
        t -= "src/.*"_rr;
        t += "src/.*"_r;
        //t += "src/gp2xwiz/.*"_rr;
        t += "src/misc/.*"_rr;
        t += "src/opengl/.*"_rr;
        t += "org.sw.demo.find.opengl-master"_dep;

        t.Public += sw::Static, "ALLEGRO_STATICLINK"_def;
        t += "ALLEGRO_SRC"_def;
        t.Protected += "ALLEGRO_LIB_BUILD"_def;

        t.Public += "include"_idir;

        t.Variables["ALLEGRO_LITTLE_ENDIAN"] = 1;

        //t += "src/sdl/.*"_rr;
        //t.Public += "org.sw.demo.valve.sdl"_dep;
        //t.Variables["ALLEGRO_SDL"] = 1;

        t.Public += "org.sw.demo.find.opengl-master"_dep;
        t.Variables["ALLEGRO_CFG_OPENGL"] = 1;

        //t -= "org.sw.demo.tronkko.dirent-master"_dep;
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            t += "src/win/.*"_rr;
            t += "UNICODE"_def;
            t += "_UNICODE"_def;
            //t += "c:/Program Files (x86)/Microsoft DirectX SDK (June 2010)/Include"_idir;
            //t += "dxguid.lib"_slib;
            t -= "src/win/d3d_d3dx9.cpp";
            //t -= "src/win/wjoydxnu.cpp";
            t.Variables["ALLEGRO_MSVC"] = 1;
            t.Variables["ALLEGRO_CFG_D3D"] = 1;
            //t.Variables["ALLEGRO_CFG_D3DX9"] = 1;
            //t.Variables["ALLEGRO_CFG_XINPUT"] = 1;

            t += "gdi32.lib"_slib;
            t += "user32.lib"_slib;
            t += "ole32.lib"_slib;
            t += "Winmm.lib"_slib;
            t += "Shell32.lib"_slib;
            t += "Shlwapi.lib"_slib;
        }
        else
        {
            t += "src/unix/.*"_rr;
            t.Variables["ALLEGRO_CFG_PTHREADS_TLS"] = 1;

            t += "ALLEGRO_HAVE_VA_COPY"_def;

            //t += "org.sw.demo.tronkko.dirent-master"_dep;
            //t += "ALLEGRO_HAVE_DIRENT_H"_def;
        }
        if (t.getBuildSettings().TargetOS.Type == OSType::Linux)
            t.Variables["ALLEGRO_LINUX"] = 1;
        if (t.getBuildSettings().TargetOS.Type == OSType::Macos)
        {
            t += "src/macosx/.*"_rr;
            t -= "src/unix/upath.c";
            t -= "src/unix/ukeybd.c";
            t -= "src/unix/umouse.c";
            t -= "src/unix/ukeybd.c";
            t -= "src/unix/udrvlist.c";
            t.Variables["ALLEGRO_DARWIN"] = 1;
            t.Variables["ALLEGRO_MACOSX"] = 1;
            t += "objc"_slib;
            t += "CoreFoundation"_framework;
            t += "CoreVideo"_framework;
            t += "IOKit"_framework;
            t += "OpenGL"_framework;
            t += "AppKit"_framework;
        }

        t.configureFile("include/allegro5/platform/alplatf.h.cmake", "allegro5/platform/alplatf.h");
    }

    auto &addons = allegro5.addDirectory("addons");

    auto &image = addons.addLibrary("image");
    {
        auto &t = image;
        if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
            t.ExportAllSymbols = true;
        t.setRootDirectory("addons/image");
        t -= ".*"_rr;
        t += "ALLEGRO_IIO_SRC"_def;
        t.Public += allegro5;

        t += "bmp.c";

        t += "png.c";
        t += "org.sw.demo.glennrp.png"_dep;
        t += "webp.c";
        t += "org.sw.demo.webmproject.webp"_dep;
        t += "jpg.c";
        t += "org.sw.demo.jpeg"_dep;

        t.Variables["ALLEGRO_CFG_WANT_NATIVE_IMAGE_LOADER"] = 1;

        t.Variables["ALLEGRO_CFG_IIO_HAVE_PNG"] = 1;
        t.Variables["ALLEGRO_CFG_IIO_HAVE_JPG"] = 1;
        t.Variables["ALLEGRO_CFG_IIO_HAVE_WEBP"] = 1;
        t.Variables["ALLEGRO_CFG_IIO_SUPPORT_PNG"] = 1;
        t.Variables["ALLEGRO_CFG_IIO_SUPPORT_JPG"] = 1;
        t.Variables["ALLEGRO_CFG_IIO_SUPPORT_WEBP"] = 1;

        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            t.Variables["ALLEGRO_CFG_IIO_HAVE_GDIPLUS"] = 1;
            //t.Variables["ALLEGRO_CFG_IIO_HAVE_GDIPLUS_LOWERCASE_H"] = 1;
        }
        if (t.getBuildSettings().TargetOS.Type == OSType::Macos)
        {
            t += "macosx.m";
            t += "objc"_slib;
            t += "AppKit"_framework;
        }

        t.configureFile("allegro5/internal/aintern_image_cfg.h.cmake", "allegro5/internal/aintern_image_cfg.h");
    }

    auto &primitives = addons.addLibrary("primitives");
    {
        auto &t = primitives;
        if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
            t.ExportAllSymbols = true;
        t.setRootDirectory("addons/primitives");
        t += ".*"_rr;
        t -= "nshader.cpp";
        if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
             t -= "directx_shaders.cpp";
        t += "ALLEGRO_PRIMITIVES_SRC"_def;
        t.Public += allegro5;
    }

    auto &main = addons.addLibrary("main");
    {
        auto &t = main;
        if (t.getBuildSettings().TargetOS.Type != OSType::Windows)
            t.ExportAllSymbols = true;
        t.setRootDirectory("addons/main");
        t += ".*"_rr;
        t.Public += allegro5;
    }
}

void check(Checker &c)
{
    auto &s = c.addSet("x");
    s.checkFunctionExists("mmap");
    s.checkFunctionExists("fseeki64");
    s.checkFunctionExists("ftelli64");
    s.checkIncludeExists("assert.h");
    s.checkIncludeExists("dirent.h");
    s.checkIncludeExists("stdint.h");
    s.checkIncludeExists("inttypes.h");
    s.checkIncludeExists("pthread.h");
    s.checkIncludeExists("time.h");
    s.checkIncludeExists("sys/time.h");
    s.checkIncludeExists("sys/stat.h");
    s.checkIncludeExists("sys/types.h");
    s.checkIncludeExists("sys/io.h");
    s.checkIncludeExists("sys/utsname.h");
    s.checkIncludeExists("sys/procfs.h");
    s.checkIncludeExists("sys/ioctl.h");
    s.checkIncludeExists("sal.h");
    s.checkIncludeExists("stdbool.h");

    for (auto &check : s.all)
        check->Prefixes.insert("ALLEGRO_");
}
