void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("VHRanger.pcg", "master");
    t += Git("https://github.com/VHRanger/pcg.h");
}
