void build(Solution &s)
{
    auto &yaml_cpp = s.addTarget<LibraryTarget>("jbeder.yaml_cpp", "master");
    yaml_cpp += Git("https://github.com/jbeder/yaml-cpp", "", "master");

    yaml_cpp.CPPVersion = CPPLanguageStandard::CPP11;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        yaml_cpp.Private += sw::Shared, "yaml_cpp_EXPORTS"_d;
        yaml_cpp.Public += sw::Shared, "YAML_CPP_DLL"_d;
    }
    else
        yaml_cpp.replaceInFileOnce("include/yaml-cpp/dll.h",
            "#define YAML_CPP_API",
            "#define  YAML_CPP_API __attribute__((visibility (\"default\")))");

    yaml_cpp.Public += "org.sw.demo.boost.iterator-*"_dep;
    yaml_cpp.Public += "org.sw.demo.boost.smart_ptr-*"_dep;
}
