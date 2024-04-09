void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("wtl", "10.0.10320");
    t += Git("https://git.code.sf.net/p/wtl/git", "Release_{M}.{m}");

    t += "Include/.*"_rr;
    t.Public += "Include"_idir;
}
