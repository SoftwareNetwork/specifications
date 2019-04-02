void build(Solution &s)
{
    auto &p = s.addTarget<StaticLibrary>("byronknoll.cmix", "17");
    p += Git("https://github.com/byronknoll/cmix", "v{M}");
}
