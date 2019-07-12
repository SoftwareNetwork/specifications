void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("yhirose.cpp_peglib", "master");
    t += Git("https://github.com/yhirose/cpp-peglib");
    t += "peglib.h";
}
