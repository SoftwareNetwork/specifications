void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("bernedom.SI", "1.2.0");
    t += Git("https://github.com/bernedom/SI");
}
