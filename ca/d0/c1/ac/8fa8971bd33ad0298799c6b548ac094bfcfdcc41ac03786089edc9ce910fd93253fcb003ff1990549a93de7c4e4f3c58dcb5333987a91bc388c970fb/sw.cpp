void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.c_io", "0.10.3");
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

    t -= "org.sw.demo.amazon.awslabs.s2n"_dep;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "source/windows/.*"_rr;
        t.Public += "AWS_USE_IO_COMPLETION_PORTS"_d;
        t += "advapi32.lib"_slib;
        t += "Crypt32.lib"_slib;
        t += "Secur32.lib"_slib;
        t += "Shlwapi.lib"_slib;
        t += "ws2_32.lib"_slib;
        t += "Ncrypt.lib"_slib;
    }
    else
    {
        t += "source/linux/.*"_rr;
        t += "source/posix/.*"_rr;
        t += "dl"_slib;
        t += "source/s2n/.*"_r;
        t += "org.sw.demo.amazon.awslabs.s2n"_dep;
    }

    t.Public += "org.sw.demo.amazon.awslabs.c_cal"_dep;
}
