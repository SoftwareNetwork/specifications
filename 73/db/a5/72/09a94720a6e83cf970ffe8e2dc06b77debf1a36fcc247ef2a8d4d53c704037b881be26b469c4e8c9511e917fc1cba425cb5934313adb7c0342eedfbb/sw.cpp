void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("microsoft.wil", "1.0.250325.1");
    t += Git("https://github.com/microsoft/wil");
}
