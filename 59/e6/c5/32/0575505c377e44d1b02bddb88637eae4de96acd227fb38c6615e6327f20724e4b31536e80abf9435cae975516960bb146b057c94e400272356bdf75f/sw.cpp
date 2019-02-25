void build(Solution &s)
{
    auto &array_hash = s.addTarget<LibraryTarget>("tessil.ordered_map", "master");
    array_hash += Git("https://github.com/Tessil/ordered-map", "", "{v}");
}
