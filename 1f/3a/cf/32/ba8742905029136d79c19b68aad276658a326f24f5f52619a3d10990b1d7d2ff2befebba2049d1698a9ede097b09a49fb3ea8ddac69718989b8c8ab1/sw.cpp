void build(Solution &s)
{
    auto &t = s.addStaticLibrary("erincatto.box2d", "2.4.1");
    t += Git("https://github.com/erincatto/box2d", "v{v}");
}
