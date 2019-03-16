void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.optional_bare", "master");
    t += Git("https://github.com/martinmoene/optional-bare");
}
