void build(Solution &s)
{
    auto &t = s.addStaticLibrary("erincatto.box2d", "master");
    t += Git("https://github.com/erincatto/box2d");
}
