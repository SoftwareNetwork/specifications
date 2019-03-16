void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.ring_span_lite", "0.2.0");
    t += Git("https://github.com/martinmoene/ring-span-lite", "v{v}");
}
