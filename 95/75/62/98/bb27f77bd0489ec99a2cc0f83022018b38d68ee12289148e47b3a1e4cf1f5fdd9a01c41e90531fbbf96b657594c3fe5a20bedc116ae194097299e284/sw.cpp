void build(Build &s)
{
    auto &t = s.addStaticLibrary("ReneNyffenegger.cpp_base64", "master");
    t += Git("https://github.com/ReneNyffenegger/cpp-base64");
    t += "base64\\..*"_r;
}
