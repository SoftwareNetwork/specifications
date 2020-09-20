void build(Solution &s)
{
    auto &p = s.addProject("KhronosGroup", "master");
    p += Git("https://github.com/KhronosGroup/glslang");

    auto &gls = p.addStaticLibrary("glslang");
    gls += "glslang/.*"_rr;
    gls += "OGLCompilersDLL/.*"_rr;
    gls += "SPIRV/.*"_rr;
    gls -= "glslang/OSDependent/Unix/.*"_rr;
    gls -= "glslang/OSDependent/Web/.*"_rr;

    gls.Public += "ENABLE_HLSL"_def;

    gls.Public += "."_idir;
    gls += "OGLCompilersDLL"_idir;

    gls.patch("build_info.h.tmpl", "<major>", std::to_string((int)gls.getPackage().getVersion().getMajor()));
    gls.patch("build_info.h.tmpl", "<minor>", std::to_string((int)gls.getPackage().getVersion().getMinor()));
    gls.patch("build_info.h.tmpl", "<patch>", std::to_string((int)gls.getPackage().getVersion().getPatch()));
    gls.patch("build_info.h.tmpl", "<flavor>", "");
    gls.configureFile("build_info.h.tmpl", "glslang/build_info.h");
}
