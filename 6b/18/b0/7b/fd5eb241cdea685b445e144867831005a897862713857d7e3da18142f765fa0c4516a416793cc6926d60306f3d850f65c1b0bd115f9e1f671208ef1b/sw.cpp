void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("jlctools.evo", "master");
    t += Git("https://github.com/jlctools/evo");
}
