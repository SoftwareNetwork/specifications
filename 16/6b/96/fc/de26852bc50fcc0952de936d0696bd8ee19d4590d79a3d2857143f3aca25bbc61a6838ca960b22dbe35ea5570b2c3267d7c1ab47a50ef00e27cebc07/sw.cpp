void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("martinus.robin_hood_hashing", "3.10.0");
    t += Git("https://github.com/martinus/robin-hood-hashing");
    t += "src/include/.*\\.h"_rr;
    t += "src/include"_idir;
}
