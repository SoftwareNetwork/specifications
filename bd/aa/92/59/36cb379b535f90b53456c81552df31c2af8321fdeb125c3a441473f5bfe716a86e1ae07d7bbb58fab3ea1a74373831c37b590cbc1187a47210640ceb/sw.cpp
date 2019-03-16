void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.type_lite", "master");
    t += Git("https://github.com/martinmoene/type-lite");
}
