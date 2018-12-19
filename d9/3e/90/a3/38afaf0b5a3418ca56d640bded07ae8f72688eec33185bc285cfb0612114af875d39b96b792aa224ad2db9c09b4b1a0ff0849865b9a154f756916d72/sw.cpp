void build(Solution &s)
{
    auto &cpp_linenoise = s.addTarget<LibraryTarget>("yhirose.cpp_linenoise", "master");
    cpp_linenoise += Git("https://github.com/yhirose/cpp-linenoise", "", "{v}");
}
