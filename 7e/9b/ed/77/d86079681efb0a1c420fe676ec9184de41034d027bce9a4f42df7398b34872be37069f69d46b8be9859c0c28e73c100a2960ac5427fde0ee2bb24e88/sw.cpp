void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("TheLartians.Event", "2.0.3");
    t += Git("https://github.com/TheLartians/Event", "v{v}");
}
