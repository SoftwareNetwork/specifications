void build(Solution &s)
{
    auto &t = s.addStaticLibrary("skywind3000.kcp", "1.7.0");
    t += Git("https://github.com/skywind3000/kcp", "{M}.{m}");
    t += "ikcp.*"_r;
}
