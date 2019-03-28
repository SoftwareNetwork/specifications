void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ned14.outcome", "master");
    t += Git("https://github.com/ned14/outcome");
}
