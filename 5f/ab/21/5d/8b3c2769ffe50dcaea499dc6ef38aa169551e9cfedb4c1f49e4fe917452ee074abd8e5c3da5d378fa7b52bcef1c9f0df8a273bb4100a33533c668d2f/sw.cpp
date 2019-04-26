void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("Microsoft.LightLDA", "master");
    t += Git("https://github.com/Microsoft/LightLDA");
}
