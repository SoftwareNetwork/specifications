void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("taocpp.json", "master");
    t += Git("https://github.com/taocpp/json", "", "{v}");
    t += "include/.*"_rr;
}
