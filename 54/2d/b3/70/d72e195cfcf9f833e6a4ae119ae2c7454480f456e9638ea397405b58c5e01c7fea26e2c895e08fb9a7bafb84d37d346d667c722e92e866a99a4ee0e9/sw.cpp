void build(Solution &s)
{
    auto &t = s.addTarget<Library>("effolkronium.random", "1.3.0");
    t += Git("https://github.com/effolkronium/random", "v{v}");
}
