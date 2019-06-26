void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("jlctools.evo", "master");
    t += Git("https://github.com/jlctools/evo");
}
