void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &cpp_linenoise = s.addTarget<LibraryTarget>("yhirose.cpp_linenoise", "master");
    cpp_linenoise.Source = Git("https://github.com/yhirose/cpp-linenoise", "", "{b}");
}