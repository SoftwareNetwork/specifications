void build(Solution &s)
{
    auto &p = s.addLibrary("TartanLlama.function_ref", "master");
    p += Git("https://github.com/TartanLlama/function_ref");
}
