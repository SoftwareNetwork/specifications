void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("greg7mdp.parallel_hashmap", "master");
    t += Git("https://github.com/greg7mdp/parallel-hashmap");
    t += "parallel_hashmap/.*"_rr;
}
