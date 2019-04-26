void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("blitzpp.blitz", "master");
    t += Git("https://github.com/blitzpp/blitz");
    t += "blitz/.*"_rr;
}
