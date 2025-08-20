#pragma sw require header org.sw.demo.google.protobuf.protoc

#include <primitives/emitter.h>
#include <primitives/http.h>

void build(Solution &s)
{
    auto &opencv = s.addProject("intel.opencv", "4.12.0");
    opencv += Git("https://github.com/opencv/opencv", "{v}");
    auto current_project = &opencv;

    auto add_opencl_kernels = [](auto &t)
    {
        String hpp, cpp;

        auto nl = [](auto &&s) {
            return s + "\n"s;
        };

        hpp += nl("// This file is auto-generated. Do not edit!");
        hpp += nl("#include \"opencv2/core/ocl.hpp\"");
        hpp += nl("#include \"opencv2/core/ocl_genbase.hpp\"");
        hpp += nl("#include \"opencv2/core/opencl/ocl_defs.hpp\"");
        hpp += nl("#ifdef HAVE_OPENCL");
        hpp += nl("namespace cv { namespace ocl {");
        hpp += nl("namespace " + t.getPackage().getPath().back() + " {");

        cpp += nl("// This file is auto-generated. Do not edit!");
        cpp += nl("#include \"opencv2/core.hpp\"");
        cpp += nl("#include \"cvconfig.h\"");
        cpp += nl("#include \"opencl_kernels_" + t.getPackage().getPath().back() + ".hpp\"");
        cpp += nl("#ifdef HAVE_OPENCL");
        cpp += nl("namespace cv { namespace ocl {");
        cpp += nl("namespace " + t.getPackage().getPath().back() + " {");
        cpp += nl("static const char* const moduleName = \"" + t.getPackage().getPath().back() + "\";");

        // todo
        //auto files = enumerate_files_like(t.SourceDir, "modules/" + t.getPackage().getPath().back() + "/src/opencl/.*\\.cl");

        hpp += nl("}}}");
        hpp += nl("#endif");

        cpp += nl("}}}");
        cpp += nl("#endif");

        t.writeFileOnce("opencl_kernels_" + t.getPackage().getPath().back() + ".hpp", hpp);
        t.writeFileOnce("opencl_kernels_" + t.getPackage().getPath().back() + ".cpp", cpp);
        t += path("opencl_kernels_" + t.getPackage().getPath().back() + ".cpp");
    };

    auto add_target = [&current_project, &add_opencl_kernels](const String &name) -> decltype(auto)
    {
        auto &t = current_project->addTarget<LibraryTarget>(name);

        t += cpp17;

        t.AllowEmptyRegexes = true;

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
            FileRegex("modules/" + name + "/src/.*lasx.cpp", true),
            FileRegex("modules/" + name + "/src/.*sse4_1.cpp", true),
            FileRegex("modules/" + name + "/src/.*avx512.*", true)
            ;

        t.AllowEmptyRegexes = false;

        t.Private += IncludeDirectory("modules/" + name + "/src");
        t.Public += IncludeDirectory("modules/" + name + "/include");

        t.Private += "__OPENCV_BUILD"_d;
        t.Private += sw::Shared, "CVAPI_EXPORTS"_d;
        t.Public += Definition("HAVE_OPENCV_" + boost::to_upper_copy(name));

        // for msvc
        t += "_USE_MATH_DEFINES"_def;

        t.writeFileOnce("opencv_" + name + "_config.hpp");

        add_opencl_kernels(t);

        return t;
    };

    auto add_dispatch_file = [](auto &t, const String &name)
    {
        t.writeFileOnce(name + ".simd_declarations.hpp",
            "#define CV_CPU_SIMD_FILENAME \"" + name + ".simd.hpp\"\n"
            "#if !defined(__aarch64__)\n"
            "#define CV_CPU_DISPATCH_MODE SSE2\n"
            "#else\n"
            "#define CV_CPU_DISPATCH_MODE NEON\n"
            "#endif\n"
            "#include \"opencv2/core/private/cv_cpu_include_simd_declarations.hpp\"\n"
            "#if !defined(__aarch64__)\n"
            "#define CV_CPU_DISPATCH_MODES_ALL SSE2, BASELINE\n"
            "#else\n"
            "#define CV_CPU_DISPATCH_MODES_ALL NEON, BASELINE\n"
            "#endif\n"
        );
    };

    auto &core = add_target("core");
    {
        core +=
            "cmake/.*\\.in"_rr;
        core.Public += "org.sw.demo.madler.zlib"_dep;

        core.Variables["OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE"] = R"(
            #if !defined(__aarch64__)
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
            #else
                #define CV_CPU_COMPILE_NEON 1
                #define CV_CPU_BASELINE_COMPILE_NEON 1

                //FP16?

                #define CV_CPU_BASELINE_FEATURES 0 \
                    , CV_CPU_NEON

                #define CV_CPU_DISPATCH_FEATURES 0 \
                    , CV_CPU_NEON
            #endif
)";

        core.Variables["OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE"] = R"(
            #define CV_CPU_DISPATCH_COMPILE_FP16 1
            #if !defined(__aarch64__)
                #define CV_CPU_DISPATCH_COMPILE_SSE4_1 1
                #define CV_CPU_DISPATCH_COMPILE_SSE4_2 1
            #else
                #define CV_CPU_DISPATCH_COMPILE_NEON 1
            #endif
)";

        add_dispatch_file(core, "mathfuncs_core");
        add_dispatch_file(core, "stat");
        add_dispatch_file(core, "arithm");
        add_dispatch_file(core, "split");
        add_dispatch_file(core, "convert");
        add_dispatch_file(core, "convert_scale");
        add_dispatch_file(core, "count_non_zero");
        add_dispatch_file(core, "matmul");
        add_dispatch_file(core, "mean");
        add_dispatch_file(core, "sum");
        add_dispatch_file(core, "merge");
        add_dispatch_file(core, "has_non_zero");
        add_dispatch_file(core, "norm");

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

        if (core.getBuildSettings().TargetOS.Type != OSType::Windows &&
            core.getBuildSettings().TargetOS.Type != OSType::Mingw
            )
        {
            core += "dl"_slib;
            if (!core.getBuildSettings().TargetOS.Android)
                core += "pthread"_slib;
        }
        if (core.getBuildSettings().TargetOS.Android)
            core += "log"_slib;
    }

    auto add_target_with_core = [&add_target, &core](const String &name) -> decltype(auto)
    {
        auto &t = add_target(name);
        t.Public += core;
        return t;
    };

    auto &imgproc = add_target_with_core("imgproc");
    add_dispatch_file(imgproc, "accum");
    add_dispatch_file(imgproc, "bilateral_filter");
    add_dispatch_file(imgproc, "box_filter");
    add_dispatch_file(imgproc, "color_hsv");
    add_dispatch_file(imgproc, "color_rgb");
    add_dispatch_file(imgproc, "color_yuv");
    add_dispatch_file(imgproc, "filter");
    add_dispatch_file(imgproc, "median_blur");
    add_dispatch_file(imgproc, "morph");
    add_dispatch_file(imgproc, "smooth");
    add_dispatch_file(imgproc, "sumpixels");

    auto &ml = add_target_with_core("ml");
    auto &flann = add_target_with_core("flann");

    auto &features2d = add_target("features2d");
    features2d.Public += flann, imgproc;
    features2d.Public += "HAVE_OPENCV_FLANN"_def;
    add_dispatch_file(features2d, "sift");

    auto &imgcodecs = add_target("imgcodecs");
    {
        imgcodecs.Public += imgproc;

        imgcodecs += "HAVE_PNG"_def;
        imgcodecs += "org.sw.demo.glennrp.png"_dep;

        imgcodecs += "HAVE_JPEG"_def;
        imgcodecs += "org.sw.demo.jpeg"_dep;

        imgcodecs += "HAVE_WEBP"_def;
        imgcodecs += "org.sw.demo.webmproject.webp"_dep;

        imgcodecs += "HAVE_TIFF"_def;
        imgcodecs += "org.sw.demo.tiff"_dep;

        imgcodecs += "HAVE_JASPER"_def;
        imgcodecs += "org.sw.demo.mdadams.jasper"_dep;

        imgcodecs -= "org.sw.demo.openexr.ilmimf"_dep;
        if (imgcodecs.getBuildSettings().TargetOS.Type != OSType::IOS) {
            imgcodecs += "HAVE_OPENEXR"_def;
            imgcodecs += "org.sw.demo.openexr.ilmimf"_dep;
        }

        imgcodecs += "HAVE_GDCM"_def;
        imgcodecs += "org.sw.demo.malaterre.gdcm.gdcm"_dep;

        imgcodecs -= "org.sw.demo.OSGeo.gdal"_dep;
        if (imgcodecs.getBuildSettings().TargetOS.Type != OSType::IOS) {
            imgcodecs += "HAVE_GDAL"_def;
            imgcodecs += "org.sw.demo.OSGeo.gdal"_dep;
        }

        //imgcodecs -= "modules/imgcodecs/src/apple_conversions.mm";
        imgcodecs -= "modules/imgcodecs/src/macosx_conversions.mm";
        imgcodecs -= "modules/imgcodecs/src/ios_conversions.mm";
        if (imgcodecs.getBuildSettings().TargetOS.isApple()) {
            imgcodecs.Public += "CoreFoundation"_framework;
            imgcodecs.Public += "CoreGraphics"_framework;
            if (imgcodecs.getBuildSettings().TargetOS.Type == OSType::IOS) {
                imgcodecs += "modules/imgcodecs/src/ios_conversions.mm";
                imgcodecs.Public += "UIKit"_framework;
            } else {
                //imgcodecs += "modules/imgcodecs/src/apple_conversions.mm";
                imgcodecs += "modules/imgcodecs/src/macosx_conversions.mm";
                imgcodecs.Public += "AppKit"_framework;
                imgcodecs.Public += "Cocoa"_framework;
            }
        }
    }

    auto &highgui = add_target("highgui");
    {
        highgui.Public += imgcodecs;
        highgui -=
            "modules/highgui/src/window_framebuffer.cpp",
            "modules/highgui/src/window_winrt.cpp",
            "modules/highgui/src/window_winrt_bridge.cpp"
            ;
        if (highgui.getBuildSettings().TargetOS.Type == OSType::Windows ||
            highgui.getBuildSettings().TargetOS.Type == OSType::Mingw
            )
        {
            highgui.Public += "OPENCV_HIGHGUI_BUILTIN_BACKEND_STR=\"WIN32\""_def;
            highgui.Public += "HAVE_WIN32UI"_def;
            highgui.Public +=
                "Comdlg32.lib"_slib,
                "Gdi32.lib"_slib,
                "user32.lib"_slib,
                "advapi32.lib"_slib;
        }
        else
        {
            highgui.Public += "OPENCV_HIGHGUI_BUILTIN_BACKEND_STR=\"GTK\""_def;
            highgui -= "modules/highgui/src/window_w32.cpp";
        }
        if (highgui.getBuildSettings().TargetOS.isApple()) {
            highgui -= "modules/highgui/src/window_cocoa.mm";
        }
    }

    auto &videoio = add_target("videoio");
    {
        videoio.Public += imgcodecs;
        if (videoio.getBuildSettings().TargetOS.Type == OSType::Windows)
        {
            videoio += "HAVE_DSHOW"_def;
            videoio += "HAVE_MSMF"_def;
            videoio += "HAVE_VFW"_def;
            videoio +=
                "Vfw32.lib"_slib,
                "ole32.lib"_slib,
                "OleAut32.lib"_slib,
                "uuid.lib"_slib,
                "Mfplat.lib"_slib,
                "Mfreadwrite.lib"_slib,
                "Mf.lib"_slib,
                "Mfuuid.lib"_slib,
                "comsupp.lib"_slib,
                "Shlwapi.lib"_slib
                ;
        }
        videoio -=
            "modules/videoio/src/cap_android.*"_rr,
            "modules/videoio/src/cap_avfoundation.*"_rr,
            "modules/videoio/src/cap_winrt.*"_rr,
            "modules/videoio/src/cap_mfx.*"_rr,
            "modules/videoio/src/cap_ios.*"_rr,
            "modules/videoio/src/cap_ffmpeg.*"_rr,
            "modules/videoio/src/cap_gstreamer.cpp",
            "modules/videoio/src/cap_ueye.cpp",
            "modules/videoio/src/cap_ximea.cpp",
            "modules/videoio/src/cap_msmf.cpp",
            "modules/videoio/src/cap_xine.cpp";

        if (videoio.getBuildSettings().TargetOS.Type != OSType::Windows &&
            videoio.getBuildSettings().TargetOS.Type != OSType::Mingw
            )
        {
            videoio += "dl"_slib;
        }
        else
        {
            videoio += "modules/videoio/src/cap_msmf.cpp";
        }
        if (imgcodecs.getBuildSettings().TargetOS.isApple()) {
            videoio.Public += "Foundation"_framework;
            videoio.Public += "AVFoundation"_framework;
            videoio.Public += "CoreMedia"_framework;
            videoio.Public += "CoreVideo"_framework;
            videoio.Public += "QuartzCore"_framework;
            if (videoio.getBuildSettings().TargetOS.Type == OSType::IOS) {
                videoio += "modules/videoio/src/cap_ios.*"_rr;
                videoio += "modules/videoio/src/cap_avfoundation.mm";
            } else {
                videoio += "modules/videoio/src/cap_avfoundation_mac.mm";
            }
        }
    }

    auto &calib3d = add_target("calib3d");
    calib3d.Public += features2d;
    add_dispatch_file(calib3d, "undistort");

    auto &photo = add_target("photo");
    photo.Public += imgproc;

    auto &video = add_target("video");
    video.Public += imgproc;

    auto &dnn = add_target("dnn");
    {
        dnn.Public += imgproc;
        dnn -= "modules/dnn/src/cuda4dnn/.*"_rr;
        dnn -= "modules/dnn/src/ocl4dnn/.*"_rr;
        dnn += "HAVE_PROTOBUF"_def;
        dnn += "OPENCV_DNN_EXTERNAL_PROTOBUF"_def;
        add_dispatch_file(dnn, "layers/layers_common");
        add_dispatch_file(dnn, "layers/cpu_kernels/conv_block");
        add_dispatch_file(dnn, "layers/cpu_kernels/conv_winograd_f63");
        add_dispatch_file(dnn, "layers/cpu_kernels/conv_depthwise");
        add_dispatch_file(dnn, "int8layers/layers_common");

        for (auto &f : enumerate_files_like(dnn.SourceDir / "modules/dnn/src", ".*\\.proto", true))
        {
            ProtobufData d;
            d.addIncludeDirectory(f.parent_path());
            gen_protobuf_cpp("org.sw.demo.google.protobuf"_dep, dnn, f, d);
        }
    }

    auto &objdetect = add_target("objdetect");
    objdetect.Public += ml, videoio, calib3d, dnn;
    if (objdetect.getBuildSettings().TargetOS.Type != OSType::Windows &&
        objdetect.getBuildSettings().TargetOS.Type != OSType::Mingw
        )
    {
        if (!objdetect.getBuildSettings().TargetOS.Android)
            objdetect += "pthread"_slib;
    }

    auto &stitching = add_target("stitching");
    stitching.Public += objdetect, calib3d, features2d;
    stitching += "HAVE_OPENCV_XFEATURES2D"_def;

    //
    auto &contrib = opencv.addProject("contrib");
    contrib += Git("https://github.com/opencv/opencv_contrib", "{v}");
    current_project = &contrib;

    auto &aruco = add_target("aruco");
    aruco.Public += calib3d, objdetect;

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
    freetype.Public += imgproc, "org.sw.demo.behdad.harfbuzz"_dep;

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

    auto &quality = add_target("quality");
    quality.Public += imgproc, ml;

    auto &rgbd = add_target("rgbd");
    rgbd.Public += calib3d;

    auto &saliency = add_target("saliency");
    saliency.Public += videoio, highgui;

    auto &stereo = add_target("stereo");
    stereo.Public += calib3d, videoio, video, highgui;

    auto &surface_matching = add_target("surface_matching");
    surface_matching.Public += flann;

    auto &xfeatures2d = add_target("xfeatures2d");
    {
        xfeatures2d.Public += features2d;
        xfeatures2d += "OPENCV_XFEATURES2D_HAS_VGG_DATA"_def;

        if (!s.DryRun)
        {
            String url = "https://raw.githubusercontent.com/opencv/opencv_3rdparty/fccf7cd6a4b12079f73bbfb21745f9babcd4eb1d/";
            for (auto f : { "vgg_generated_48.i","vgg_generated_64.i","vgg_generated_80.i","vgg_generated_120.i" })
            {
                if (!fs::exists(xfeatures2d.BinaryDir / f))
                    downloadFile(url + f, xfeatures2d.BinaryDir / f);
            }
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
            {
                if (!fs::exists(xfeatures2d.BinaryDir / f))
                    downloadFile(url + f, xfeatures2d.BinaryDir / f);
            }
        }
    }

    auto &ximgproc = add_target("ximgproc");
    ximgproc.Public += calib3d, video, videoio, highgui;
    ximgproc.pushFrontToFileOnce("modules/ximgproc/src/fourier_descriptors.cpp", "#define M_PI 3.14159265358979323846");

    auto &xobjdetect = add_target("xobjdetect");
    xobjdetect.Public += highgui, objdetect;

    auto &xphoto = add_target("xphoto");
    xphoto.Public += photo;

    auto &tracking = add_target("tracking");
    tracking.Public += dnn, datasets, highgui, video, videoio;

    auto &structured_light = add_target("structured_light");
    structured_light.Public += phase_unwrapping, calib3d;

    auto &optflow = add_target("optflow");
    optflow.Public += ximgproc;

    auto &text = add_target("text");
    text.Public += imgproc, dnn, ml, "org.sw.demo.google.tesseract.libtesseract"_dep;
    text.writeFileOnce("text_config.hpp");
    text.Public += "HAVE_TESSERACT"_def;

    auto &hdf = add_target("hdf");
    hdf.Public += imgproc, "org.sw.demo.hdfgroup.hdf5.hdf5"_dep;

    auto &shape = add_target("shape");
    shape.Public += calib3d, imgproc;

    auto &superres = add_target("superres");
    superres.Public += video, videoio, optflow;

    //
    stitching.Public += xfeatures2d;
}
