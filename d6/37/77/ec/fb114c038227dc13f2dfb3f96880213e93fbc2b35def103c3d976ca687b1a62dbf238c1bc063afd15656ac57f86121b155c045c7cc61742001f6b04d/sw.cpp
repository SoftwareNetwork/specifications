void build(Solution &s)
{
    auto &p = s.addLibrary("Naios.function2", "master");
    p += Git("https://github.com/Naios/function2", "", "{v}");
}
