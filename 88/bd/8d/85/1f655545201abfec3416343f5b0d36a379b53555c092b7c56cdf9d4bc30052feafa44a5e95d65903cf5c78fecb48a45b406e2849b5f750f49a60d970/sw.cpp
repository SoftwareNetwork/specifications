void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ned14.quickcpplib", "master");
    t += Git("https://github.com/ned14/quickcpplib");
}
