void build(Solution &s)
{
    auto &t = s.addLibrary("mity.md4c", "0.4.2");
    t += Git("https://github.com/mity/md4c", "release-{v}");
    t.ExportAllSymbols = true;
    t += "md4c/.*\\.[hc]"_rr;
}
