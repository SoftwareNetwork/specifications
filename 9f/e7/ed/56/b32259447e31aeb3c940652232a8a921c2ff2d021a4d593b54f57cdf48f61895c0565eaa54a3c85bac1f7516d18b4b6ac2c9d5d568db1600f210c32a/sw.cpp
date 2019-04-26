void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("disheng222.SZ", "2.1.4.2");
    t += Git("https://github.com/disheng222/SZ", "v{v}");
    t.setRootDirectory("sz");
}
