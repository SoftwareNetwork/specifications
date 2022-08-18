void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("daniele77.cli", "2.0.2");
    t += Git("https://github.com/daniele77/cli", "v{v}");
}
