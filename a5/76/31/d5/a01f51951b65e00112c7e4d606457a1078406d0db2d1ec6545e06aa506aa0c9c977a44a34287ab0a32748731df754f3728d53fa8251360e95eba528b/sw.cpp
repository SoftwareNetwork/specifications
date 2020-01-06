void build(Solution &s)
{
    auto &yaml_cpp = s.addTarget<LibraryTarget>("jbeder.yaml_cpp", "0.6.3");
    yaml_cpp += RemoteFile("https://github.com/jbeder/yaml-cpp/archive/yaml-cpp-{v}.zip");

    yaml_cpp.ApiName = "YAML_CPP_API";
    yaml_cpp.CPPVersion = CPPLanguageStandard::CPP11;
    yaml_cpp.writeFileOnce("include/yaml-cpp/dll.h");
}
