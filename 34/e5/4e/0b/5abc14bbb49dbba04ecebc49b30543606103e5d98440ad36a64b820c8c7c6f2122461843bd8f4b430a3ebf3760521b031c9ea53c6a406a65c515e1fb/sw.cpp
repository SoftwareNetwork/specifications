void build(Solution &s)
{
    auto &t = s.addLibrary("sharkdp.dbg_macro", "master");
    t += Git("https://github.com/sharkdp/dbg-macro");
}
