void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.variant_lite", "1.2.0");
    t += Git("https://github.com/martinmoene/variant-lite", "v{v}");
}
