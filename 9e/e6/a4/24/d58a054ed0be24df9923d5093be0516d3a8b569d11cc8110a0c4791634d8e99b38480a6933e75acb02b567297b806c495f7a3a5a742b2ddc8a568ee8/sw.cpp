void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("rodrigocfd.winlamb", "master");
    t += Git("https://github.com/rodrigocfd/winlamb");
    t += ".*\\.h"_rr;
}
