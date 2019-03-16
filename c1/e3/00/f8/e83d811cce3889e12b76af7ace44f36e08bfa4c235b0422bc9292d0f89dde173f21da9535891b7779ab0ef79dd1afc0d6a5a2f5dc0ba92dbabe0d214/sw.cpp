void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.optional_lite", "master");
    t += Git("https://github.com/martinmoene/optional-lite");
}
