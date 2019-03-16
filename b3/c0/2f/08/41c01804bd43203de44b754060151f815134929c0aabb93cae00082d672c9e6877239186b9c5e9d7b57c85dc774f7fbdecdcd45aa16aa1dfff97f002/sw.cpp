void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.ring_span_lite", "master");
    t += Git("https://github.com/martinmoene/ring-span-lite");
}
