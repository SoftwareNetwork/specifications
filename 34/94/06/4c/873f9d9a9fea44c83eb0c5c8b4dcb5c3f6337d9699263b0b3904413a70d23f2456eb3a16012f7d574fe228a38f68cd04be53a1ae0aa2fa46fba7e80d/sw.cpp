void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("cinemast.libjson_rpc_cpp", "1.2.0");
    t += Git("https://github.com/cinemast/libjson-rpc-cpp", "v{v}");
}
