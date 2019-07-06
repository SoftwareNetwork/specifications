void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("ValeevGroup.tiledarray", "master");
    t += Git("https://github.com/ValeevGroup/tiledarray");
}
