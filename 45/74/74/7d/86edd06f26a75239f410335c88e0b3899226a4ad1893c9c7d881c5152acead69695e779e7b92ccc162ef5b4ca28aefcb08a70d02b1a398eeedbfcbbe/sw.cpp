void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("pnnl.SHAD", "1.1");
    t += Git("https://github.com/pnnl/SHAD", "{M}.{m}");
    t += "include/.*"_rr;
}
