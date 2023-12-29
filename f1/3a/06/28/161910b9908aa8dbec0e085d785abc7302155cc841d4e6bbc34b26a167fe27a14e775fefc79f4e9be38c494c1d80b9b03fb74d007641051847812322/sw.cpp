void build(Solution &s)
{
    auto &t = s.addLibrary("jxl", "0.9.0");
    t += Git("https://github.com/libjxl/libjxl", "v{v}");

    t += cpp11;
    t += "JXL_EXPORT"_api;

    t += "lib/include/.*"_rr;
    t += "lib/jxl/.*"_rr;
    //t += "lib/jxl/modular/.*"_r;
    t -= "lib/jxl/.*bench.*"_r;
    t -= "lib/jxl/.*test.*"_r;

    t.Public += "JXL_DEPRECATED="_def;
    t += Definition("JPEGXL_MAJOR_VERSION=" + std::to_string(t.getPackage().getVersion().getMajor()));
    t += Definition("JPEGXL_MINOR_VERSION=" + std::to_string(t.getPackage().getVersion().getMinor()));
    t += Definition("JPEGXL_PATCH_VERSION=" + std::to_string(t.getPackage().getVersion().getPatch()));

    t += "."_idir;
    t.Public += "lib/include"_idir;

    t += "org.sw.demo.google.highway-0.15"_dep; // public?
    t += "org.sw.demo.google.brotli"_dep;
    t += "org.sw.demo.ArtifexSoftware.thirdparty.lcms2"_dep;

    t.writeFileOnce("jxl/jxl_export.h");
}
