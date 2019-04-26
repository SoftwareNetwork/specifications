void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("dmtx.libdmtx", "0.7.5");
    t += Git("https://github.com/dmtx/libdmtx", "v{v}");
}
