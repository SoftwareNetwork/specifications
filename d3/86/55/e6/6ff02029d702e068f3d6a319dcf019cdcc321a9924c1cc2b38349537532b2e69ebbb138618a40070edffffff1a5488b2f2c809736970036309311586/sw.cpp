void build(Solution &s)
{
    auto &t = s.addLibrary("xorg.X11.auth", "1.0.9");
    t += Git("https://gitlab.freedesktop.org/xorg/lib/libxau", "libXau-{v}");

    t.ExportAllSymbols = true;

    t += "include/.*"_rr;
    t += ".*\\.[hc]"_r;
    t -= "Autest.c";

    t.Public += "org.sw.demo.xorg.proto"_dep;

    if (t.getBuildSettings().TargetOS.is(OSType::Windows))
    {
        t.writeFileOnce("unistd.h", R"(
#define R_OK    4       /* Test for read permission.  */
#define W_OK    2       /* Test for write permission.  */
//#define   X_OK    1       /* execute permission - unsupported in windows*/
#define F_OK    0       /* Test for existence.  */
)");
        t.writeFileOnce("sys/time.h", "#include <time.h>");
    }
}
