void build(Solution &s)
{
    auto &p = s.addLibrary("woboq.verdigris", "1.1");
    p += Git("https://github.com/woboq/verdigris", "v{M}.{m}");
}
