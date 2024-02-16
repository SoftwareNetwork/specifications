void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.c_cal", "0.6.10");
    t += Git("https://github.com/awslabs/aws-c-cal", "v{v}");

    t -=
        "include/.*"_rr,
        "source/.*"_rr;
    t += "source/.*"_r;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_CAL_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_CAL_USE_IMPORT_EXPORT"_d;

    //t -= "org.sw.demo.amazon.awslabs.lc-main"_dep;
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t.Public += "WIN32"_def;
        t += "source/windows/.*"_rr;
        t += "bcrypt.lib"_slib;
        /*t += "advapi32.lib"_slib;
        t += "Crypt32.lib"_slib;
        t += "Secur32.lib"_slib;
        t += "Shlwapi.lib"_slib;
        t += "ws2_32.lib"_slib;*/
    }
    else
    {
        t += "source/unix/.*"_rr;
        t += "dl"_slib;
        //t += "org.sw.demo.amazon.awslabs.lc-main"_dep;
    }

    if (t.getBuildSettings().TargetOS.isApple()) {
        t -= "source/unix/.*"_rr;
        t += "source/darwin/.*"_rr;
        t += "CoreFoundation"_framework;
        t += "Security"_framework;
        //t += "OPENSSL_IS_AWSLC"_def;
    }

    t += "org.sw.demo.openssl.crypto"_dep;
    t.Public += "org.sw.demo.amazon.awslabs.c_common"_dep;
    //t.Public += "org.sw.demo.amazon.awslabs.c_common"_dep;
}
