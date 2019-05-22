void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("Manu343726.ctti", "master");
    t += Git("https://github.com/Manu343726/ctti");
}
