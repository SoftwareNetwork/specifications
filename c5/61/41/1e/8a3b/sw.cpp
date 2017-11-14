void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &t = s.addTarget<LibraryTarget>("zlib", "1.2.11");
    t.Source = Git("https://github.com/madler/zlib", "v{M}.{m}.{p}");
    t += "[^/]*\\.[hc]"_rr;
    t += sw::Shared, "ZLIB_DLL"_d;
}