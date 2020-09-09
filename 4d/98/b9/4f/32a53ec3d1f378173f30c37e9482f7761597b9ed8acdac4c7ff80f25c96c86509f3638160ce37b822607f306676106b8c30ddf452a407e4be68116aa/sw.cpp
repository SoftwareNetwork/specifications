void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("H_EAL.oqpi", "master");
    t += Git("https://github.com/H-EAL/oqpi");
}
