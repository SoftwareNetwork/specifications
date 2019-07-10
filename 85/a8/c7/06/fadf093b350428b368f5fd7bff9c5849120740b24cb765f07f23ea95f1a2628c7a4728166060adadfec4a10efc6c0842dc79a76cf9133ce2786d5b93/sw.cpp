void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("ingowald.pbrt_parser", "master");
    t += Git("https://github.com/ingowald/pbrt-parser");
    t.setRootDirectory("pbrtParser");
    t += ".*"_rr;
}
