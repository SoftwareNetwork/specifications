void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("fraillt.bitsery", "5.0.0");
    t += Git("https://github.com/fraillt/bitsery", "v{v}");
}
