void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("Forceflow.libmorton", "0.2.0");
    t += Git("https://github.com/Forceflow/libmorton", "v{M}.{m}");
    t += "libmorton/.*"_rr;
}
