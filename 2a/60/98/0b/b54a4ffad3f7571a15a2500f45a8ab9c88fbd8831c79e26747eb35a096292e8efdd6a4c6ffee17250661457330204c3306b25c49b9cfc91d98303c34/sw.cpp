void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.c_http", "0.7.14");
    t += Git("https://github.com/awslabs/aws-c-http", "v{v}");

    t -=
        "include/.*"_rr,
        "source/.*"_rr;
    t += "source/.*"_r;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_HTTP_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_HTTP_USE_IMPORT_EXPORT"_d;

    t.Public += "org.sw.demo.amazon.awslabs.c_io"_dep;
    t.Public += "org.sw.demo.amazon.awslabs.c_compression"_dep;
}
