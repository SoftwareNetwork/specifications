void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("cemyuksel.cyCodeBase", "master");
    t += Git("https://github.com/cemyuksel/cyCodeBase");
}
