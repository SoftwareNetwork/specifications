void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("hasherezade.libpeconv", "master");
    t += Git("https://github.com/hasherezade/libpeconv");
    t.setRootDirectory("libpeconv");
}
