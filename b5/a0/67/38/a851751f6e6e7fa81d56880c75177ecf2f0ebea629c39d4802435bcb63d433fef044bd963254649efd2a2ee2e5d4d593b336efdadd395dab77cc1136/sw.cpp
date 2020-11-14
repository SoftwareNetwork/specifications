void build(Solution &s)
{
    auto &t = s.addLibrary("amazon.awslabs.checksums", "0.1.10");
    t += Git("https://github.com/awslabs/aws-checksums", "v{v}");

    t -=
        "include/.*"_rr,
        "source/.*"_rr;
    t += "source/.*"_r;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_CHECKSUMS_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_CHECKSUMS_USE_IMPORT_EXPORT"_d;
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "source/intel/visualc/.*"_rr;
    }

    t.Public += "org.sw.demo.amazon.awslabs.c_common"_dep;
}
