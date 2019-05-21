void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("LouisCharlesC.safe", "master");
    t += Git("https://github.com/LouisCharlesC/safe");
}
