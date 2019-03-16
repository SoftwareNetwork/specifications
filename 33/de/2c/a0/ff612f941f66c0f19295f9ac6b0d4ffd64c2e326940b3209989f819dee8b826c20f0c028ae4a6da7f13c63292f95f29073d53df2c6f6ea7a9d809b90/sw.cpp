void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.optional_bare", "1.0.0");
    t += Git("https://github.com/martinmoene/optional-bare", "v{v}");
}
