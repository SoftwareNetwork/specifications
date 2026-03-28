#pragma sw require header org.sw.demo.find.nvidia.cuda

void build(Solution &s)
{
    auto &dlib = s.addTarget<StaticLibraryTarget>("davisking.dlib", "20.0.0");
    {
        auto &t = dlib;
        t += Git("https://github.com/davisking/dlib", "v{M}.{m}");
        t -= "dlib/.*"_rr;
        t += "dlib/all/.*"_rr;
        //t -= "dlib/cuda/.*"_rr;
        //t -= "dlib/external/.*"_rr;
        //t -= "dlib/test/.*"_rr;
        //t -= "dlib/cmake_.*"_rr;
        //t -= "dlib/matlab/example_.*"_rr;

        t.Public += "DLIB_PNG_SUPPORT"_def;
        t.Public += "org.sw.demo.glennrp.png"_dep;
        t.Public += "DLIB_JPEG_SUPPORT"_def;
        t.Public += "org.sw.demo.jpeg"_dep;
        t.Public += "DLIB_GIF_SUPPORT"_def;
        t.Public += "org.sw.demo.gif"_dep;
        t.Public += "DLIB_WEBP_SUPPORT"_def;
        t.Public += "org.sw.demo.webmproject.webp"_dep;
        t.Public += "DLIB_JXL_SUPPORT"_def;
        t.Public += "org.sw.demo.jxl"_dep;

        //t += "DLIB_NO_GUI_SUPPORT"_def;
        t += "DLIB_HAVE_AVX"_def;

        t -= "org.sw.demo.find.nvidia.cuda"_dep;
        t -= "org.sw.demo.find.nvidia.cudnn"_dep;
        if (t.Variables["with-cuda"] == "true"s) {
            t += "dlib/cuda/.*"_rr;
            t.Public += "DLIB_USE_CUDA"_def;
            t.Public += "org.sw.demo.find.nvidia.cuda"_dep;
            t.Public += "org.sw.demo.find.nvidia.cudnn"_dep;
            setup_cuda_for_target(t);
        }

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

    auto add_example = [&](auto &&n)
    {
        auto &t = dlib.addExecutable(n);
        t.Publish = false;
        t += cpp20;
        t += path{"examples/"s + n + ".cpp"s};
        auto d = t + dlib;
        d->Variables["with-cuda"] = "true";
    };
    add_example("dnn_metric_learning_ex");
}
