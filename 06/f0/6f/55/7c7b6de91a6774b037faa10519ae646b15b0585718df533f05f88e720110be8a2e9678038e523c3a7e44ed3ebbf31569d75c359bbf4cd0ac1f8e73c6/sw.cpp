void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("Microsoft.Multiverso", "0.2");
    t += Git("https://github.com/Microsoft/Multiverso", "v{M}.{m}");
}
