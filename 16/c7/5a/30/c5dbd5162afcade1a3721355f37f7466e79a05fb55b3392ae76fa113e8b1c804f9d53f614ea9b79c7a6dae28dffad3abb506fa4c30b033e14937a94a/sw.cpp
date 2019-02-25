void build(Solution &s)
{
    auto &hat_trie = s.addTarget<LibraryTarget>("tessil.robin_map", "0.6.1");
    hat_trie += Git("https://github.com/Tessil/robin-map", "v{v}");
}
