void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("memononen.NanoVG", "master");
    t += Git("https://github.com/memononen/NanoVG", "", "{v}");
}
