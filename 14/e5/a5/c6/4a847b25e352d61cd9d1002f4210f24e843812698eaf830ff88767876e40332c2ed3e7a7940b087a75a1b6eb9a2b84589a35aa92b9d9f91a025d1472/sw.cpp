void build(Solution &s)
{
    auto &t = s.addLibrary("amazon.awslabs.checksums", "0.1.5");
    t += Git("https://github.com/awslabs/aws-checksums", "v{v}");

    t +=
        "include/.*"_rr,
        "source/.*"_rr;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_CHECKSUMS_EXPORTS"_d;
    t.Public += sw::Shared, "USE_IMPORT_EXPORT"_d;
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t.Public += "USE_WINDOWS_DLL_SEMANTICS"_d;
        t.Public += "WIN32"_d;
    }
    else
        t -= "source/visualc/.*"_rr;

    t.replaceInFileOnce("source/intel/crc32c_sse42_asm.c", "+c", "+g");
    t.replaceInFileOnce("source/intel/crc32c_sse42_asm.c", "+d", "+g");
}
