void build(Solution &s)
{
    auto &yaml_cpp = s.addTarget<LibraryTarget>("jbeder.yaml_cpp", "0.7.0");
    yaml_cpp += Git("https://github.com/jbeder/yaml-cpp", "yaml-cpp-{v}");

    yaml_cpp += "include/.*"_rr;
    yaml_cpp += "src/.*"_rr;

    yaml_cpp += "YAML_CPP_API"_api;
    yaml_cpp += cpp11;
    yaml_cpp.writeFileOnce("include/yaml-cpp/dll.h");

    yaml_cpp.Public += "org.sw.demo.boost.iterator"_dep;
    yaml_cpp.Public += "org.sw.demo.boost.smart_ptr"_dep;

    yaml_cpp.writeFileOnce("include/yaml-cpp/dll.h", "");
    yaml_cpp.patch("include/yaml-cpp/node/detail/node.h", "static std::atomic<size_t> m_amount;", "static YAML_CPP_API std::atomic<size_t> m_amount;");
    yaml_cpp.patch("src/node_data.cpp", "std::atomic<size_t> node::m_amount{0};", "YAML_CPP_API std::atomic<size_t>  node::m_amount{0};");
}
