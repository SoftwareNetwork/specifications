void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("tessil.hopscotch_map", "2.2.1");
    t += Git("https://github.com/Tessil/hopscotch-map", "v{v}");
}
