void build(Solution &s)
{
    auto &cpp_httplib = s.addTarget<LibraryTarget>("yhirose.cpp_httplib", "0.6.7");
    cpp_httplib += Git("https://github.com/yhirose/cpp-httplib", "v{v}");
    cpp_httplib += "httplib.h";
}
