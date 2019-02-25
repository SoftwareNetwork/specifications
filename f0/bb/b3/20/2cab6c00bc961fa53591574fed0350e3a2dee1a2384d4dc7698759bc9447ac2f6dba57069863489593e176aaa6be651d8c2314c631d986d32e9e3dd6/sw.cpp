void build(Solution &s)
{
    auto &t = s.addTarget<Executable>("skarupke.flat_hash_map");
    t += Git("https://github.com/skarupke/flat_hash_map", "", "master");
}
