void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo.intel");
    auto &opencv = s.addProject("opencv", "3.3.1");
    opencv.Source = Git("https://github.com/opencv/opencv", "{v}");

    auto cl2cpp = [](auto &t)
    {
        const auto m = t.getPackage().ppath.back();
        const Files inputs = enumerate_files_like(t.SourceDir / "modules" / "core" / "src" / "opencl", ".*\\.cl", false);
        const auto o_hpp = t.BinaryDir / ("opencl_kernels_" + m + ".hpp");
        const auto o_cpp = t.BinaryDir / ("opencl_kernels_" + m + ".cpp");
        auto c = std::make_shared<ExecuteCommand>([m, inputs, o_hpp, o_cpp]()
        {
            std::string k_hpp, k_cpp;
            k_hpp += R"(#include "opencv2/core/ocl.hpp"
#include "opencv2/core/ocl_genbase.hpp"
#include "opencv2/core/opencl/ocl_defs.hpp"

#ifdef HAVE_OPENCL

namespace cv
{
namespace ocl
{
namespace )" + m + R"(
{

)";
            k_cpp += R"(#include "precomp.hpp"
#include "cvconfig.h"
#include "opencl_kernels_)" + m + R"(.hpp"

#ifdef HAVE_OPENCL

namespace cv
{
namespace ocl
{
namespace )" + m + R"(
{

static const char* const moduleName = ")" + m + R"(";

)";
            for (auto &cl : inputs)
            {
                auto f = read_file(cl);
                auto lines = split_lines(f);
                k_hpp += "extern struct cv::ocl::internal::ProgramEntry " + cl.filename().stem().string() + "_oclsrc;\n";
                k_cpp += "struct cv::ocl::internal::ProgramEntry arithm_oclsrc={moduleName, \"" + m + "\",\n";
                for (auto &l : lines)
                    k_cpp += "R\"xxx(" + l + "\\n)xxx\"\n";
                k_cpp += ", \"" + md5(f) + "\", NULL};\n\n";
            }
            k_hpp += R"(
}}}
#endif
)";
            k_cpp += R"(
}}}
#endif
)";
            write_file(o_hpp, k_hpp);
            write_file(o_cpp, k_cpp);
        });
        c->addInput(inputs);
        c->addOutput(o_hpp);
        c->addOutput(o_cpp);
        t += o_hpp, o_cpp;
    };

    auto add_module = [&cl2cpp](auto &t, const std::string &m) -> decltype(auto)
    {
        auto &module = t.addTarget<LibraryTarget>(m);

        const auto mdir = "modules/"s + m;

        for (auto &r : { "include/.*", "src/.*"/*, "src/cuda/.*.cu", "src/opencl/.*.cl"*/ })
            module += FileRegex(path(mdir), std::regex(r), true);

        // drop cuda, cl
        for (auto &r : { "src/cuda/.*.cu", "src/opencl/.*.cl" })
            module -= FileRegex(path(mdir), std::regex(r), true);

        // drop instruction sources
        for (auto &r : { "src/.*sse4_1\\.cpp", "src/.*avx\\.cpp", "src/.*avx2\\.cpp", })
            module -= FileRegex(path(mdir), std::regex(r), true);

        module.Private += IncludeDirectory{ mdir + "/src" };
        module.Public += IncludeDirectory{ mdir + "/include" };

        module.Private += "__OPENCV_BUILD"_d;
        module.Private += sw::Shared, "CVAPI_EXPORTS"_d;
        module.Public.add(Definition{ "HAVE_OPENCV_" + boost::to_upper_copy(m) });

        cl2cpp(module);

        return module;
    };

    auto add_dispatch_file = [](auto &m, const std::string &fn)
    {
        m.writeFileOnce(fn + ".simd_declarations.hpp", R"(
#define CV_CPU_SIMD_FILENAME ")" + fn + R"(.simd.hpp"
#define CV_CPU_DISPATCH_MODE SSE2
#include "opencv2/core/private/cv_cpu_include_simd_declarations.hpp"

#define CV_CPU_DISPATCH_MODES_ALL SSE2, BASELINE
)", true);
    };

    //
    auto &core = add_module(opencv, "core");

    core += "cmake/templates/.*\\.in"_rr;
    core.Public += "pub.cppan2.demo.madler.zlib-1"_dep;

    core.Variables["OPENCV_CPU_BASELINE_DEFINITIONS_CONFIGMAKE"] = R"(
#define CV_CPU_COMPILE_SSE 1
#define CV_CPU_BASELINE_COMPILE_SSE 1

#define CV_CPU_COMPILE_SSE2 1
#define CV_CPU_BASELINE_COMPILE_SSE2 1

#define CV_CPU_BASELINE_FEATURES 0 \
    , CV_CPU_SSE \
    , CV_CPU_SSE2 \


)";

    core.Variables["OPENCV_CPU_DISPATCH_DEFINITIONS_CONFIGMAKE"] = R"(
#define CV_CPU_DISPATCH_COMPILE_SSE4_1 1
#define CV_CPU_DISPATCH_COMPILE_SSE4_2 1
#define CV_CPU_DISPATCH_COMPILE_FP16 1
)";

    core.writeFileOnce("precomp.hpp", true);
    core.writeFileOnce("opencv2/opencv_modules.hpp", true);
    core.configureFile("cmake/templates/cvconfig.h.in", "cvconfig.h");
    core.configureFile("cmake/templates/cv_cpu_config.h.in", "cv_cpu_config.h");
    core.configureFile("cmake/templates/custom_hal.hpp.in", "custom_hal.hpp");

    core.writeFileOnce("version_string.inc", "\"OpenCV: " + opencv.getPackage().target_name + "\\nPowered by Software Network\"", true);

    add_dispatch_file(core, "mathfuncs_core");
    add_dispatch_file(core, "stat");

    //
    auto &flann = add_module(opencv, "flann");
    flann.Public += core;

    auto &ml = add_module(opencv, "ml");
    ml.Public += core;

    auto &imgproc = add_module(opencv, "imgproc");
    imgproc.Public += core;
    add_dispatch_file(imgproc, "accum");

    auto &dnn = add_module(opencv, "dnn");
    add_dispatch_file(dnn, "layers/layers_common");
    dnn.Public += imgproc;
    dnn -= "modules/dnn/src/torch/.*"_rr;
    dnn += "modules/dnn/src/ocl4dnn/include"_id;
    dnn += "HAVE_PROTOBUF"_d;
    dnn += "pub.cppan2.demo.google.protobuf.protobuf-3"_dep;
    auto dnn_protoc = dnn + "pub.cppan2.demo.google.protobuf.protoc-3"_dep;
    dnn_protoc->Dummy = true;
    {
        auto protos = enumerate_files_like(dnn.SourceDir / "modules/dnn/src/caffe", ".*\\.proto", false);
        auto protos2 = enumerate_files_like(dnn.SourceDir / "modules/dnn/src/tensorflow", ".*\\.proto", false);
        protos.insert(protos2.begin(), protos2.end());

        auto c = std::make_shared<Command>();
        c->dependency = dnn_protoc;
        c->args.push_back("-I=" + (dnn.SourceDir / "modules/dnn/src/caffe").string());
        c->args.push_back("-I=" + (dnn.SourceDir / "modules/dnn/src/tensorflow").string());
        c->args.push_back("--cpp_out=" + dnn.BinaryDir.string());
        for (auto &f : protos)
        {
            c->args.push_back(f.string());
            c->addInput(f);
            auto n = f.filename().stem().string();
            c->addOutput(dnn.BinaryDir / (n + ".pb.h"));
            c->addOutput(dnn.BinaryDir / (n + ".pb.cc"));
            dnn += dnn.BinaryDir / (n + ".pb.h");
            dnn += dnn.BinaryDir / (n + ".pb.cc");
        }
    }

    auto &video = add_module(opencv, "video");
    video.Public += imgproc;

    auto &photo = add_module(opencv, "photo");
    photo.Public += imgproc;

    auto &shape = add_module(opencv, "shape");
    shape.Public += video;

    auto &features2d = add_module(opencv, "features2d");
    features2d.Public += flann, imgproc;

    auto &calib3d = add_module(opencv, "calib3d");
    calib3d.Public += features2d;

    auto &imgcodecs = add_module(opencv, "imgcodecs");
    imgcodecs.Public += imgproc;
    imgcodecs.Public += "HAVE_GDCM"_d;
    imgcodecs.Public += "HAVE_JASPER"_d;
    imgcodecs.Public += "HAVE_JPEG"_d;
    imgcodecs.Public += "HAVE_OPENCV_IMGCODECS"_d;
    imgcodecs.Public += "HAVE_OPENEXR"_d;
    imgcodecs.Public += "HAVE_PNG"_d;
    imgcodecs.Public += "HAVE_TIFF"_d;
    imgcodecs.Public += "HAVE_WEBP"_d;
    imgcodecs.Public += "pub.cppan2.demo.mdadams.jasper-2"_dep;
    imgcodecs.Public += "pub.cppan2.demo.jpeg-9"_dep;
    imgcodecs.Public += "pub.cppan2.demo.malaterre.gdcm.gdcm-2"_dep;
    imgcodecs.Public += "pub.cppan2.demo.openexr.ilmimf-2"_dep;
    imgcodecs.Public += "pub.cppan2.demo.tiff-4"_dep;
    imgcodecs.Public += "pub.cppan2.demo.png-1"_dep;
    imgcodecs.Public += "pub.cppan2.demo.webp-0"_dep;

    auto &highgui = add_module(opencv, "highgui");
    highgui.Public += imgcodecs;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        highgui += "HAVE_WIN32UI"_def;
        highgui.Public += "gdi32.lib"_lib, "Comdlg32.lib"_lib;
        highgui -= "modules/highgui/src/window_winrt.*"_rr;
        highgui -= "modules/highgui/src/window_carbon.cpp";
        highgui.replaceInFileOnce("modules/highgui/src/window_w32.cpp", "#include <commctrl.h>", "#include <Commdlg.h>\n#include <commctrl.h>");
    }

    auto &videoio = add_module(opencv, "videoio");
    videoio.Public += imgcodecs;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        videoio += "HAVE_DSHOW"_d, "HAVE_MSMF"_d, "HAVE_VFW"_d, "Vfw32.lib"_lib;
        videoio -= "modules/videoio/src/cap_winrt.*"_rr;
        videoio -= "modules/videoio/src/cap_giganetix.cpp";
        videoio -= "modules/videoio/src/cap_gstreamer.cpp";
        videoio -= "modules/videoio/src/cap_mfx.*"_rr;
        videoio -= "modules/videoio/src/cap_qt.cpp";
        videoio -= "modules/videoio/src/cap_unicap.cpp";
        videoio -= "modules/videoio/src/cap_ximea.cpp";
        videoio -= "modules/videoio/src/cap_xine.cpp";
    }

    auto &objdetect = add_module(opencv, "objdetect");
    objdetect.Public += videoio, ml;

    auto &superres = add_module(opencv, "superres");
    superres.Public += video, videoio;

    auto &videostab = add_module(opencv, "videostab");
    videostab.Public += video, photo, calib3d;

    auto &contrib = opencv.addProject("contrib");
    contrib.Source = Git("https://github.com/opencv/opencv_contrib", "{v}");

    auto &xfeatures2d = add_module(contrib, "xfeatures2d");
    {
        xfeatures2d.Public += features2d;

        static const Strings boostdesc{
            "boostdesc_bgm.i",
            "boostdesc_bgm_bi.i",
            "boostdesc_bgm_hd.i",
            "boostdesc_binboost_064.i",
            "boostdesc_binboost_128.i",
            "boostdesc_binboost_256.i",
            "boostdesc_lbgm.i",
        };

        static const Strings vgg{
            "vgg_generated_48.i",
            "vgg_generated_64.i",
            "vgg_generated_80.i",
            "vgg_generated_120.i",
        };

        auto c = std::make_shared<ExecuteCommand>([&xfeatures2d]()
        {
            const auto base = "https://raw.githubusercontent.com/opencv/opencv_3rdparty/34e4206aef44d50e6bbcd0ab06354b52e7466d26/"s;
            for (auto &f : boostdesc)
            {
                download_file(base + f, xfeatures2d.BinaryDir / f, 10_MB);
            }

            const auto base2 = "https://raw.githubusercontent.com/opencv/opencv_3rdparty/fccf7cd6a4b12079f73bbfb21745f9babcd4eb1d/"s;
            for (auto &f : vgg)
            {
                download_file(base2 + f, xfeatures2d.BinaryDir / f, 10_MB);
            }
        });

        for (auto &f : boostdesc)
        {
            auto p = xfeatures2d.BinaryDir / f;
            c->addOutput(p);
            xfeatures2d += p;
        }

        for (auto &f : vgg)
        {
            auto p = xfeatures2d.BinaryDir / f;
            c->addOutput(p);
            xfeatures2d += p;
        }
    }

    auto &stitching = add_module(opencv, "stitching");
    stitching.Public += xfeatures2d, calib3d, objdetect;

    auto &text = add_module(contrib, "text");
    text.Public += "HAVE_TESSERACT"_d;
    text.Public += dnn, imgproc, ml, "pub.cppan2.demo.google.tesseract.libtesseract-master"_dep;
    text.writeFileOnce("text_config.hpp", true);
    text.replaceInFileOnce("modules/text/src/precomp.hpp", "#include <tesseract/baseapi.h>", "#include <baseapi.h>");
    text.replaceInFileOnce("modules/text/src/precomp.hpp", "#include <tesseract/resultiterator.h>", "#include <resultiterator.h>");

    auto &aruco = add_module(contrib, "aruco");
    aruco.Public += calib3d;

    auto &bgsegm = add_module(contrib, "bgsegm");
    bgsegm.Public += video;

    auto &bioinspired = add_module(contrib, "bioinspired");
    bioinspired.Public += core;

    auto &ccalib = add_module(contrib, "ccalib");
    ccalib.Public += calib3d, highgui;

    auto &datasets = add_module(contrib, "datasets");
    datasets.Public += videoio, highgui;

    auto &dpm = add_module(contrib, "dpm");
    dpm.Public += imgproc;

    auto &face = add_module(contrib, "face");
    face.Public += imgproc;

    auto &freetype = add_module(contrib, "freetype");
    freetype.Public += imgproc, "pub.cppan2.demo.behdad.harfbuzz-1"_dep;

    auto &fuzzy = add_module(contrib, "fuzzy");
    fuzzy.Public += imgproc;

    auto &hdf = add_module(contrib, "hdf");
    hdf.Public += imgproc, "pub.cppan2.demo.hdfgroup.hdf5.hdf5-1"_dep;

    auto &line_descriptor = add_module(contrib, "line_descriptor");
    line_descriptor.Public += features2d, highgui, videoio;

    auto &phase_unwrapping = add_module(contrib, "phase_unwrapping");
    phase_unwrapping.Public += imgproc;

    auto &plot = add_module(contrib, "plot");
    plot.Public += imgproc;

    auto &reg = add_module(contrib, "reg");
    reg.Public += imgproc;

    auto &rgbd = add_module(contrib, "rgbd");
    rgbd.Public += calib3d;

    auto &saliency = add_module(contrib, "saliency");
    saliency.Public += highgui, videoio;
    if (s.Settings.TargetOS.Type == OSType::Windows)
        saliency += "WIN32"_def;

    auto &stereo = add_module(contrib, "stereo");
    stereo.Public += calib3d, highgui, videoio;

    auto &structured_light = add_module(contrib, "structured_light");
    structured_light.Public += phase_unwrapping, calib3d;

    auto &surface_matching = add_module(contrib, "surface_matching");
    surface_matching.Public += flann;

    auto &tracking = add_module(contrib, "tracking");
    tracking.Public += datasets, dnn, highgui, videoio, video;

    auto &ximgproc = add_module(contrib, "ximgproc");
    ximgproc.Public += calib3d, highgui, videoio;

    auto &xobjdetect = add_module(contrib, "xobjdetect");
    xobjdetect.Public += objdetect, highgui;

    auto &xphoto = add_module(contrib, "xphoto");
    xphoto.Public += imgproc;

    auto &optflow = add_module(contrib, "optflow");
    optflow.Public += video, ximgproc;
}
