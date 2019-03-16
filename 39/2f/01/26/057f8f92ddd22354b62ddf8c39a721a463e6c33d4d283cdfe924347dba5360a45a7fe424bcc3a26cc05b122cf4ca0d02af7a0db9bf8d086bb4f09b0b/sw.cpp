void build(Solution &s)
{
    auto &p = s.addProject("openalpr", "master");
    p += Git("https://github.com/openalpr/openalpr");

    auto &openalpr = p.addTarget<StaticLibraryTarget>("openalpr");
    {
        openalpr += "src/openalpr/.*"_rr;
        openalpr -= "src/openalpr/support/re2/.*"_rr;
        openalpr.Public += "src/openalpr"_id, "src/openalpr/support"_id;

        openalpr.Public += "INSTALL_PREFIX=\"\""_d;
        openalpr.Public += "OPENALPR_MAJOR_VERSION=2"_d;
        openalpr.Public += "OPENALPR_MINOR_VERSION=3"_d;
        openalpr.Public += "OPENALPR_PATCH_VERSION=0"_d;

        openalpr.Public += "org.sw.demo.google.tesseract.libtesseract-master"_dep;
    }

    auto &statedetection = p.addTarget<StaticLibraryTarget>("statedetection");
    {
        statedetection += "src/statedetection/.*"_rr;

        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            statedetection.Public += "NOMINMAX"_d;
            statedetection.Public += "WINDOWS"_d;
        }

        statedetection.Public += "org.sw.demo.intel.opencv.video"_dep;
        (statedetection.Public + openalpr)->IncludeDirectoriesOnly = true;
        statedetection.Public += "org.sw.demo.intel.opencv.calib3d"_dep;
        statedetection.Public += "org.sw.demo.intel.opencv.highgui"_dep;
        statedetection.Public += "org.sw.demo.intel.opencv.objdetect"_dep;
    }

    openalpr.Public += statedetection;

    auto &video = p.addTarget<StaticLibraryTarget>("video");
    {
        video += "src/video/.*"_rr;

        video.Public += "src"_id;

        if (s.Settings.TargetOS.Type == OSType::Windows)
        {
            video.Public += "NOMINMAX"_d;
            video.Public += "WINDOWS"_d;
        }

        video.Public += "org.sw.demo.intel.opencv.videoio"_dep;
        video.Public += "org.sw.demo.intel.opencv.highgui"_dep;
        video.Public += openalpr;
    }

    auto &alpr = p.addTarget<ExecutableTarget>("alpr");
    {
        alpr += "src/main.cpp";
        alpr.Public += "src"_id;
        alpr.Public += "HAVE_SSTREAM"_d;
        alpr.Public += video;
    }
}
