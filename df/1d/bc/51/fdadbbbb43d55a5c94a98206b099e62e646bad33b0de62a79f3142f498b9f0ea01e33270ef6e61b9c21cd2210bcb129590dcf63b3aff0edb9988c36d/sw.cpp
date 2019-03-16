void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.any_lite", "master");
    t += Git("https://github.com/martinmoene/any-lite");
}
