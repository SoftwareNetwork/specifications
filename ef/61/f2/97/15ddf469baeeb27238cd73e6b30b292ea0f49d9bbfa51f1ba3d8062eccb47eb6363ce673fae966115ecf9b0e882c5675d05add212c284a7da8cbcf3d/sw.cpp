void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("taocpp.config", "master");
    t += Git("https://github.com/taocpp/config", "", "{v}");
    t += "include/.*"_rr;
}
