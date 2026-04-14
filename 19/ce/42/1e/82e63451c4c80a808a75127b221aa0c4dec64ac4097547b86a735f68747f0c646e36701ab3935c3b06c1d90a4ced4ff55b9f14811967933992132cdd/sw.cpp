void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("stephenberry.glaze", "7.3.3");
    t += Git("https://github.com/stephenberry/glaze", "v{v}");
    t += "include/.*"_rr;
}
