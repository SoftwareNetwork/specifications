void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("madler.zliby", "1.2.11");
    t += Git("https://github.com/madler/zlib", "v{v}");
    t += "[^/]*\\.[hc]"_rr;
    t += sw::Shared, "ZLIB_DLL"_d;
}
