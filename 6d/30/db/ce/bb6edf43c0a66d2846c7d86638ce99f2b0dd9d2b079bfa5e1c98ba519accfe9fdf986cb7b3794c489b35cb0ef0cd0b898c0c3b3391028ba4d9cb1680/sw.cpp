void build(Solution &s)
{
    auto &zxing_cpp = s.addTarget<StaticLibraryTarget>("zxing_cpp.zxing_cpp", "2.3.0");
    zxing_cpp += Git("https://github.com/zxing-cpp/zxing-cpp", "v{v}");

    zxing_cpp += cpp17;
    zxing_cpp += "core/.*"_rr;
    zxing_cpp.Public += "core/src"_id;
}
