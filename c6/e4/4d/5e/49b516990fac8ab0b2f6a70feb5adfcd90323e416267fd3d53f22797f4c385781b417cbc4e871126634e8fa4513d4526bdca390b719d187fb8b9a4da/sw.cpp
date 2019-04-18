void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("dougbinks.enkiTS", "master");
    t += Git("https://github.com/dougbinks/enkiTS");
}
