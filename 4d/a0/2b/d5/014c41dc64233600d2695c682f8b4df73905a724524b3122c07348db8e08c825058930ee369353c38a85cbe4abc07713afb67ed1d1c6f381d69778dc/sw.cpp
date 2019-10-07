void build(Solution &s)
{
    auto &t = s.addStaticLibrary("freedesktop.uchardet", "master");
    t += Git("https://gitlab.freedesktop.org/uchardet/uchardet");

    t += "src/.*"_rr;
    t -= "src/tools/.*"_rr;
}
