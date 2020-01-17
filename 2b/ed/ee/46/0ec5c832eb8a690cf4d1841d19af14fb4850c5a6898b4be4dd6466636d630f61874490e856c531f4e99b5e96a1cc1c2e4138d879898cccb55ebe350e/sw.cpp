void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.proto", "2019.2.0");
    t += Git("https://gitlab.freedesktop.org/xorg/proto/xorgproto", "xorgproto-{M}.{m}");
    t += "include/.*\\.h"_rr;
}
