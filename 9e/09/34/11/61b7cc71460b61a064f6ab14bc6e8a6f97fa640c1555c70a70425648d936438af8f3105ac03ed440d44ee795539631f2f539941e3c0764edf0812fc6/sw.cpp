void build(Solution &s)
{
    auto &t = s.addLibrary("amazon.awslabs.c_event_stream", "master");
    t += Git("https://github.com/awslabs/aws-c-event-stream", "", "{v}");

    t +=
        "include/.*"_rr,
        "source/.*"_rr;

    if (s.Settings.Native.CompilerType == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
        //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_EVENT_STREAM_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_EVENT_STREAM_USE_IMPORT_EXPORT"_d;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        t.Public += "USE_WINDOWS_DLL_SEMANTICS"_d;
        t.Public += "WIN32"_d;
    }

    t.Public +=
        "org.sw.demo.amazon.awslabs.c_common-master"_dep,
        "org.sw.demo.amazon.awslabs.checksums-master"_dep;
}

