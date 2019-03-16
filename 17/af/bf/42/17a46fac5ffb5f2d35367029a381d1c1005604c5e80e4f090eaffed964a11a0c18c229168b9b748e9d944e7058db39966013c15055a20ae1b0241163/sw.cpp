void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TartanLlama.optional", "master");
    t += Git("https://github.com/TartanLlama/optional");
    t += "tl/.*"_rr;
}
