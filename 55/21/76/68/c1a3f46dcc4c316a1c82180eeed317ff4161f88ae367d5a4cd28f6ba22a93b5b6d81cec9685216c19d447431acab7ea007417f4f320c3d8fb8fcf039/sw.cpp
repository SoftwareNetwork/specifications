void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("jarro2783.cxxopts", "3.2.1");
    t += Git("https://github.com/jarro2783/cxxopts", "v{v}");
    t += "include/.*"_rr;
}
