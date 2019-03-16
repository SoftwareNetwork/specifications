void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibrary>("gfx.cpp_TimSort", "master");
    t += Git("https://github.com/gfx/cpp-TimSort");
}
