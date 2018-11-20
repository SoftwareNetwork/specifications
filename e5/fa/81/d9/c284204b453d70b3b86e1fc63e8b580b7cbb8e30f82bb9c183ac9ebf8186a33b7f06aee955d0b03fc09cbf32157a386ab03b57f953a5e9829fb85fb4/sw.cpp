#pragma sw require pub.egorpugin.primitives.context-master
#pragma sw require pub.egorpugin.primitives.http-master
#pragma sw require header org.sw.demo.google.protobuf.protoc-3

#include <primitives/context.h>
#include <primitives/http.h>

void build(Solution &s)
{
    auto &opencv = s.addProject("intel.opencv", "4.0.0");
    opencv += Git("https://github.com/opencv/opencv", "{v}");
    auto current_project = &opencv;

    auto add_opencl_kernels = [](auto &t)
    {
        primitives::CppContext hpp, cpp;
        hpp.addLine("// This file is auto-generated. Do not edit!");
        hpp.addLine("#include \"opencv2/core/ocl.hpp\"");
        hpp.addLine("#include \"opencv2/core/ocl_genbase.hpp\"");
        hpp.addLine("#include \"opencv2/core/opencl/ocl_defs.hpp\"");
        hpp.addLine("#ifdef HAVE_OPENCL");
        hpp.addLine("namespace cv { namespace ocl {");
        hpp.addLine("namespace " + t.pkg.ppath.back() + " {");

        cpp.addLine("// This file is auto-generated. Do not edit!");
        cpp.addLine("#include \"opencv2/core.hpp\"");
        cpp.addLine("#include \"cvconfig.h\"");
        cpp.addLine("#include \"opencl_kernels_" + t.pkg.ppath.back() + ".hpp\"");
        cpp.addLine("#ifdef HAVE_OPENCL");
        cpp.addLine("namespace cv { namespace ocl {");
        cpp.addLine("namespace " + t.pkg.ppath.back() + " {");
        cpp.addLine("static const char* const moduleName = \"" + t.pkg.ppath.back() + "\";");

        // todo
        //auto files = enumerate_files_like(t.SourceDir, "modules/" + t.pkg.ppath.back() + "/src/opencl/.*\\.cl");

        hpp.addLine("}}}");
        hpp.addLine("#endif");

        cpp.addLine("}}}");
        cpp.addLine("#endif");

        t.writeFileOnce("opencl_kernels_" + t.pkg.ppath.back() + ".hpp", hpp.getText());
        t.writeFileOnce("opencl_kernels_" + t.pkg.ppath.back() + ".cpp", cpp.getText());
        t += path("opencl_kernels_" + t.pkg.ppath.back() + ".cpp");
    };

    auto add_target = [&current_project, &add_opencl_kernels](const String &name) -> decltype(auto)
    {
        auto &t = current_project->addTarget<LibraryTarget>(name);

        t +=
            FileRegex("modules/" + name + "/include/.*", true),
            FileRegex("modules/" + name + "/src/.*", true),
            FileRegex("modules/" + name + "/src/cuda/.*.cu", true),
            FileRegex("modules/" + name + "/src/opencl/.*.cl", true);

        t -=
            FileRegex("modules/" + name + "/src/cuda/.*", true),
            FileRegex("modules/" + name + "/src/opencl/.*", true);

        t -=
            FileRegex("modules/" + name + "/src/.*avx.cpp", true),
            FileRegex("modules/" + name + "/src/.*avx2.cpp", true),
            FileRegex("modules/" + name + "/src/.*sse4_1.cpp", true);

        t.Private += IncludeDirectory("modules/" + name + "/src");
        t.Public += IncludeDirectory("modules/" + name + "/include");

        t.Private += "__OPENCV_BUILD"_d;
        t.Private += sw::Shared, "CVAPI_EXPORTS"_d;

        add_opencl_kernels(t);

        return t;
    };

    auto add_dispatch_file = [](auto &t, const String &name)
    {
        t.writeFileOnce(name + ".simd_declarations.hpp",
            "#define CV_CPU_SIMD_FILENAME \"" + name + ".simd.hpp\"\n"
            "#define CV_CPU_DISPATCH_MODE SSE2\n"
            "#include \"opencv2/core/private/cv_cpu_include_simd_declarations.hpp\"\n"
            "#define CV_CPU_DISPATCH_MODES_ALL SSE2, BASELINE\n"
        );
    };

    auto &core = add_target("core");
    {
        core +=
            "cmake/.*\\.in"_rr;
        core.Public += "org.sw.demo.madler.zlib-1"_dep;

        core.Variables["OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE"] = R"(
            #define CV_CPU_COMPILE_SSE 1
            #define CV_CPU_BASELINE_COMPILE_SSE 1

            #define CV_CPU_COMPILE_SSE2 1
            #define CV_CPU_BASELINE_COMPILE_SSE2 1

            #define CV_CPU_BASELINE_FEATURES 0 \
                , CV_CPU_SSE \
                , CV_CPU_SSE2

            #define CV_CPU_DISPATCH_FEATURES 0 \
                , CV_CPU_SSE4_1 \
                , CV_CPU_SSE4_2 \
                , CV_CPU_FP16 \
                , CV_CPU_AVX
)";

        core.Variables["OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE"] = R"(
            #define CV_CPU_DISPATCH_COMPILE_SSE4_1 1
            #define CV_CPU_DISPATCH_COMPILE_SSE4_2 1
            #define CV_CPU_DISPATCH_COMPILE_FP16 1
)";

        add_dispatch_file(core, "mathfuncs_core");
        add_dispatch_file(core, "stat");
        add_dispatch_file(core, "arithm");

        core.writeFileOnce("opencv_data_config.hpp",
            "#define OPENCV_INSTALL_PREFIX \"\"\n"
            "#define OPENCV_DATA_INSTALL_PATH \"\"\n"
            "#define OPENCV_BUILD_DIR \"\"\n"
            "#define OPENCV_DATA_BUILD_DIR_SEARCH_PATHS \"\"\n"
            "#define OPENCV_INSTALL_DATA_DIR_RELATIVE \"\"\n"
        );
        core.writeFileOnce("opencv2/opencv_modules.hpp");
        core.writeFileOnce("version_string.inc", "\"OpenCV - ${PACKAGE_NAME} - ${PACKAGE_VERSION}\\n\"\n\"Powered by Software Network\"");
        core.configureFile("cmake/templates/cvconfig.h.in", "cvconfig.h");
        core.configureFile("cmake/templates/cv_cpu_config.h.in", "cv_cpu_config.h");
        core.configureFile("cmake/templates/custom_hal.hpp.in", "custom_hal.hpp");
    }

    auto add_target_with_core = [&add_target, &core](const String &name) -> decltype(auto)
    {
        auto &t = add_target(name);
        t.Public += core;
        return t;
    };

    auto &imgproc = add_target_with_core("imgproc");
    add_dispatch_file(imgproc, "accum");

    auto &ml = add_target_with_core("ml");
    auto &flann = add_target_with_core("flann");

    auto &features2d = add_target("features2d");
    features2d.Public += flann, imgproc;
    features2d.Public += "HAVE_OPENCV_FLANN"_def;

    auto &imgcodecs = add_target("imgcodecs");
    {
        imgcodecs.Public += imgproc;

        imgcodecs.Public += "HAVE_OPENCV_IMGCODECS"_def;

        imgcodecs += "HAVE_PNG"_def;
        imgcodecs += "org.sw.demo.glennrp.png-1"_dep;

        imgcodecs += "HAVE_JPEG"_def;
        imgcodecs += "org.sw.demo.jpeg-*"_dep;

        imgcodecs += "HAVE_WEBP"_def;
        imgcodecs += "org.sw.demo.webmproject.webp-1"_dep;

        imgcodecs += "HAVE_TIFF"_def;
        imgcodecs += "org.sw.demo.tiff-4"_dep;

        imgcodecs += "HAVE_JASPER"_def;
        imgcodecs += "org.sw.demo.mdadams.jasper-2"_dep;

        imgcodecs += "HAVE_OPENEXR"_def;
        imgcodecs += "org.sw.demo.openexr.ilmimf-2"_dep;

        imgcodecs += "HAVE_GDCM"_def;
        imgcodecs += "org.sw.demo.malaterre.gdcm.gdcm-2"_dep;
    }

    auto &highgui = add_target("highgui");
    {
        highgui.Public += imgcodecs;
        highgui -=
            "modules/highgui/src/window_carbon.cpp",
            "modules/highgui/src/window_winrt.cpp",
            "modules/highgui/src/window_winrt_bridge.cpp"
            ;
        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            highgui.Public += "HAVE_WIN32UI"_def;
            highgui.Public +=
                "Comdlg32.lib"_lib,
                "Gdi32.lib"_lib,
                "user32.lib"_lib,
                "advapi32.lib"_lib;
        }
    }

    auto &videoio = add_target("videoio");
    {
        videoio.Public += imgcodecs;
        videoio -=
            "modules/videoio/src/cap_winrt.*"_rr,
            "modules/videoio/src/cap_mfx.*"_rr,
            "modules/videoio/src/cap_giganetix.cpp",
            "modules/videoio/src/cap_gstreamer.cpp",
            "modules/videoio/src/cap_qt.cpp",
            "modules/videoio/src/cap_unicap.cpp",
            "modules/videoio/src/cap_ximea.cpp",
            "modules/videoio/src/cap_xine.cpp";
    }

    auto &calib3d = add_target("calib3d");
    calib3d.Public += features2d;

    auto &objdetect = add_target("objdetect");
    objdetect.Public += ml, videoio, calib3d;

    auto &photo = add_target("photo");
    photo.Public += imgproc;

    auto &video = add_target("video");
    video.Public += imgproc;

    auto &dnn = add_target("dnn");
    {
        dnn.Public += imgproc;
        dnn -= "modules/dnn/src/ocl4dnn/.*"_rr;
        dnn += "HAVE_PROTOBUF"_def;
        dnn += "OPENCV_DNN_EXTERNAL_PROTOBUF"_def;
        add_dispatch_file(dnn, "layers/layers_common");

        for (auto &f : enumerate_files_like(dnn.SourceDir / "modules/dnn/src", ".*\\.proto", true))
            gen_protobuf(dnn, f);
    }

    auto &stitching = add_target("stitching");
    stitching.Public += objdetect, calib3d, features2d;
    stitching += "HAVE_OPENCV_XFEATURES2D"_def;

    //
    auto &contrib = opencv.addProject("contrib");
    contrib += Git("https://github.com/opencv/opencv_contrib", "{v}");
    current_project = &contrib;

    //if (contrib.Local)
        //contrib.SourceDir /= path("..") / "opencv_contrib";

    auto &aruco = add_target("aruco");
    aruco.Public += calib3d;

    auto &bgsegm = add_target("bgsegm");
    bgsegm.Public += calib3d, video;
    bgsegm.pushFrontToFileOnce("modules/bgsegm/src/synthetic_seq.cpp", "#define M_PI 3.14159265358979323846");

    auto &bioinspired = add_target_with_core("bioinspired");

    auto &ccalib = add_target("ccalib");
    ccalib.Public += calib3d, highgui;

    auto &datasets = add_target("datasets");
    datasets.Public += videoio, highgui;

    auto &dpm = add_target("dpm");
    dpm.Public += videoio, imgproc;

    auto &face = add_target("face");
    face.Public += imgproc, objdetect;

    auto &freetype = add_target("freetype");
    freetype.Public += imgproc, "org.sw.demo.behdad.harfbuzz-*"_dep;

    auto &fuzzy = add_target("fuzzy");
    fuzzy.Public += imgproc;

    auto &line_descriptor = add_target("line_descriptor");
    line_descriptor.Public += features2d, videoio, highgui;

    auto &phase_unwrapping = add_target("phase_unwrapping");
    phase_unwrapping.Public += imgproc;

    auto &plot = add_target("plot");
    plot.Public += imgproc;

    auto &reg = add_target("reg");
    reg.Public += imgproc;

    auto &rgbd = add_target("rgbd");
    rgbd.Public += calib3d;

    auto &saliency = add_target("saliency");
    saliency.Public += videoio, highgui;

    auto &stereo = add_target("stereo");
    stereo.Public += calib3d, videoio, highgui;

    auto &surface_matching = add_target("surface_matching");
    surface_matching.Public += flann;

    auto &xfeatures2d = add_target("xfeatures2d");
    {
        xfeatures2d.Public += features2d;

        if (!s.DryRun)
        {
            String url = "https://raw.githubusercontent.com/opencv/opencv_3rdparty/fccf7cd6a4b12079f73bbfb21745f9babcd4eb1d/";
            for (auto f : { "vgg_generated_48.i","vgg_generated_64.i","vgg_generated_80.i","vgg_generated_120.i" })
                download_file(url + f, xfeatures2d.BinaryDir / f);
            url = "https://raw.githubusercontent.com/opencv/opencv_3rdparty/34e4206aef44d50e6bbcd0ab06354b52e7466d26/";
            for (auto f : {
                     "boostdesc_bgm.i",
                     "boostdesc_bgm_bi.i",
                     "boostdesc_bgm_hd.i",
                     "boostdesc_binboost_064.i",
                     "boostdesc_binboost_128.i",
                     "boostdesc_binboost_256.i",
                     "boostdesc_lbgm.i",
                })
                download_file(url + f, xfeatures2d.BinaryDir / f);
        }
    }

    auto &ximgproc = add_target("ximgproc");
    ximgproc.Public += calib3d, videoio, highgui;
    ximgproc.pushFrontToFileOnce("modules/ximgproc/src/fourier_descriptors.cpp", "#define M_PI 3.14159265358979323846");

    auto &xobjdetect = add_target("xobjdetect");
    xobjdetect.Public += highgui, objdetect;

    auto &xphoto = add_target("xphoto");
    xphoto.Public += imgproc;

    auto &tracking = add_target("tracking");
    tracking.Public += dnn, datasets, highgui, video, videoio;

    auto &structured_light = add_target("structured_light");
    structured_light.Public += phase_unwrapping, calib3d;

    auto &optflow = add_target("optflow");
    optflow.Public += ximgproc, video;

    auto &text = add_target("text");
    text.Public += imgproc, dnn, ml, "org.sw.demo.google.tesseract.libtesseract-master"_dep;
    text.writeFileOnce("text_config.hpp");

    auto &hdf = add_target("hdf");
    hdf.Public += imgproc, "org.sw.demo.hdfgroup.hdf5.hdf5-1"_dep;

    auto &shape = add_target("shape");
    shape.Public += calib3d, imgproc;

    auto &superres = add_target("superres");
    superres.Public += video, videoio, optflow;

    //
    stitching.Public += xfeatures2d;
}
