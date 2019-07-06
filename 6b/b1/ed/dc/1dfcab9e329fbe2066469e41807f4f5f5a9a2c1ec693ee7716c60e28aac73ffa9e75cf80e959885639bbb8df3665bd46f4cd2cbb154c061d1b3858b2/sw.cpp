void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("progschj.ThreadPool", "master");
    t += Git("https://github.com/progschj/ThreadPool");
    t += ".*\\.h"_r;
}
