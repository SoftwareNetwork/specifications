void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("amazon.awslabs.c_io", "0.3.0");
    t += Git("https://github.com/awslabs/aws-c-io", "v{v}");

    t -=
        "include/.*"_rr,
        "source/.*"_rr;
    t += "source/.*"_r;

    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_IO_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_IO_USE_IMPORT_EXPORT"_d;

    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        t += "source/windows/.*"_rr;
        t.Public += "AWS_USE_IO_COMPLETION_PORTS"_d;
    }
    else
    {
        t += "source/posix/.*"_rr;
    }

    t += "org.sw.demo.amazon.awslabs.c_common"_dep;
}
