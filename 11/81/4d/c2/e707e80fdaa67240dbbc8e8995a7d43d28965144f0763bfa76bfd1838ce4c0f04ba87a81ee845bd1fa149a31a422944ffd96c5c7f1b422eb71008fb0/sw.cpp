void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("taocpp.sequences", "master");
    t += Git("https://github.com/taocpp/sequences", "", "{v}");
    t += "include/.*"_rr;
}
