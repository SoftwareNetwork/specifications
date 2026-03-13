void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("hosseinmoein.DataFrame", "4.0.0");
    t += Git("https://github.com/hosseinmoein/DataFrame");
    t += "include/.*"_rr;
}
