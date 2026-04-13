void build(Solution &s)
{
    auto &pystring = s.addTarget<StaticLibraryTarget>("imageworks.pystring", "1.1.5");
    pystring += Git("https://github.com/imageworks/pystring", "v{v}");
    pystring += "pystring.*"_rr;
}
