void build(Solution &s)
{
    auto &t = s.addStaticLibrary("BLAKE3_team.BLAKE3", "1.4.1");
    t += Git("https://github.com/BLAKE3-team/BLAKE3");

    t.setRootDirectory("c");
    t += "blake3.*"_r;
    t -= "blake3_neon.c";
}
