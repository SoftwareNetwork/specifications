void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &yaml_cpp = s.addTarget<StaticLibraryTarget>("jbeder.yaml_cpp", "master");
    yaml_cpp.Source = Git("https://github.com/jbeder/yaml-cpp", "", "master");

    yaml_cpp.Private += sw::Shared, "yaml_cpp_EXPORTS"_d;
    yaml_cpp.Public += sw::Shared, "YAML_CPP_DLL"_d;

    yaml_cpp.Public += "pub.cppan2.demo.boost.iterator-*"_dep;
    yaml_cpp.Public += "pub.cppan2.demo.boost.smart_ptr-*"_dep;
}
