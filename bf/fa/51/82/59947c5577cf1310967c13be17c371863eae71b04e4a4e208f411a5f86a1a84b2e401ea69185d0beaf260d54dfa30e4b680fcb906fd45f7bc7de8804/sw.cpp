void build(Solution &s)
{
    auto &t = s.addLibrary("mity.md4c", "0.5.2");
    t += Git("https://github.com/mity/md4c", "release-{v}");
    t.ExportAllSymbols = true;
    t += "src/.*\\.[hc]"_rr;
    t.Public += "src"_id;
}
