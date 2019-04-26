void build(Solution &s)
{
    auto &t = s.addTarget<Library>("niXman.yas", "7.0.2");
    t += Git("https://github.com/niXman/yas");
}
