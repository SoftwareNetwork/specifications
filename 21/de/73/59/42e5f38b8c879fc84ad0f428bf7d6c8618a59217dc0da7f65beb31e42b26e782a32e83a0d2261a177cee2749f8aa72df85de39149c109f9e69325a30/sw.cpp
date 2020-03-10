void build(Solution &s)
{
    auto &t = s.addStaticLibrary("coruus.keccak_tiny", "singlefile");
    t += Git("https://github.com/coruus/keccak-tiny");
    t += "keccak-tiny.h";
    t += "keccak-tiny.c";

    auto &u = t.addStaticLibrary("unrolled");
    u += "keccak-tiny.h";
    u += "keccak-tiny-unrolled.c";
}
