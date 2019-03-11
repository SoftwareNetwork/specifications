void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("taocpp.operators", "master");
    t += Git("https://github.com/taocpp/operators", "", "{v}");
    t += "include/.*"_rr;
}
