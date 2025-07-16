void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TartanLlama.expected", "1.2.0");
    t += Git("https://github.com/TartanLlama/expected", "v{v}");
}
