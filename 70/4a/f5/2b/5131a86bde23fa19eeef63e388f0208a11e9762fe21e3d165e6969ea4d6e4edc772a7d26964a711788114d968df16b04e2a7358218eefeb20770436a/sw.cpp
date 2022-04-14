void build(Solution &s)
{
    auto &t = s.addLibrary("ReactiveX.RxCpp", "4.1.1");
    t += Git("https://github.com/ReactiveX/RxCpp", "v{v}");
    t += "Rx/v2/src/.*"_rr;
    t.Public += "Rx/v2/src"_idir;
}
