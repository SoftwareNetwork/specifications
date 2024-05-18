void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("mpusz.units", "2.1.1");
    t += Git("https://github.com/mpusz/units");
}
