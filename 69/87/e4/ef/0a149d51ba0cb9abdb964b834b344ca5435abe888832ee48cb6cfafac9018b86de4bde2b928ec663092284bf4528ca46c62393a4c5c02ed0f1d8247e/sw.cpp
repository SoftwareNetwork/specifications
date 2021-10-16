void build(Solution &s)
{
    auto &t = s.addLibrary("amazon.awslabs.c_sdkutils", "0.1.1");
    t += Git("https://github.com/awslabs/aws-c-sdkutils", "v{v}");

    t +=
        "include/.*"_rr,
        "source/.*"_rr;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (t.getSettings().Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_SDKUTILS_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_SDKUTILS_USE_IMPORT_EXPORT"_d;

    t.Public += "org.sw.demo.amazon.awslabs.c_common"_dep;
}
