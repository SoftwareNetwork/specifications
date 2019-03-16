void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.optional_lite", "3.1.1");
    t += Git("https://github.com/martinmoene/optional-lite", "v{v}");
}
