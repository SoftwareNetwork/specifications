void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("tlx.tlx", "master");
    t += Git("https://github.com/tlx/tlx");
    t += "tlx/.*"_rr;
}
