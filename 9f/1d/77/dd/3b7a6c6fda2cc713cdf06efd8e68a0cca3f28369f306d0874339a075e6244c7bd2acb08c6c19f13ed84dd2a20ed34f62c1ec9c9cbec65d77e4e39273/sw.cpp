void build(Solution &s)
{
    auto &t = s.addLibrary("bellard.libbf", "2020.01.19");
    t += RemoteFile("https://bellard.org/libbf/libbf-{M}-{m:02}-{p:02}.tar.gz");
    t -= "bftest.c";
    t -= "bfbench.c";
    t -= "tinypi.c";
}
