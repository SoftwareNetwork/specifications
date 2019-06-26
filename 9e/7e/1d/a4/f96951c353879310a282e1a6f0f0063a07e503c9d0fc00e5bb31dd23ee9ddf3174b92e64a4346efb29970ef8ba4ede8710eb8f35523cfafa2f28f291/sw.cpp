void build(Solution &s)
{
    auto &t = s.addLibrary("LukasBanana.LLGL", "master");
    t += Git("https://github.com/LukasBanana/LLGL");
}
