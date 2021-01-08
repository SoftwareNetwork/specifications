void build(Solution &s)
{
    auto &t = s.addLibrary("Samsung.rlottie", "0.2.0");
    t += Git("https://github.com/Samsung/rlottie", "v{M}.{m}");

    t += "inc/.*"_rr;
    t += "src/lottie/.*"_r;
    t += "src/vector/.*"_r;
    t += "src/vector/freetype/.*"_r;
    t += "src/vector/stb/.*"_r;
    t += "vs2019/config.h";

    t.Public += "inc"_idir;
    t += "src"_idir;
    t += "src/vector"_idir;
    t += "src/vector/freetype"_idir;
    t += "vs2019"_idir;

    t += "RLOTTIE_BUILD"_def;
}
