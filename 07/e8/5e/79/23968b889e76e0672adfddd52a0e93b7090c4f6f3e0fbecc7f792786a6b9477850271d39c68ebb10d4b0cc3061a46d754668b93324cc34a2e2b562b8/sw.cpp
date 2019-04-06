void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("KonanM.static_enum", "master");
    t += Git("https://github.com/KonanM/static_enum");
}
