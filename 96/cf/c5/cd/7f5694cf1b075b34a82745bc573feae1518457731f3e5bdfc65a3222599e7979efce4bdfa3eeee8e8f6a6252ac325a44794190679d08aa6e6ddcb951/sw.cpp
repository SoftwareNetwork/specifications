void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("Naios.continuable", "master");
    t += Git("https://github.com/Naios/continuable");
}
