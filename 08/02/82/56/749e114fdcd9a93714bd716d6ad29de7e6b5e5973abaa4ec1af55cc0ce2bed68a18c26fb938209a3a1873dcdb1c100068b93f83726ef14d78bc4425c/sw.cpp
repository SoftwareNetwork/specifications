void build(Solution &s)
{
    auto &t = s.addLibrary("BlueBrain.HighFive", "master");
    t += Git("https://github.com/BlueBrain/HighFive");
    t += "include/.*"_rr;
}
