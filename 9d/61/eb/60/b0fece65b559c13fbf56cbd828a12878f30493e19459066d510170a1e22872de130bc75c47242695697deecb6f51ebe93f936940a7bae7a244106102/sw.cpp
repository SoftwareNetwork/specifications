void build(Solution &s)
{
    auto &t = s.addLibrary("dtschump.CImg", "4.0.7");
    t += Git("https://github.com/dtschump/CImg", "v.{v}");
}
