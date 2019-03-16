void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TartanLlama.tl", "master");
    t += Git("https://github.com/TartanLlama/tl");
}
