void build(Solution &s)
{
    auto &t = s.addLibrary("piscisaureus.wepoll", "1.5.8");
    t += Git("https://github.com/piscisaureus/wepoll");
    t += "WEPOLL_EXPORT"_api;
    t += "wepoll\\.[hc]"_r;
    t += "ws2_32.lib"_slib;
}
