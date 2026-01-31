void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("microsoft.wil", "1.0.260126.7");
    t += Git("https://github.com/microsoft/wil");
}
