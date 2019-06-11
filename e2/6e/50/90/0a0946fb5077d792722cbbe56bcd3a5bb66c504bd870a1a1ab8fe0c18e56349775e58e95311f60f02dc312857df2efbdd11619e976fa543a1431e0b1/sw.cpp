void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("fmela.libdict", "master");
    t += Git("https://github.com/fmela/libdict");
}
