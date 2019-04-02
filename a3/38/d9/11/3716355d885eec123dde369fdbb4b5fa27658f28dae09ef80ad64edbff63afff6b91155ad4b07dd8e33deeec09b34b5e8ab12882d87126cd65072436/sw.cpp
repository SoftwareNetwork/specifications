void build(Solution &s)
{
    auto &p = s.addTarget<LibraryTarget>("CathalT.Cerializer", "master");
    p += Git("https://github.com/CathalT/Cerializer");
    p += "Cerializer/.*"_rr;
}
