void build(Solution &s)
{
    auto &t = s.addLibrary("amazon.awslabs.c_event_stream", "0.5.2");
    t += Git("https://github.com/awslabs/aws-c-event-stream", "v{v}");

    t +=
        "include/.*"_rr,
        "source/.*"_rr;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_EVENT_STREAM_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_EVENT_STREAM_USE_IMPORT_EXPORT"_d;
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t.Public += "USE_WINDOWS_DLL_SEMANTICS"_d;
        t.Public += "WIN32"_d;
    }

    t.Public +=
        "org.sw.demo.amazon.awslabs.c_io"_dep,
        "org.sw.demo.amazon.awslabs.checksums"_dep;
}
