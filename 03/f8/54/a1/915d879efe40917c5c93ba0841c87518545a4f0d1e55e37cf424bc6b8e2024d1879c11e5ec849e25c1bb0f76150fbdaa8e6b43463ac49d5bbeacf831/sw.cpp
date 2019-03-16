void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.any_lite", "0.1.0");
    t += Git("https://github.com/martinmoene/any-lite", "v{v}");
}
