void build(Solution &s)
{
    auto &yaml_cpp = s.addTarget<LibraryTarget>("jbeder.yaml_cpp", "master");
    yaml_cpp += Git("https://github.com/jbeder/yaml-cpp", "", "{v}");

    yaml_cpp.ApiName = "YAML_CPP_API";
    yaml_cpp.CPPVersion = CPPLanguageStandard::CPP11;
    yaml_cpp.writeFileOnce("include/yaml-cpp/dll.h");

    yaml_cpp.Public += "org.sw.demo.boost.iterator-*"_dep;
    yaml_cpp.Public += "org.sw.demo.boost.smart_ptr-*"_dep;
}
