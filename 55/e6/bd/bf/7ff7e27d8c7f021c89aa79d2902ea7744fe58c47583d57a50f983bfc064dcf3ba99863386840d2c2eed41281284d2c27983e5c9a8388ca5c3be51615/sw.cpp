void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("rnburn.stackext", "master");
    t += Git("https://github.com/rnburn/stackext");
}
