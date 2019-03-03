void build(Solution &s)
{
    auto &zxing_cpp = s.addTarget<StaticLibraryTarget>("glassechidna.zxing_cpp", "master");
    zxing_cpp += Git("https://github.com/glassechidna/zxing-cpp", "", "master");

    zxing_cpp +=
        "core/src/zxing/.*"_rr;

    zxing_cpp.Public +=
        "core/src"_id;

    zxing_cpp.Public += "org.sw.demo.gnu.iconv.libiconv-1"_dep;
    zxing_cpp.Public += "org.sw.demo.mattmccutchen.bigint-master"_dep;

    zxing_cpp.patch("core/src/zxing/aztec/decoder/Decoder.cpp", "char* ss = s;", "const char* ss = s;");
}
