void build(Solution &s)
{
    auto &range_v3 = s.addTarget<LibraryTarget>("ericniebler.range_v3", "0.10.0");
    range_v3 += Git("https://github.com/ericniebler/range-v3");
}
