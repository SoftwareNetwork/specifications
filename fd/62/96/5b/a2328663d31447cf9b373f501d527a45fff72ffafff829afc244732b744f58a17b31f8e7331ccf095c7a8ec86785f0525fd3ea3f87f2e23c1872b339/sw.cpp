void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("nemequ.hedley", "master");
    t += Git("https://github.com/nemequ/hedley");
}
