void build(Solution &s)
{
    auto &array_hash = s.addTarget<LibraryTarget>("tessil.array_hash", "master");
    array_hash += Git("https://github.com/Tessil/array-hash", "", "{v}");
}
