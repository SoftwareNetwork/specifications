void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("ned14.llfio", "master");
    t += Git("https://github.com/ned14/llfio");
    t += "include/.*"_rr;
}
