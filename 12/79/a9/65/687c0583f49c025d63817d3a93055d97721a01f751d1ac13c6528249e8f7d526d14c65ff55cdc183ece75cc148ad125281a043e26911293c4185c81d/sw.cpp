void build(Solution &s)
{
    auto &t = s.addStaticLibrary("USCiLab.cereal", "1.2.2");
    t += Git("https://github.com/USCiLab/cereal", "v{v}");
}
