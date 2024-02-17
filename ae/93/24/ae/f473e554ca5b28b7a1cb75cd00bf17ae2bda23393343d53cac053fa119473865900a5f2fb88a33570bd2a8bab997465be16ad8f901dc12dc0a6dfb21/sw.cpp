void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("amazon.awslabs.crt_cpp", "0.26.2");
    t += Git("https://github.com/awslabs/aws-crt-cpp", "v{v}");

    t -=
        "include/.*"_rr,
        "source/.*"_rr;
    t += "source/.*"_rr;

    if (t.getCompilerType() == CompilerType::MSVC)
        t.CompileOptions.push_back("-bigobj");
    //else if (s.Settings.Native.CompilerType == CompilerType::GNU)
    //t.CompileOptions.push_back("-Wa,-mbig-obj");

    t.Private += sw::Shared, "AWS_CRT_CPP_EXPORTS"_d;
    t.Public += sw::Shared, "AWS_CRT_CPP_USE_IMPORT_EXPORT"_d;

    t.Public += "org.sw.demo.amazon.awslabs.c_auth"_dep;
    t.Public += "org.sw.demo.amazon.awslabs.c_event_stream"_dep;
    t.Public += "org.sw.demo.amazon.awslabs.c_mqtt"_dep;
    t.Public += "org.sw.demo.amazon.awslabs.c_s3"_dep;
    t.Public += "org.sw.demo.amazon.awslabs.c_sdkutils"_dep;

    t.Variables["AWS_CRT_CPP_VERSION"] = t.Variables["PACKAGE_VERSION"];
    t.Variables["PROJECT_VERSION_MAJOR"] = t.Variables["PACKAGE_VERSION_MAJOR"];
    t.Variables["PROJECT_VERSION_MINOR"] = t.Variables["PACKAGE_VERSION_MINOR"];
    t.Variables["PROJECT_VERSION_PATCH"] = t.Variables["PACKAGE_VERSION_PATCH"];
    t.configureFile("include/aws/crt/Config.h.in", "aws/crt/Config.h");
}
