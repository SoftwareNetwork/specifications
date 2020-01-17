void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.xcb", "1.13.1");
    t += Git("https://gitlab.freedesktop.org/xorg/lib/libxcb");

    t.ExportAllSymbols = true;

    t.setChecks("xcb");

    t += "src/.*\\.[hc]"_r;

    t += "XCB_QUEUE_BUFFER_SIZE=16384"_def;
    if (!t.Variables["HAVE_DECL_IOV_MAX"])
    {
        if (t.Variables["HAVE_DECL_UIO_MAXIOV"])
            t += "IOV_MAX=UIO_MAXIOV"_def;
        else
            t += "IOV_MAX=16"_def;
    }

    t.Public += "org.sw.demo.xorg.X11.auth"_dep;

    auto generate = [&t](const String &name)
    {
        auto c = t.addCommand();
        c
            << cmd::wdir(t.BinaryDir)
            << cmd::prog("org.sw.demo.python.exe"_dep)
            << cmd::in("src/c_client.py")
            << "-c" << ""
            << "-l" << ""
            << "-s" << ""
            << "-p" << t.getFile("org.sw.demo.xorg.proto.xcb"_dep)
            << cmd::in(t.getFile("org.sw.demo.xorg.proto.xcb"_dep, "src/" + name + ".xml"))
            << cmd::end()
            << cmd::out(name + ".h")
            << cmd::out(name + ".c")
            ;
    };

    for (auto &f : enumerate_files_like(t.getFile("org.sw.demo.xorg.proto.xcb"_dep) / "src", ".*\\.xml", false))
        generate(f.stem().string());

    t.configureFile("src/xcb.h", "xcb/xcb.h", ConfigureFlags::CopyOnly);
    t.configureFile("src/xcbext.h", "xcb/xcbext.h", ConfigureFlags::CopyOnly);
}

void check(Checker &c)
{
    auto &s = c.addSet("xcb");

    auto &c1 = s.checkDeclarationExists("IOV_MAX");
    c1.Parameters.Includes.push_back("limits.h");
    auto &c2 = s.checkDeclarationExists("UIO_MAXIOV");
    c2.Parameters.Includes.push_back("sys/uio.h");
}
