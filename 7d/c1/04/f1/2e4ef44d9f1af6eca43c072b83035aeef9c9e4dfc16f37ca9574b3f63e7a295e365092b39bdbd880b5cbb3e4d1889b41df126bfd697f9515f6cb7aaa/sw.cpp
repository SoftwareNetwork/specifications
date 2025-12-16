void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.c_io", "0.24.0");
    t += Git("https://github.com/awslabs/aws-c-io", "v{v}");

    t -=
        "include/.*"_rr,
        "source/.*"_rr;
    t += "source/.*"_r;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_IO_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_IO_USE_IMPORT_EXPORT"_d;

    t += "org.sw.demo.intel.ittnotify"_dep;
    t -= "org.sw.demo.amazon.awslabs.s2n"_dep;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "source/windows/.*"_rr;
        //t.Public += "AWS_USE_IO_COMPLETION_PORTS"_d;
        t.Public += "AWS_ENABLE_IO_COMPLETION_PORTS"_d;
        t.Public += "AWS_OS_WINDOWS"_d;
        t += "advapi32.lib"_slib;
        t += "Crypt32.lib"_slib;
        t += "Secur32.lib"_slib;
        t += "Shlwapi.lib"_slib;
        t += "ws2_32.lib"_slib;
        t += "Ncrypt.lib"_slib;
    }
    else
    {
        t += "source/posix/.*"_rr;
        t += "dl"_slib;
    }
    if (t.getBuildSettings().TargetOS.Type == OSType::Linux) {
        t += "source/linux/.*"_rr;
        t += "source/s2n/.*"_r;
        t += "org.sw.demo.amazon.awslabs.s2n"_dep;
        t += "USE_S2N=1"_def;
        t.Public += "AWS_ENABLE_EPOLL"_def;
    }
    else
    {
        t += "USE_S2N=0"_def;
    }
    if (t.getBuildSettings().TargetOS.isApple()) {
        //t += "source/bsd/.*"_rr;
        t += "source/bsd/kqueue_event_loop.c";
        t += "source/darwin/.*"_rr;
        t += "CoreFoundation"_framework;
        t += "Security"_framework;
        t.Public += "AWS_ENABLE_KQUEUE"_def;
    }

    t.Public += "org.sw.demo.amazon.awslabs.c_cal"_dep;
}
