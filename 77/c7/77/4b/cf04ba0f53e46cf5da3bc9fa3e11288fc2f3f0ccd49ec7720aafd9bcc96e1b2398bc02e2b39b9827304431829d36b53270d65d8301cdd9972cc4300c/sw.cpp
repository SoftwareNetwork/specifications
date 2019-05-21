void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("odinthenerd.tmp", "master");
    t += Git("https://github.com/odinthenerd/tmp");
}
