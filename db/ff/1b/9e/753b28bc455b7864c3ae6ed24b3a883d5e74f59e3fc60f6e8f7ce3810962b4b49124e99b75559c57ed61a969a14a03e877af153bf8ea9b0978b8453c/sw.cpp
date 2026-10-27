void build(Solution &s)
{
    auto &t = s.addLibrary("dtschump.CImg", "4.0.6");
    t += Git("https://github.com/dtschump/CImg", "v.{v}");
}
