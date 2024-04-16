void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.proto.xcb", "1.17.0");
    t += Git("https://gitlab.freedesktop.org/xorg/proto/xcbproto", "xcb-proto-{v}");

    t += "src/.*\\.xml"_r;
    t += "src/.*\\.xsd"_r;
    t += "xcbgen/.*\\.py"_r;
}
