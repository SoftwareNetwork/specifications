void build(Solution &s)
{
    auto &t = s.addLibrary("groundswellaudio.swl_variant", "main");
    t += Git("https://github.com/groundswellaudio/swl-variant");
}
