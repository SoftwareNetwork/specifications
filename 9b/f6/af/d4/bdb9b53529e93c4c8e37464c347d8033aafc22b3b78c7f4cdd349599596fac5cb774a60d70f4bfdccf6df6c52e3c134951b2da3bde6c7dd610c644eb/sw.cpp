void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.byte_lite", "0.1.0");
    t += Git("https://github.com/martinmoene/byte-lite", "v{v}");
}
