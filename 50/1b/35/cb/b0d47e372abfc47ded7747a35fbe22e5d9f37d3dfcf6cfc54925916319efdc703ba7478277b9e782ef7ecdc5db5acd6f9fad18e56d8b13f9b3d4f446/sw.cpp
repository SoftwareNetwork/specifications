void build(Solution &s)
{
    auto &t = s.addStaticLibrary("erincatto.box2d", "3.0.0");
    t += Git("https://github.com/erincatto/box2d", "v{v}");
}
