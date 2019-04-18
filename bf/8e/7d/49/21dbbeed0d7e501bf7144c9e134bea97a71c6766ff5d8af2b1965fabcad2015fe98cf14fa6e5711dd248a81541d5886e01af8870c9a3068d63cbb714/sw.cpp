void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("felixguendling.cista", "0.4.0");
    t += Git("https://github.com/felixguendling/cista", "v{M}.{m}");
}
