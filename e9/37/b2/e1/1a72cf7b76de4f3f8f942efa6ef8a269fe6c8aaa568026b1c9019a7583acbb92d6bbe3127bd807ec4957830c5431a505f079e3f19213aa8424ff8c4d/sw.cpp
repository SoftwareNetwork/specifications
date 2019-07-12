void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("septag.sx", "master");
    t += Git("https://github.com/septag/sx");
}
