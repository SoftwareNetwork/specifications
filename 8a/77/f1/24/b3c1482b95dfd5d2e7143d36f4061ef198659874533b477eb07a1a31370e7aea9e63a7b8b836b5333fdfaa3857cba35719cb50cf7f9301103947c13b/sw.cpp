void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("google.libnop", "master");
    t += Git("https://github.com/google/libnop");
}
