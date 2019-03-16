void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("imneme.pcg_c", "master");
    t += Git("https://github.com/imneme/pcg-c");
}
