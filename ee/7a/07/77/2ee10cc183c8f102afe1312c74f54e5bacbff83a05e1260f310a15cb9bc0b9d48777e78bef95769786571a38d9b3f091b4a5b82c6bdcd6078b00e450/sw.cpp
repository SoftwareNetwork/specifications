void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("SRombauts.SQLiteCpp", "master");
    t += Git("https://github.com/SRombauts/SQLiteCpp");
}
