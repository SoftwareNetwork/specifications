void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("johnmcfarlane.cnl", "master");
    t += Git("https://github.com/johnmcfarlane/cnl");
    t += "include/.*"_rr;
}
