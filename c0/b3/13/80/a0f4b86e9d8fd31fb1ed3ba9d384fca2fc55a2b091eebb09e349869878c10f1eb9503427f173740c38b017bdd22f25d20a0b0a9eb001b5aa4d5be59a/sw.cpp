void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("njoy.ENDFtk", "master");
    t += Git("https://github.com/njoy/ENDFtk");
}
