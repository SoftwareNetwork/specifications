void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("Terraspace.UASM", "master");
    t += Git("https://github.com/Terraspace/UASM");
}
