void build(Solution &s)
{
    auto &pegtl = s.addTarget<LibraryTarget>("taocpp.config", "master");
    pegtl += Git("https://github.com/taocpp/config", "", "{v}");
    pegtl += "include/.*"_rr;
}
