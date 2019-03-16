void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.lest", "1.35.1");
    t += Git("https://github.com/martinmoene/lest", "v{v}");
}
