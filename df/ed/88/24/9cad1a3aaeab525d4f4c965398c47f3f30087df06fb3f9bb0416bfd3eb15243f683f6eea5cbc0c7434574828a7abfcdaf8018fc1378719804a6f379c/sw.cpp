void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("scylladb.seastar", "master");
    t += Git("https://github.com/scylladb/seastar");
}
