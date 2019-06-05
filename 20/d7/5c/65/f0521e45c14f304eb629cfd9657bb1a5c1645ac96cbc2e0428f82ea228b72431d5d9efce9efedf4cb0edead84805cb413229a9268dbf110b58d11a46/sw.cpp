void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("martinus.robin_hood_hashing", "3.2.3");
    t += Git("https://github.com/martinus/robin-hood-hashing", "", "v{v}");
    t += "src/include/.*"_rr;
    t += "src/include/.*"_idir;
}
