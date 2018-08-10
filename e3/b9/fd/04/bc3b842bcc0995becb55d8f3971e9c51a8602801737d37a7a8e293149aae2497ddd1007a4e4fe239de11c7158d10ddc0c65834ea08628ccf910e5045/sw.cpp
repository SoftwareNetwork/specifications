void build(Solution &s)
{
    auto &enum_flags = s.addTarget<LibraryTarget>("grisumbras.enum_flags", "master");
    enum_flags += Git("https://github.com/grisumbras/enum-flags", "", "{v}");
}
