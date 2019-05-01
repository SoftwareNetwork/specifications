void build(Solution &s)
{
    auto &t = s.addTarget<Library>("cginternals.glbinding", "3.1.0");
    t += Git("https://github.com/cginternals/glbinding", "v{v}");
    t.setRootDirectory("source/glbinding");
}
