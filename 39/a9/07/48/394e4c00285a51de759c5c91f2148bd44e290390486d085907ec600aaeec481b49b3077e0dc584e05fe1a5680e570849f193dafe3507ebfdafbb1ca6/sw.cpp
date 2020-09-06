void build(Solution &s)
{
    auto &t = s.addLibrary("dtschump.CImg", "2.9.2");
    t += Git("https://github.com/dtschump/CImg", "v.{v}");
}
