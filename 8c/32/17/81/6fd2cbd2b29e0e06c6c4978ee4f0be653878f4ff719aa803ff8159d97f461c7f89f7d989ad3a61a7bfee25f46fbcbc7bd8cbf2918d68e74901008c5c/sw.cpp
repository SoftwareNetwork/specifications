void build(Solution &s)
{
    auto &p = s.addLibrary("arcosuc3m.grppi", "0.4.0");
    p += Git("https://github.com/arcosuc3m/grppi", "v{v}");
}
