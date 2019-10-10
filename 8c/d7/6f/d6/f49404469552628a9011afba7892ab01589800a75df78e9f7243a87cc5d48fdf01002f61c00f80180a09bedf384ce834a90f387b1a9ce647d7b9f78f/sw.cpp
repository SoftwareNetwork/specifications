void build(Solution &s)
{
    auto &t = s.addLibrary("libraw", "0.19.5");
    t += RemoteFile("https://www.libraw.org/data/LibRaw-{v}.tar.gz");

    t += "internal/.*"_rr;
    t += "libraw/.*"_rr;
    t += "src/.*"_rr;
    t -= "src/libraw_fuji_compressed.cpp";
    t -= "internal/libraw_x3f.cpp";
    t -= "internal/.*_demosaic.*"_rr;
    t -= "internal/wf_filtering.cpp";

    t.Public += "."_idir;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t.Public += "WIN32"_def;

        t += "ws2_32.lib"_slib;
    }

    t.Public += sw::Static, "LIBRAW_NODLL"_def;
    t.Public += "LIBRAW_BUILDLIB"_def;
}
