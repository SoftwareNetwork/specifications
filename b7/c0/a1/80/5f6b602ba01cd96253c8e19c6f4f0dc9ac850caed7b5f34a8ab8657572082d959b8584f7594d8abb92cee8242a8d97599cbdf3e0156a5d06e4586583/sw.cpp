void build(Solution &s)
{
    auto &t = s.addLibrary("dtschump.CImg", "3.2.0");
    t += Git("https://github.com/dtschump/CImg", "v.{v}");
}
