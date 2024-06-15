void build(Solution &s)
{
    auto &t = s.addLibrary("BLAKE3_team.blake3", "1.5.1");
    t += Git("https://github.com/BLAKE3-team/BLAKE3");

    t.setRootDirectory("c");
    t += "BLAKE3_API"_api;
    t -= ".*"_r;
    t += "blake3.c";
    t += "blake3_dispatch.c";
    t += "blake3_portable.c";

    t += "BLAKE3_NO_SSE2"_def;
    t += "BLAKE3_NO_SSE41"_def;
    t += "BLAKE3_NO_AVX2"_def;
    t += "BLAKE3_NO_AVX512"_def;

    //t -= "blake3_neon.c";
    t += "BLAKE3_USE_NEON=0"_def;
}
