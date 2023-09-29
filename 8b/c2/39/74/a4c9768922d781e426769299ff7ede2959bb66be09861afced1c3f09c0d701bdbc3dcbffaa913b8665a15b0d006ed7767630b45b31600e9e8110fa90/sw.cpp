void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.xcb.renderutil", "0.3.10");
    t += Git("https://gitlab.freedesktop.org/xorg/lib/libxcb-render-util", "xcb-util-renderutil-{v}");

    t.ExportAllSymbols = true;
    t += "renderutil/.*\\.[hc]"_r;
    t.Public += "org.sw.demo.xorg.xcb"_dep;
    t.configureFile("renderutil/xcb_renderutil.h", "xcb/xcb_renderutil.h", ConfigureFlags::CopyOnly);
}
