void build(Solution &s)
{
    auto &p = s.addStaticLibrary("powturbo.TurboPFor", "master");
    p += Git("https://github.com/powturbo/TurboPFor");

    p += ".*\\.[h]"_r;

    p +=
        "bitpack.c",
        "bitunpack.c",
        "vp4c.c",
        "vp4d.c",
        "transpose.c",
        "bitutil.c",
        "fp.c",
        "vsimple.c",
        "vint.c"
        ;
}
