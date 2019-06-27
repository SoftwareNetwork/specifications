void build(Solution &s)
{
    auto &t = s.addLibrary("HandmadeMath.HandmadeMath", "master");
    t += Git("https://github.com/HandmadeMath/Handmade-Math");
}
