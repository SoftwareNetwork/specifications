void build(Solution &s)
{
    auto &t = s.addStaticLibrary("USCiLab.cereal", "master");
    t += Git("https://github.com/USCiLab/cereal");
}
