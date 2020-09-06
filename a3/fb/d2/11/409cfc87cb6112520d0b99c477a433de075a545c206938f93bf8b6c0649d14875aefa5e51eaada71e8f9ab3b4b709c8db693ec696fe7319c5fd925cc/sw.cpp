void build(Solution &s)
{
    auto &t = s.addStaticLibrary("olvb.nodesoup", "master");
    t += Git("https://github.com/olvb/nodesoup");

    t += cpp14;
    t += "_USE_MATH_DEFINES"_def; // msvc
}
