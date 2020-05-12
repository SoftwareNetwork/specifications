void build(Solution &s)
{
    auto &sdk = s.addProject("amazon.aws.sdk", "1.7.333");
    sdk += Git("https://github.com/aws/aws-sdk-cpp", "{v}");

    auto add_target = [&sdk, &s](const String &name) -> decltype(auto)
    {
        auto &t = sdk.addTarget<LibraryTarget>(name);
        t.setRootDirectory("aws-cpp-sdk-" + name);
        t +=
            "include/.*"_rr,
            "source/.*"_rr;

        if (t.getCompilerType() == CompilerType::MSVC)
            t.CompileOptions.push_back("-bigobj");
        //else if (t.getSettings().Native.CompilerType == CompilerType::GNU)
        //t.CompileOptions.push_back("-Wa,-mbig-obj");

        Definition d;
        d.d = "AWS_" + boost::to_upper_copy(name) + "_EXPORTS";
        t.Private += sw::Shared, d;
        t.Public += sw::Shared, "USE_IMPORT_EXPORT"_d;
        if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            t.Public += "USE_WINDOWS_DLL_SEMANTICS"_d;
            t.Public += "WIN32"_d;
        }
        else
        {
            t.ExportAllSymbols = true;
        }

        return t;
    };

    auto &core = add_target("core");
    {
        core -=
            ".*/android/.*"_rr,
            ".*/windows/.*"_rr,
            ".*/linux-shared/.*"_rr,
            "source/net/.*"_rr,
            "source/utils/crypto/.*"_rr
            ;
        core += "source/utils/crypto/.*"_r;
        core += "source/utils/crypto/factory/.*"_rr;

        Definition d;
        d.d = "AWS_SDK_VERSION_MAJOR="s + core.Variables["PACKAGE_VERSION_MAJOR"].toString();
        core += d;
        d.d = "AWS_SDK_VERSION_MINOR="s + core.Variables["PACKAGE_VERSION_MINOR"].toString();
        core += d;
        d.d = "AWS_SDK_VERSION_PATCH="s + core.Variables["PACKAGE_VERSION_PATCH"].toString();
        core += d;
        d.d = "AWS_SDK_VERSION_STRING="s + core.Variables["PACKAGE_VERSION"].toString();
        core += d;

        if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            core += "Version.lib"_slib;
            core += "bcrypt.lib"_slib;
            core += "rpcrt4.lib"_slib;
            core += "userenv.lib"_slib;
            core += "winhttp.lib"_slib;
            core += "wininet.lib"_slib;
        }
        else
        {
            core += "pthread"_slib;
        }

        core += "ENABLE_CURL_CLIENT"_def;
        core += "ENABLE_OPENSSL_ENCRYPTION"_def;

        core += "source/http/curl/.*"_rr;
        core += "source/utils/crypto/openssl/.*"_rr;

        core += "org.sw.demo.badger.curl.libcurl"_dep;
        core += "org.sw.demo.openssl.ssl"_dep;
        core.Public += "org.sw.demo.amazon.awslabs.c_event_stream"_dep;

        if (core.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            core += ".*/windows/.*"_rr;
            core -= "source/http/windows/IXmlHttpRequest2HttpClient.cpp";
        }
        else
            core += ".*/linux-shared/.*"_rr;

        core.configureFile("include/aws/core/SDKConfig.h.in", "aws/core/SDKConfig.h");
    }

    auto add_target_with_core = [&add_target, &core](const String &name) -> decltype(auto)
    {
        return add_target(name).Public += core;
    };

    add_target_with_core("ec2");
    add_target_with_core("s3");

    return; // for now

    std::unordered_set<String> manually_created_targets
    {
        "core",
        "s3",
    };

    for (auto &d : fs::directory_iterator(s.SourceDir))
    {
        if (!fs::is_directory(d))
            continue;
        auto dn = d.path().filename().u8string();
        if (dn.find("aws-cpp-sdk-") != 0)
            continue;
        if (dn.find("-tests") != dn.npos)
            continue;
        auto n = dn.substr(strlen("aws-cpp-sdk-"));
        if (manually_created_targets.find(n) != manually_created_targets.end())
            continue;
        add_target_with_core(n);
    }
}
