void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.xcb.image", "0.4.1");
    t += Git("https://gitlab.freedesktop.org/xorg/lib/libxcb-image", "xcb-util-image-{v}");

    t.ExportAllSymbols = true;
    t += "image/.*\\.[hc]"_r;
    t.Public += "org.sw.demo.xorg.xcb.util"_dep;
    t.configureFile("image/xcb_image.h", "xcb/xcb_image.h", ConfigureFlags::CopyOnly);
}
