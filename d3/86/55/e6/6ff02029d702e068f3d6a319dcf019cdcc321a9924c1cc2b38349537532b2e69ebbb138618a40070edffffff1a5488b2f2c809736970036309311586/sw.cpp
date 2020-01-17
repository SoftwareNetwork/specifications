void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.X11.auth", "1.0.9");
    t += Git("https://gitlab.freedesktop.org/xorg/lib/libxau", "libXau-{v}");

    t.ExportAllSymbols = true;

    t += "include/.*"_rr;
    t += ".*\\.[hc]"_r;
    t -= "Autest.c";

    t.Public += "org.sw.demo.xorg.proto"_dep;
}
