void build(Solution &s)
{
    auto &t = s.addLibrary("dtschump.CImg", "3.5.5");
    t += Git("https://github.com/dtschump/CImg", "v.{v}");
}
