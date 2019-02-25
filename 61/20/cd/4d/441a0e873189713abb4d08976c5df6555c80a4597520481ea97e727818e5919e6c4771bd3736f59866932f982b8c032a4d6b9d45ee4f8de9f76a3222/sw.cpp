void build(Solution &s)
{
    auto &hat_trie = s.addTarget<LibraryTarget>("tessil.hat_trie", "master");
    hat_trie += Git("https://github.com/Tessil/hat-trie", "", "{v}");
    hat_trie += "src/[^/]*\\.h"_rr;
}
