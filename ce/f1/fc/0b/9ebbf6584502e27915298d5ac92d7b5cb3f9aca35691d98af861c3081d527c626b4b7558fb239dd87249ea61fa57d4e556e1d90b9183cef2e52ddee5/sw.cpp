void build(Solution &s)
{
    auto &p = s.addLibrary("darrengarvey.cgi", "master");
    p += Git("https://github.com/darrengarvey/cgi", "", "{v}");
    p += "boost/.*"_rr;
}
