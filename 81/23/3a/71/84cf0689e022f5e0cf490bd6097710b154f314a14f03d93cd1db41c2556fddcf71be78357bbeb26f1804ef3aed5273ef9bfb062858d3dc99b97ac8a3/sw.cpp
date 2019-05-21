void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("microsoft.wil", "master");
    t += Git("https://github.com/microsoft/wil");
}
