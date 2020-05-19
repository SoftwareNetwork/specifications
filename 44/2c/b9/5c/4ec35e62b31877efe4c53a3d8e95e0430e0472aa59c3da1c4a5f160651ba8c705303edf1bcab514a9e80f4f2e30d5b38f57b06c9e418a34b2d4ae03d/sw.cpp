void build(Build &s)
{
    auto &t = s.addStaticLibrary("MemoryModule.MemoryModule", "master");
    t += Git("https://github.com/fancycode/MemoryModule");
    t += "MemoryModule\\.[hc]"_r;
}
