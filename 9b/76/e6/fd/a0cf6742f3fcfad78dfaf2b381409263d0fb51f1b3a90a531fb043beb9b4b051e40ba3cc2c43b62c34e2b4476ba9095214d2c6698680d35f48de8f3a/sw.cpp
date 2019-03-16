void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.lest", "master");
    t += Git("https://github.com/martinmoene/lest");
}
