void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("bstamour.units", "master");
    t += Git("https://github.com/bstamour/units");
}
