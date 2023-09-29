void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.xcb.cursor", "0.1.4");
    t += Git("https://gitlab.freedesktop.org/xorg/lib/libxcb-cursor", "xcb-util-cursor-{v}");

    t.ExportAllSymbols = true;
    t += "cursor/.*\\.[hc]"_r;
    t += "XCURSORPATH=\"~/.icons:/usr/share/icons:/usr/share/pixmaps:/usr/X11R6/lib/X11/icons\""_def;
    t.Public += "org.sw.demo.xorg.xcb.image"_dep;
    t.Public += "org.sw.demo.xorg.xcb.renderutil"_dep;
    t.configureFile("cursor/xcb_cursor.h", "xcb/xcb_cursor.h", ConfigureFlags::CopyOnly);

    t.addCommand()
        << cmd::prog("org.sw.demo.gnu.gperf"_dep)
        << cmd::wdir(t.BinaryDir)
        << "--includes"
        << "--struct-type"
        << "--language=ANSI-C"
        << "--switch=1"
        << ("--output-file=shape_to_id.c")
        << cmd::in("cursor/shape_to_id.gperf")
        << cmd::end()
        << cmd::out("shape_to_id.c")
        ;
}
