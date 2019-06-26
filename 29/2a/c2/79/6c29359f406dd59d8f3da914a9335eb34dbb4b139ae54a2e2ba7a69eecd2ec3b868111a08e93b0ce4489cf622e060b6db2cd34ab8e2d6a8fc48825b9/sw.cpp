void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("pinam45.dynamic_bitset", "master");
    t += Git("https://github.com/pinam45/dynamic_bitset");
}
