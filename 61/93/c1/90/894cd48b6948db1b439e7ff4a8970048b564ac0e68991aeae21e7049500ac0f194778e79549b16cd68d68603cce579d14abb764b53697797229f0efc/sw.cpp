void build(Solution &s)
{
    auto &t = s.addLibrary("odygrd.quill", "11.1.0");
    t += Git("https://github.com/odygrd/quill");
    t += "include/.*"_rr;
}
