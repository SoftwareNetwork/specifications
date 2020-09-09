void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("cameron314.concurrentqueue", "1.0.2");
    t += Git("https://github.com/cameron314/concurrentqueue");
}
