void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("lava.matplotlib_cpp", "master");
    t += Git("https://github.com/lava/matplotlib-cpp");
}
