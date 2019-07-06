void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("graeme_hill.crossguid", "master");
    t += Git("https://github.com/graeme-hill/crossguid");
}
