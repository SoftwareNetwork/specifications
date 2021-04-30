void build(Solution &s)
{
    auto &t = s.addTarget<Library>("anholt.libepoxy", "1.5.6");
    t += Git("https://github.com/anholt/libepoxy");

    t.ApiName = "EPOXY_PUBLIC";
    t += "include/.*"_rr;
    t += "registry/.*"_rr;
    t += "src/.*"_rr;

    t.writeFileOnce(t.BinaryPrivateDir / "config.h");

    auto output_dir = t.BinaryDir / "epoxy";

    // hdr
    for (auto [gs, r, deps] : {
        std::tuple{"gl_generated.h","gl",Strings{"gl.h"}},
        std::tuple{"egl_generated.h","egl",Strings{"egl.h"}},
        std::tuple{"glx_generated.h","glx",Strings{"glx.h"}},
        std::tuple{"wgl_generated.h","wgl",Strings{"wgl.h"}},
        })
    {
        if (t.getBuildSettings().TargetOS.is(OSType::Windows) || t.getBuildSettings().TargetOS.is(OSType::Macos))
        {
            if (r == "egl" || r == "glx")
                continue;
        }
        auto c = t.addCommand();
        c << cmd::prog("org.sw.demo.python.exe-3"_dep)
            << cmd::in("src/gen_dispatch.py")
            << "--header"
            << "--no-source"
            << ("--outputdir="s + to_string(to_path_string(output_dir)))
            << cmd::in("registry/"s + r + ".xml")
            << cmd::end()
            << cmd::out(output_dir / gs)
            ;
        for (auto &f : deps)
            c << cmd::in("include/epoxy/"s + f);
    }

    // src
    for (auto [gs, r, deps] : {
        std::tuple{"gl_generated_dispatch.c","gl",Strings{"dispatch_common.c","dispatch_common.h"}},
        std::tuple{"egl_generated_dispatch.c","egl",Strings{"dispatch_egl.c"}},
        std::tuple{"glx_generated_dispatch.c","glx",Strings{"dispatch_glx.c"}},
        std::tuple{"wgl_generated_dispatch.c","wgl",Strings{"dispatch_wgl.c"}},
        })
    {
        if (t.getBuildSettings().TargetOS.is(OSType::Windows) || t.getBuildSettings().TargetOS.is(OSType::Macos))
        {
            if (r == "egl" || r == "glx")
                continue;
        }
        auto c = t.addCommand();
        c << cmd::prog("org.sw.demo.python.exe-3"_dep)
            << cmd::in("src/gen_dispatch.py")
            << "--source"
            << "--no-header"
            << ("--outputdir="s + to_string(to_path_string(output_dir)))
            << cmd::in("registry/"s + r + ".xml")
            << cmd::end()
            << cmd::out(output_dir / gs)
            ;
        for (auto &f : deps)
            c << cmd::in("src/"s + f);
    }

    if (t.getBuildSettings().TargetOS.is(OSType::Windows) || t.getBuildSettings().TargetOS.is(OSType::Macos))
    {
        t -= ".*egl.*"_rr;
        t -= ".*glx.*"_rr;
    }
}
