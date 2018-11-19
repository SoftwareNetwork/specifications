void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("madler.zlib", "1.2.8");
    t += Git("https://github.com/madler/zlib", "v{v}");
    t += "[^/]*\\.[hc]"_rr;
    t += sw::Shared, "ZLIB_DLL"_d;
}

