void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.span_lite", "0.11.0");
    t += Git("https://github.com/martinmoene/span-lite", "v{v}");
}
