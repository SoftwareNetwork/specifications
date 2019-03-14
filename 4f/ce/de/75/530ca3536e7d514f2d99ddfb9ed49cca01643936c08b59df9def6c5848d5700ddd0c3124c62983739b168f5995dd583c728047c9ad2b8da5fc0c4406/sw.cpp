void build(Solution &s)
{
    auto &p = s.addLibrary("c42f.tinyformat", "master");
    p += Git("https://github.com/c42f/tinyformat", "", "{v}");
}
