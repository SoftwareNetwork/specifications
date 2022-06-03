void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.proto.xcb", "1.15.0");
    t += Git("https://gitlab.freedesktop.org/xorg/proto/xcbproto", "xcb-proto-{M}.{m}{po}");

    t += "src/.*\\.xml"_r;
    t += "src/.*\\.xsd"_r;
    t += "xcbgen/.*\\.py"_r;
}
