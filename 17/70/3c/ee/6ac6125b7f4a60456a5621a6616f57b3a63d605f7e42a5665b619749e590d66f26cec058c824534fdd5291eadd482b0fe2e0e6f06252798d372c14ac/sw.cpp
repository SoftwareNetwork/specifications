void build(Solution &s)
{
    auto &p = s.addLibrary("wjakob.filesystem", "master");
    p += Git("https://github.com/wjakob/filesystem", "", "{v}");
    p += "filesystem/.*"_rr;
}
