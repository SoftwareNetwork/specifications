void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("crosire.blink", "master");
    t += Git("https://github.com/crosire/blink");
}
