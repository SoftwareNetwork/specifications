void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("demo.zlib");
    t.Version = "1.2.11";
    t.Source = Git("https://github.com/madler/zlib", "v1.2.11");
    t += "[^/]*\\.[hc]"_rr;
    t += sw::Shared, "ZLIB_DLL"_d;
}
