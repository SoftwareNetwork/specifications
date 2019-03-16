void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.clue", "master");
    t += Git("https://github.com/martinmoene/clue");
}
