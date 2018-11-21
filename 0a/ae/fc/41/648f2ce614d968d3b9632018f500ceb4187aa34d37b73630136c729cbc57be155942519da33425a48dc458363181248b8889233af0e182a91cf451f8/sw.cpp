void build(Solution &s)
{
    auto &range_v3_vs2015 = s.addTarget<LibraryTarget>("microsoft.range_v3_vs2015", "master");
    range_v3_vs2015 += Git("https://github.com/microsoft/Range-V3-VS2015", "", "{v}");
}