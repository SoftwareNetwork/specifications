void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("taocpp.tuple", "master");
    t += Git("https://github.com/taocpp/tuple", "", "{v}");
    t += "include/.*"_rr;
}
