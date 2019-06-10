void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("cameron314.readerwriterqueue", "master");
    t += Git("https://github.com/cameron314/readerwriterqueue");
}
