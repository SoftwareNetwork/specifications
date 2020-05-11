void build(Solution &s)
{
    auto &cpp_httplib = s.addTarget<LibraryTarget>("yhirose.cpp_httplib", "0.5.13");
    cpp_httplib += Git("https://github.com/yhirose/cpp-httplib", "v{v}");
    cpp_httplib += "httplib.h";
}
