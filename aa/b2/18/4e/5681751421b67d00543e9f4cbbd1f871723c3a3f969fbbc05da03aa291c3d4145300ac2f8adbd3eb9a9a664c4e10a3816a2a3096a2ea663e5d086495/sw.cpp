void build(Solution &s)
{
    auto &t = s.addLibrary("facebook.experimental.libunifex", "master");
    t += Git("https://github.com/facebookexperimental/libunifex");
    t += cpplatest;
}
