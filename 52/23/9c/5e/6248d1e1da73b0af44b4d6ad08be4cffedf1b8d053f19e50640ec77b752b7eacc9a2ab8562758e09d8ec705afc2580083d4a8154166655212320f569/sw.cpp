void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("nholthaus.units", "2.3.1");
    t += Git("https://github.com/nholthaus/units", "v{v}");
}
