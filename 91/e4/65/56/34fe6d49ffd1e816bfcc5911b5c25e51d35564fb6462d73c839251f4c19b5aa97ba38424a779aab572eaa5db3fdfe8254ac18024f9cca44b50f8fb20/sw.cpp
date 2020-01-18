void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.xtrans", "1.4.0");
    t += Git("https://gitlab.freedesktop.org/xorg/lib/libxtrans", "xtrans-{v}");

    t.setChecks("x", true);

    t.ExportAllSymbols = true;

    t -= ".*\\.[hc]"_r;
    //t += "transport.c";

    t.Public += "org.sw.demo.xorg.proto"_dep;

    //t.LinkOptions.push_back("-Wl,--no-undefined");

    for (auto &[p, sf] : t[".*\\.[hc]"_r])
        t.configureFile(p, "X11/Xtrans/" + p.filename().string(), ConfigureFlags::CopyOnly);
}

void check(Checker &c)
{
    auto &s = c.addSet("x");
    s.checkFunctionExists("fchown");
}
