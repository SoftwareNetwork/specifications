void build(Solution &s)
{
    auto &t = s.addStaticLibrary("jll63.yomm2", "master");
    t += Git("https://github.com/jll63/yomm2");

    t += cpp17;
    t += "org.sw.demo.boost.dynamic_bitset"_dep;
    t += "org.sw.demo.boost.preprocessor"_dep;
    t += "org.sw.demo.boost.type_traits"_dep;
}
