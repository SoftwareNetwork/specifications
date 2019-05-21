void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("Naios.continuable", "3.0.0");
    t += Git("https://github.com/Naios/continuable");
}
