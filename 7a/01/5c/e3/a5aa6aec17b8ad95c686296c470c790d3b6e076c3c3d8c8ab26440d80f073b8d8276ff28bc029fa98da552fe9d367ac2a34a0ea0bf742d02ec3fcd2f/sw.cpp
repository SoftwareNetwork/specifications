void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TartanLlama.expected", "0.3");
    t += Git("https://github.com/TartanLlama/expected", "v{M}.{m}");
    t += "tl/.*"_rr;
}
