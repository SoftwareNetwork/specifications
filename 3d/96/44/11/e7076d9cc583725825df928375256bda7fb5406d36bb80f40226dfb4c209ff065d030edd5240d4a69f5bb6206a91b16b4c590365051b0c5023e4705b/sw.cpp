void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("TartanLlama.expected", "1.3.1");
    t += Git("https://github.com/TartanLlama/expected", "v{v}");
}
