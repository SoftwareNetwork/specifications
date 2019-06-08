void build(Solution &s)
{
    auto &p = s.addLibrary("ddemidov.amgcl", "master");
    p += Git("https://github.com/ddemidov/amgcl");

    p += "amgcl/.*"_rr;
    p += "lib/amgcl\\..*"_rr;
    p += "lib/dll.def";

    p += "NOMINMAX"_def;
    p.Public += "."_id;

    p += "org.sw.demo.boost.iterator"_dep;
    p += "org.sw.demo.boost.property_tree"_dep;
    p += "org.sw.demo.boost.range"_dep;

    p.patch("lib/dll.def", "LIBRARY libamgcl", "");
}
