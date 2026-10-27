void build(Solution &s)
{
    auto &p = s.addLibrary("Neargye.nameof", "0.10.6");
    p += Git("https://github.com/Neargye/nameof", "v{v}");
}
