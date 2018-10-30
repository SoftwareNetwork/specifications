void build(Solution &s)
{
    auto &variant = s.addTarget<LibraryTarget>("mapbox.variant", "1.1.5");
    variant += Git("https://github.com/mapbox/variant", "v{v}");
    variant += "include/.*"_rr;
    variant.Public += "include"_id;
}
