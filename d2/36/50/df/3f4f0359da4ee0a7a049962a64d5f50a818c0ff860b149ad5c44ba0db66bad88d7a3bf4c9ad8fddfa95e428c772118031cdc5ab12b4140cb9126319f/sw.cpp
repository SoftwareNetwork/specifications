void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("imneme.pcg_cpp", "master");
    t += Git("https://github.com/imneme/pcg-cpp");
}
