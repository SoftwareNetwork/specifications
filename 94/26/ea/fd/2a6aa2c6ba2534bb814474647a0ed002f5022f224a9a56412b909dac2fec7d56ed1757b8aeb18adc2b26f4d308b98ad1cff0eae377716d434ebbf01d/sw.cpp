void build(Solution &s)
{
    auto &t = s.addTarget<Library>("efficient.libcuckoo", "master");
    t += Git("https://github.com/efficient/libcuckoo");
    t += "libcuckoo/.*\\.hh"_rr;
}
