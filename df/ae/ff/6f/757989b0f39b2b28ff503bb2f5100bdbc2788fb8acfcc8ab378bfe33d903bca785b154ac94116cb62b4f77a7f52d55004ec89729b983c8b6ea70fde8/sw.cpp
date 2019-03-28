void build(Solution &s)
{
    auto &p = s.addLibrary("ademakov.Oroch", "master");
    p += Git("https://github.com/ademakov/Oroch");
    p += "oroch/.*"_rr;
}
