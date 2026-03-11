void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("davisking.dlib", "20.0.0");
    t += Git("https://github.com/davisking/dlib", "v{M}.{m}");
    t -= "dlib/.*"_rr;
    t += "dlib/all/.*"_rr;
    //t -= "dlib/cuda/.*"_rr;
    //t -= "dlib/external/.*"_rr;
    //t -= "dlib/test/.*"_rr;
    //t -= "dlib/cmake_.*"_rr;
    //t -= "dlib/matlab/example_.*"_rr;

    t += "DLIB_PNG_SUPPORT"_def;
    t.Public += "org.sw.demo.glennrp.png"_dep;
    t += "DLIB_JPEG_SUPPORT"_def;
    t.Public += "org.sw.demo.jpeg"_dep;

    //t += "DLIB_NO_GUI_SUPPORT"_def;

    if (t.getCompilerType() == CompilerType::MSVC) {
        t.Public.CompileOptions.push_back("/bigobj");
    }
    if (t.getBuildSettings().TargetOS.Type == OSType::Windows || t.getBuildSettings().TargetOS.Type == OSType::Mingw) {
        t += "ws2_32.lib"_slib;
        t += "gdi32.lib"_slib;
        t += "User32.lib"_slib;
        t += "imm32.lib"_slib;
        t += "Winmm.lib"_slib;
        t += "comctl32.lib"_slib;
    }
}
