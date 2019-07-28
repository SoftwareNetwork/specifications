void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("eliasdaler.MetaStuff", "master");
    t += Git("https://github.com/eliasdaler/MetaStuff");
}
