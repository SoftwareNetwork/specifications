void build(Solution &s)
{
    auto &p = s.addLibrary("arun11299.cpp_subprocess", "1.0.4");
    p += Git("https://github.com/arun11299/cpp-subprocess", "v{v}");
}
