void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("rigtorp.SPSCQueue", "master");
    t += Git("https://github.com/rigtorp/SPSCQueue");
    t += "include/.*"_rr;
}
