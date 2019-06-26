void build(Solution &s)
{
    auto &t = s.addLibrary("microsoft.snmalloc", "0.2.0");
    t += Git("https://github.com/microsoft/snmalloc", "{M}.{m}");
}
