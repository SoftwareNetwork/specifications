void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("mpusz.units", "master");
    t += Git("https://github.com/mpusz/units");
}
