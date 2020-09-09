void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ned14.outcome", "master");
    t += Git("https://github.com/ned14/outcome");
    t.Public += "org.sw.demo.ned14.quickcpplib-master"_dep;
}
