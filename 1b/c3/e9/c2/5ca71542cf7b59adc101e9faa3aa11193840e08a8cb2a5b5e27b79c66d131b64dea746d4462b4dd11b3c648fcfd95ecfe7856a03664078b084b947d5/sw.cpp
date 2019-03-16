void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.span_lite", "master");
    t += Git("https://github.com/martinmoene/span-lite");
}
