void build(Solution &s)
{
    auto &t = s.addTarget<Library>("LoopPerfect.neither", "master");
    t += Git("https://github.com/LoopPerfect/neither");
    t += "neither/include/.*"_rr;
    t += "neither/include"_id;
}
