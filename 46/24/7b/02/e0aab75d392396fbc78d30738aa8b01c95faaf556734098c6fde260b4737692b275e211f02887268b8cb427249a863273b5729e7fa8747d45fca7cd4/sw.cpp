void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("sprinfall.webcc", "master");
    t += Git("https://github.com/sprinfall/webcc");
    t += "webcc/.*"_rr;
}
