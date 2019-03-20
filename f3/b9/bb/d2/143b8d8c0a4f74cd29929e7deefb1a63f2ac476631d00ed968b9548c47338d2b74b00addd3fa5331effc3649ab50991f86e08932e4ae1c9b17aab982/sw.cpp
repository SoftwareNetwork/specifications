void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("hsutter.gcpp", "master");
    t += Git("https://github.com/hsutter/gcpp");
    t += ".*\\.h"_rr;
}
