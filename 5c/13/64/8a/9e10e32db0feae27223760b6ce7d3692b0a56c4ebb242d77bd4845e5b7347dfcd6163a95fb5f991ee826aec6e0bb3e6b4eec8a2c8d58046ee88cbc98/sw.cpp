void build(Solution &s)
{
    auto &t = s.addLibrary("sharkdp.dbg_macro", "1.3.1");
    t += Git("https://github.com/adishavit/argh", "v{v}");
}
