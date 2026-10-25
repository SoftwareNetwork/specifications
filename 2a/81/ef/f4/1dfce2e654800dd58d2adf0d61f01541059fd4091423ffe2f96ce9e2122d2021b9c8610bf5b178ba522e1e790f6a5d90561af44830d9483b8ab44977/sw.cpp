void build(Solution &s)
{
    auto &t = s.add<StaticLibrary>("ArtifexSoftware.mujs", "1.3.11");
    t += Git("https://github.com/ArtifexSoftware/mujs");
    t -= ".*\\.[hc]"_r;
    t += "one.c";
}
