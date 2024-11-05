void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("mpusz.units", "2.4.0");
    t += Git("https://github.com/mpusz/units");
}
