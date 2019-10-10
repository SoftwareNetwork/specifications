void build(Solution &s)
{
    auto &p = s.addProject("freeimage", "3.18.0");
    p += RemoteFile("https://downloads.sourceforge.net/freeimage/FreeImage{M}{m}{p}.zip");

    auto &t = p.addLibrary("freeimage");
    t += "Source/.*"_r;
    t += "Source/FreeImage.*"_rr;
    t += "Source/Metadata/.*"_rr;

    t += "org.sw.demo.openexr.IlmImf"_dep;
    t += "org.sw.demo.glennrp.png"_dep;
    t += "org.sw.demo.tiff"_dep;
    t += "org.sw.demo.uclouvain.openjpeg.openjp2"_dep;
    t += "org.sw.demo.webmproject.webp"_dep;
    t += "org.sw.demo.jpeg"_dep;
    t += "org.sw.demo.4creators.jxrlib"_dep;
    t += "org.sw.demo.libraw"_dep;

    t.patch("Source/FreeImage/PluginPNG.cpp", "../LibPNG/png.h", "png.h");
    t.patch("Source/FreeImage/PluginPNG.cpp", "../ZLib/zlib.h", "zlib.h");

    t.patch("Source/FreeImage/ZLibInterface.cpp", "../ZLib/zlib.h", "zlib.h");
    t.patch("Source/FreeImage/ZLibInterface.cpp", "../ZLib/zutil.h", "zutil.h");

    t.patch("Source/FreeImage/PluginG3.cpp", "../LibTIFF4/tiffiop.h", "tiffiop.h");
    t.patch("Source/Metadata/XTIFF.cpp", "../LibTIFF4/tiffiop.h", "tiffiop.h");
    t.patch("Source/FreeImage/PluginTIFF.cpp", "../LibTIFF4/tiffiop.h", "tiffiop.h");

    t.patch("Source/FreeImage/J2KHelper.cpp", "../LibOpenJPEG/openjpeg.h", "openjpeg.h");
    t.patch("Source/FreeImage/PluginJP2.cpp", "../LibOpenJPEG/openjpeg.h", "openjpeg.h");
    t.patch("Source/FreeImage/PluginJ2K.cpp", "../LibOpenJPEG/openjpeg.h", "openjpeg.h");

    t.patch("Source/FreeImage/PluginWebP.cpp", "../LibWebP/src/webp/decode.h", "webp/decode.h");
    t.patch("Source/FreeImage/PluginWebP.cpp", "../LibWebP/src/webp/encode.h", "webp/encode.h");
    t.patch("Source/FreeImage/PluginWebP.cpp", "../LibWebP/src/webp/mux.h", "webp/mux.h");

    for (auto s : { "../OpenEXR/IlmImf/","../OpenEXR/Iex/","../OpenEXR/Half/", })
    {
        t.patch("Source/FreeImage/PluginEXR.cpp", s, "");
        t.patch("Source/FreeImage/PluginTIFF.cpp", s, "");
    }

    for (auto s : { "Source/FreeImageToolkit/JPEGTransform.cpp","Source/FreeImage/PluginJPEG.cpp" })
    {
        t.patch(s, "../LibJPEG/jinclude.h", "jinclude.h");
        t.patch(s, "../LibJPEG/jpeglib.h", "jpeglib.h");
        t.patch(s, "../LibJPEG/jerror.h", "jerror.h");
        t.patch(s, "../LibJPEG/transupp.h", "transupp.h");
    }

    t.patch("Source/FreeImage/PluginJXR.cpp", "../LibJXR/jxrgluelib/JXRGlue.h", "jxrgluelib/JXRGlue.h");

    t.patch("Source/FreeImage/PluginRAW.cpp", "../LibRawLite/libraw/libraw.h", "libraw/libraw.h");
}
