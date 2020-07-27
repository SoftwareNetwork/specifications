void build(Solution &s)
{
    auto &t = s.addStaticLibrary("erincatto.box2d", "2.4.0");
    t += Git("https://github.com/erincatto/box2d", "v{v}");
}
