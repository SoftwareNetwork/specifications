void build(Solution &s)
{
    auto &t = s.addLibrary("Samsung.rlottie", "master");
    t += Git("https://github.com/Samsung/rlottie");

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
    t.Public += "org.sw.demo.tencent.rapidjson-master"_dep;

    /*t.writeFileOnce(t.BinaryPrivateDir / "config.h", R"(
    #define LOTTIE_THREAD_SUPPORT 1
    )");*/
}
