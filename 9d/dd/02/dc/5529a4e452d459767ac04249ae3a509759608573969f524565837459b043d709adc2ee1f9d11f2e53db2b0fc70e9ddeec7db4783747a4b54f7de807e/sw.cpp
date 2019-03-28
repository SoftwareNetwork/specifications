void build(Solution &s)
{
    auto &p = s.addStaticLibrary("lemire.FastPFor", "master");
    p += Git("https://github.com/lemire/FastPFor");

    p += "include/.*"_rr;

    p +=
        "src/bitpacking.cpp",
        "src/bitpacking.cpp",
        "src/bitpackingaligned.cpp",
        "src/bitpackingunaligned.cpp",
        "src/horizontalbitpacking.cpp",
        "src/simdunalignedbitpacking.cpp",
        "src/simdbitpacking.cpp",
        "src/varintdecode.c",
        "src/streamvbyte.c"
        ;
}
