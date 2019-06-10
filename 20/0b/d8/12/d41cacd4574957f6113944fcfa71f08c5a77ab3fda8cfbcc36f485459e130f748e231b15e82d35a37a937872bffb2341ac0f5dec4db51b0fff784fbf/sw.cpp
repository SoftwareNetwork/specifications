void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("cameron314.concurrentqueue", "master");
    t += Git("https://github.com/cameron314/concurrentqueue");
}
