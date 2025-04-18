void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.c_compression", "0.3.0");
    t += Git("https://github.com/awslabs/aws-c-compression", "v{v}");

    t -=
        "include/.*"_rr,
        "source/.*"_rr;
    t += "source/.*"_r;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_COMPRESSION_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_COMPRESSION_USE_IMPORT_EXPORT"_d;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t.Public += "WIN32"_def;
    }
    t.Public += "org.sw.demo.amazon.awslabs.c_common"_dep;
}
