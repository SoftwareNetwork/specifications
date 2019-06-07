void build(Solution &s)
{
    auto &p = s.addProject("ghostscript", "1.0.5");
    p += Git("git://git.ghostscript.com/mujs.git");

    auto &t = p.addStaticLibrary("mujs");
    t -= ".*\\.[hc]"_r;
    t -= "main.c";
    t += "one.c";
}
