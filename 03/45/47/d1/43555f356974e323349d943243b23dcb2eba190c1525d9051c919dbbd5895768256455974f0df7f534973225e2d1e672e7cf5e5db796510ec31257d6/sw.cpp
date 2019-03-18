void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("beltoforion.muparserx", "4.0.8");
    t += Git("https://github.com/beltoforion/muparserx", "v{v}");

    t += "parser/.*"_rr;
}
