void build(Solution &s)
{
    auto &p = s.addLibrary("pdimov.variant2", "develop");
    p += Git("https://github.com/pdimov/variant2", "", "{v}");
}
