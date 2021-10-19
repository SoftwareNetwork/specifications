void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.proto.xcb", "1.14.0");
    t += Git("https://gitlab.freedesktop.org/xorg/proto/xcbproto", "xcb-proto-{M}.{m}");

    t += "src/.*\\.xml"_r;
    t += "src/.*\\.xsd"_r;
    t += "xcbgen/.*\\.py"_r;
}
