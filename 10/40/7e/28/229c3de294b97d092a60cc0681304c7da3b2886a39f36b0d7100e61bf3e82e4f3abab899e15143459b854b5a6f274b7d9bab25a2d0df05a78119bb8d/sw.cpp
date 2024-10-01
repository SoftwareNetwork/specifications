void build(Solution &s)
{
    auto &t = s.addLibrary("lighttransport.tinyusdz", "dev");
    t += Git("https://github.com/lighttransport/tinyusdz");

    t += cpp20;
    t += "src/.*"_r;
    t += "src/tydra/.*"_rr;
    // TODO: add nanobind separately
    //t += "src/external/nanobind/ext/.*"_rr;
    t += "src/external/nanobind/include/.*"_rr;
    t += "src/external/nanobind/src/.*"_rr;
    //t += "src/osd/opensubdiv/.*"_rr;

    //t -= "src/osd/opensubdiv/osd/.*"_rr;
    //t += "src/osd/opensubdiv/osd/cpu.*"_rr;
    //t -= "src/osd/opensubdiv/osd/cpuGLVertexBuffer.cpp";

    t.Public += "src"_idir;
    //t += "src/external/nanobind/ext/robin_map/include"_idir;
    t += "src/external/nanobind/include"_idir;
    //t += "src/osd"_idir;

    t += "TINYUSDZ_COMPILE_LIBRARY"_def;
    t.Public += sw::Shared, "TINYUSDZ_SHARED_LIBRARY"_def;

    t += "USD_EXPORTS"_def;
    t.Public += sw::Shared, "PXR_DYNAMIC"_def;

    t += "org.sw.demo.pybind.pybind11"_dep;
    t += "org.sw.demo.python.lib"_dep;
    t += "org.sw.demo.Tessil.robin_map"_dep;
    t += "org.sw.demo.lz4"_dep;
    t += "org.sw.demo.PixarAnimationStudios.OpenSubdiv"_dep;

    t.patch("src/lz4-compression.cc", "lz4/lz4.h", "lz4.h");
}
