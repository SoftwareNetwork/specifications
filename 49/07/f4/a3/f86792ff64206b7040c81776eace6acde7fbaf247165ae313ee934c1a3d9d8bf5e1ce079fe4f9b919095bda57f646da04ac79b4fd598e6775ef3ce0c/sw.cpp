void build(Solution &s)
{
    auto &p = s.addLibrary("Neargye.nameof", "master");
    p += Git("https://github.com/Neargye/nameof");
}
