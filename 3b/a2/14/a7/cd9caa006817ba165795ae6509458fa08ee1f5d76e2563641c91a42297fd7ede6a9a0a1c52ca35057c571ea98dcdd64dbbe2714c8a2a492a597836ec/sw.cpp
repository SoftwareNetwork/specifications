void build(Solution &s)
{
    auto &t = s.addTarget<Library>("phisko.kengine", "4.0.0");
    t += Git("https://github.com/phisko/kengine", "v{M}.{m}");
}
