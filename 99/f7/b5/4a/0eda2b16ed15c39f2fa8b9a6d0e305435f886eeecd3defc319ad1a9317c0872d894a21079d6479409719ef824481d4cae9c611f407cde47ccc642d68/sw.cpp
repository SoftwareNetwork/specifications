void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("RedSpah.xxhash_cpp", "master");
    t += Git("https://github.com/RedSpah/xxhash_cpp");
    t += "xxhash/xxhash\\.[hc]p?p?"_rr;
}
