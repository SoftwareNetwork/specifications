void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TartanLlama.expected", "master");
    t += Git("https://github.com/TartanLlama/expected");
    t += "tl/.*"_rr;
}
