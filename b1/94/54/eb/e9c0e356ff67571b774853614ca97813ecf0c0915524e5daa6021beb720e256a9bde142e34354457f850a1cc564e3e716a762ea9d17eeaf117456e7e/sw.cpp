void build(Solution &s)
{
    auto &t = s.addLibrary("amazon.awslabs.c_common", "0.4.22");
    t += Git("https://github.com/awslabs/aws-c-common", "v{v}");

    t +=
        "include/.*"_rr,
        "source/.*"_rr;
    t -=
        "source/windows/.*"_rr,
        "source/posix/.*"_rr;
    t -= "source/arch/encoding_avx2.c";

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (t.getSettings().Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_COMMON_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_COMMON_USE_IMPORT_EXPORT"_d;
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "source/windows/.*"_rr;
        t.Public += "USE_WINDOWS_DLL_SEMANTICS"_d;
        t.Public += "WIN32"_d;
        t += "HAVE_MSVC_CPUIDEX"_d;
        t.Public += "bcrypt.lib"_lib;
    }
    else
    {
        t += "source/posix/.*"_rr;
        t += "HAVE_BUILTIN_CPU_SUPPORTS"_d;
    }
    t.writeFileOnce("aws/common/config.h");
}
