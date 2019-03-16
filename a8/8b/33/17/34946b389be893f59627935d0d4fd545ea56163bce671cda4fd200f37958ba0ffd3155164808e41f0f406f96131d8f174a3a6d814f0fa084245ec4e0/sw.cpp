void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TartanLlama.optional", "0.5");
    t += Git("https://github.com/TartanLlama/optional", "v{M}.{m}");
    t += "tl/.*"_rr;
}
