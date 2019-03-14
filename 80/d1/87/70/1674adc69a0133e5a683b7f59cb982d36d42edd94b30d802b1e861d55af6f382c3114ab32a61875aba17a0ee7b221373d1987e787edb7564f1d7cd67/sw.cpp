void build(Solution &s)
{
    auto &t = s.addTarget<Library>("kyz.libmspack", "1.9.1");
    t += Git("https://github.com/kyz/libmspack", "v{v}");

    t.ExportAllSymbols = true;
    t += "libmspack/mspack/.*"_rr;
    t -= "libmspack/mspack/debug.c";
    t -= "libmspack/mspack/mspack.def";
    t.Public += "libmspack"_id;
    t.Public += "libmspack/mspack"_id;
}
