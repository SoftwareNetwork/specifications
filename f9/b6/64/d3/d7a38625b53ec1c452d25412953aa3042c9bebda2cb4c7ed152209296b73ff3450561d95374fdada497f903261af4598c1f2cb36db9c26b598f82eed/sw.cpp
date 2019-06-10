void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("shaovoon.concurrent_permcomb", "master");
    t += Git("https://github.com/shaovoon/concurrent_permcomb");

    t.setRootDirectory("PermComb");
    t += "permcomb/.*"_rr;
}
