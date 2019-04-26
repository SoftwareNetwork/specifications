void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("fr00b0.nod", "0.5.0");
    t += Git("https://github.com/fr00b0/nod", "v{v}");
}
