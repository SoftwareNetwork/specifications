void build(Solution &s)
{
    auto &t = s.addLibrary("jxl", "0.10.0");
    t += Git("https://github.com/libjxl/libjxl", "v{v}");

    t += cpp11;
    t += "JXL_EXPORT"_api;
    t += "JXL_CMS_EXPORT"_api;

    t += "lib/include/.*"_rr;
    t += "lib/jxl/.*"_rr;
    //t += "lib/jxl/modular/.*"_r;
    t -= "lib/jxl/.*bench.*"_rr;
    t -= "lib/jxl/.*test.*"_rr;

    t.Public += "JXL_DEPRECATED="_def;

    t += "."_idir;
    t.Public += "lib/include"_idir;

    t += "org.sw.demo.google.highway"_dep; // public?
    t += "org.sw.demo.google.brotli"_dep;
    t += "org.sw.demo.ArtifexSoftware.thirdparty.lcms2"_dep;

    t.writeFileOnce("jxl/jxl_export.h");
    t.writeFileOnce("jxl/jxl_cms_export.h");

    t.Variables["JPEGXL_MAJOR_VERSION"] = std::to_string(t.getPackage().getVersion().getMajor());
    t.Variables["JPEGXL_MINOR_VERSION"] = std::to_string(t.getPackage().getVersion().getMinor());
    t.Variables["JPEGXL_PATCH_VERSION"] = std::to_string(t.getPackage().getVersion().getPatch());
    t.configureFile("lib/jxl/version.h.in", "jxl/version.h");
}
