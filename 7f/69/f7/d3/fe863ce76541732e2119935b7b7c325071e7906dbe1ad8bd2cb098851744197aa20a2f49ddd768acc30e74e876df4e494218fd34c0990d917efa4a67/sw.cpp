void build(Solution &s)
{
    auto &p = s.addLibrary("slowriot.cgi", "master");
    p += Git("https://github.com/slowriot/cgi", "", "{v}");
    p += "boost/.*"_rr;
}
