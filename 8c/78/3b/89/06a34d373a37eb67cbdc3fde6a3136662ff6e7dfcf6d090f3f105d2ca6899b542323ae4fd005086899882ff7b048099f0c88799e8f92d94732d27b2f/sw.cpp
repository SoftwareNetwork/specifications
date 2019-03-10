void build(Solution &s)
{
    auto &p = s.addStaticLibrary("slowriot.libtelegram", "master");
    p += Git("https://github.com/slowriot/libtelegram", "", "{v}");

    p += "include/libtelegram/.*"_rr;
    p += "include/httplib.h";

    p += "org.sw.demo.chriskohlhoff.urdl-master"_dep;
    p += "org.sw.demo.slowriot.cgi-master"_dep;
    p += "org.sw.demo.boost.asio"_dep;
    p += "org.sw.demo.nlohmann.json"_dep;
}
