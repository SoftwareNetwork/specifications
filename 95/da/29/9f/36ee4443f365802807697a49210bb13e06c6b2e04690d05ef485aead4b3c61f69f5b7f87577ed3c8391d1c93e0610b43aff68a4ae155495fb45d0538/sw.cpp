void build(Solution &s)
{
    auto &t = s.addLibrary("amazon.awslabs.c_common", "0.9.24");
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
        t.Public += "WIN32"_def; // some libs still missing _WIN32 def and use WIN32
        t += "bcrypt.lib"_slib;
        t += "Shlwapi.lib"_slib;
    }
    else
    {
        t += "source/posix/.*"_rr;
        t += "source/arch/intel/asm/.*"_r;
        t -= "source/arch/intel/encoding_avx2.c";
        if (t.getBuildSettings().TargetOS.isApple())
            t += "AWS_AFFINITY_METHOD=AWS_AFFINITY_METHOD_NONE"_def;
        else
            t += "AWS_AFFINITY_METHOD=AWS_AFFINITY_METHOD_PTHREAD_ATTR"_def;
        t += "dl"_slib;
        t += "pthread"_slib;
        t += "CoreFoundation"_framework;
    }
    if (t.getBuildSettings().TargetOS.Arch == ArchType::aarch64) {
        t -= "source/arch/intel/.*"_rr;
        t += "source/arch/arm/asm/.*"_r;
    }
    t.writeFileOnce("aws/common/config.h");
}
