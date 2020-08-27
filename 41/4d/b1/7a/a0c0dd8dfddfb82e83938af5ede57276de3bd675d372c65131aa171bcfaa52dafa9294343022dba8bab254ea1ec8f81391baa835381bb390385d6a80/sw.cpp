void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("daniele77.cli", "1.2.1");
    t += Git("https://github.com/daniele77/cli", "v{v}");
}
