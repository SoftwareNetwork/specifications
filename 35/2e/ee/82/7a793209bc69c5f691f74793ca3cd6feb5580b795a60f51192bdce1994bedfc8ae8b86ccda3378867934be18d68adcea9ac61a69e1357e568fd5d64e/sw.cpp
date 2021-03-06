void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &t = s.addTarget<LibraryTarget>("madler.zlib", "1.2.11");
    t.Source = Git("https://github.com/madler/zlib", "v{v}");
    t += "[^/]*\\.[hc]"_rr;
    t += sw::Shared, "ZLIB_DLL"_d;
}