void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("tlk00.BitMagic", "master");
    t += Git("https://github.com/tlk00/BitMagic");
}
