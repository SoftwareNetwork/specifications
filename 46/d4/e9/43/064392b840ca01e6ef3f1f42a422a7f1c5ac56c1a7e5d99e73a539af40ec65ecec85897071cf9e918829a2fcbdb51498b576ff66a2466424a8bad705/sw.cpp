void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.xcb.util", "0.4.1");
    t += Git("https://gitlab.freedesktop.org/xorg/lib/libxcb-util", "xcb-util-{v}");

    t.ExportAllSymbols = true;
    t += "src/.*\\.[hc]"_r;
    t.Public += "org.sw.demo.xorg.xcb"_dep;
    t.configureFile("src/xcb_util.h", "xcb/xcb_util.h", ConfigureFlags::CopyOnly);
}
