void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("martinmoene.byte_lite", "master");
    t += Git("https://github.com/martinmoene/byte-lite");
}
