void build(Solution &s)
{
    auto &p = s.addStaticLibrary("ocornut.str", "master");
    p += Git("https://github.com/ocornut/str");
}
