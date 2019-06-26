void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("max0x7ba.atomic_queue", "master");
    t += Git("https://github.com/max0x7ba/atomic_queue");
    t += ".*\\.h"_rr;
}
