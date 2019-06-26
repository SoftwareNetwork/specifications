void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("luncliff.coroutine", "master");
    t += Git("https://github.com/luncliff/coroutine");
    t += "interface/.*"_rr;
    t += "modules/.*\\.[hc]p?p?"_rr;
}
