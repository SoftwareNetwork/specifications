void build(Solution &s)
{
    auto &t = s.addStaticLibrary("PixarAnimationStudios.OpenSubdiv", "3.6.1");
    t += Git("https://github.com/PixarAnimationStudios/OpenSubdiv", "v{M}_{m}_{p}");

    t += "opensubdiv/.*"_rr;

    t -= "opensubdiv/osd/.*"_rr;
    t += "opensubdiv/osd/cpu.*"_rr;

    if (t.getCompilerType() == CompilerType::MSVC)
        t += "_USE_MATH_DEFINES"_def;

    //t += "glLoader/.*"_rr;
    //t.Public += "."_idir;
    //t += "glLoader"_idir;
    t -= "opensubdiv/osd/cpuGLVertexBuffer.cpp";
}
