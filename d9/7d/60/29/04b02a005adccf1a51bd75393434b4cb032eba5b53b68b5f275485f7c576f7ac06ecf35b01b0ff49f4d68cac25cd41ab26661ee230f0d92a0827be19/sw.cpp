void build(Solution &s)
{
    auto &p = s.addStaticLibrary("chriskohlhoff.urdl", "master");
    p += Git("https://github.com/chriskohlhoff/urdl", "", "{v}");
}
