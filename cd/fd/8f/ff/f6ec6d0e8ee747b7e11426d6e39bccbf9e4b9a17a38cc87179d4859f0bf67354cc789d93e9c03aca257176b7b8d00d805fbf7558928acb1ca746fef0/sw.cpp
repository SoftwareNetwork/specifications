void build(Solution &s)
{
    auto &t = s.addLibrary("mayah.tinytoml", "0.4.0");
    t += Git("https://github.com/mayah/tinytoml", "v{M}.{m}");
    t += "include/.*"_rr;
}
