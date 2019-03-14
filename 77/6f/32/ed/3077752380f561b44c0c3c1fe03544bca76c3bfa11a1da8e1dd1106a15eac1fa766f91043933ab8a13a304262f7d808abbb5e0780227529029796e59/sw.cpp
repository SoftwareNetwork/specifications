void build(Solution &s)
{
    auto &p = s.addStaticLibrary("bluescarni.mppp", "0.13.0");
    p += Git("https://github.com/bluescarni/mppp", "v{M}.{m}");
}
