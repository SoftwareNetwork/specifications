void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("stephenberry.glaze", "7.0.2");
    t += Git("https://github.com/stephenberry/glaze", "v{v}");
    t += "include/.*"_rr;
}
