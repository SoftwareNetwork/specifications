void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("davisking.dlib", "19.17.0");
    t += Git("https://github.com/davisking/dlib", "v{M}.{m}");
    t += "dlib/.*"_rr;
}
