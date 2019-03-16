void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.variant_lite", "master");
    t += Git("https://github.com/martinmoene/variant-lite");
}
