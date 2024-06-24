void build(Solution &s)
{
    auto &t = s.addStaticLibrary("jinja2cpp", "1.3.2");
    t += Git("https://github.com/jinja2cpp/Jinja2Cpp");

    t += cpp17;

    t += "org.sw.demo.fmt"_dep;
    t += "org.sw.demo.boost.algorithm"_dep;
    t += "org.sw.demo.boost.filesystem"_dep;
    t += "org.sw.demo.boost.iterator"_dep;
    t += "org.sw.demo.boost.unordered"_dep;
    t += "org.sw.demo.boost.variant"_dep;
    t += "org.sw.demo.martinmoene.expected_lite"_dep;
    t += "org.sw.demo.martinmoene.optional_lite"_dep;
    t += "org.sw.demo.martinmoene.string_view_lite"_dep;
    t += "org.sw.demo.martinmoene.variant_lite"_dep;
}
