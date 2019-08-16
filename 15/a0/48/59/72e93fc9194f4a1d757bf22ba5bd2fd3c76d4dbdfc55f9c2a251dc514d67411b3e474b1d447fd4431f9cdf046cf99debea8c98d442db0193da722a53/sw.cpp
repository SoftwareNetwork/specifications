void build(Solution &s)
{
    auto &nanopb = s.addTarget<StaticLibraryTarget>("nanopb", "0.3.9.1");
    nanopb += Git("https://github.com/nanopb/nanopb", "{v}");
    nanopb += "[^/]*\\.[hc]"_rr;
    nanopb.Public += "PB_FIELD_32BIT"_d;
}
