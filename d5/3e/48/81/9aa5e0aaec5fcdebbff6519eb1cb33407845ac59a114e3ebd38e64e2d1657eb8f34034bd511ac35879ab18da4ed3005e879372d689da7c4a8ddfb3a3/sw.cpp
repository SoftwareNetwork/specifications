void build(Solution &s)
{
    auto &t = s.addLibrary("amazon.awslabs.c_common", "0.6.2");
    t += Git("https://github.com/awslabs/aws-c-common", "v{v}");

    t +=
        "include/.*"_rr,
        "source/.*"_rr;
    t -=
        "source/android/.*"_rr,
        "source/windows/.*"_rr,
        "source/posix/.*"_rr;
    t -= "source/arch/.*"_rr;
    t += "source/arch/intel/.*"_r;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (t.getSettings().Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_COMMON_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_COMMON_USE_IMPORT_EXPORT"_d;
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t += "source/windows/.*"_rr;
        t += "source/arch/intel/msvc/.*"_r;
        t += "bcrypt.lib"_slib;
    }
    else
    {
        t += "source/posix/.*"_rr;
        t += "source/arch/intel/asm/.*"_r;
        t -= "source/arch/intel/encoding_avx2.c";
        t += "dl"_slib;
        t += "pthread"_slib;
        t += "CoreFoundation"_framework;
    }
    t.writeFileOnce("aws/common/config.h");
}
