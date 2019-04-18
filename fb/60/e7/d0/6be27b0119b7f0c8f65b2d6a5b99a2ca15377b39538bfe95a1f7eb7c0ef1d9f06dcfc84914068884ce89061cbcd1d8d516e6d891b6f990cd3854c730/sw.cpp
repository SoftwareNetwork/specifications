void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("TheLartians.Parser", "1.8.0");
    t += Git("https://github.com/TheLartians/Parser", "v{M}.{m}");
    t += cpp17;
}
