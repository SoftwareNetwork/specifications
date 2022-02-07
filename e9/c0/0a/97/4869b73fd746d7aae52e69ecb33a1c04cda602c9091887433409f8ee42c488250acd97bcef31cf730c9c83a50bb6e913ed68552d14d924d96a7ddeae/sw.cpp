void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.c_auth", "0.6.11");
    t += Git("https://github.com/awslabs/aws-c-auth", "v{v}");

    t -=
        "include/.*"_rr,
        "source/.*"_rr;
    t += "source/.*"_rr;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_AUTH_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_AUTH_USE_IMPORT_EXPORT"_d;

    t.Public += "org.sw.demo.amazon.awslabs.c_http"_dep;
    t.Public += "org.sw.demo.amazon.awslabs.c_sdkutils"_dep;
}
