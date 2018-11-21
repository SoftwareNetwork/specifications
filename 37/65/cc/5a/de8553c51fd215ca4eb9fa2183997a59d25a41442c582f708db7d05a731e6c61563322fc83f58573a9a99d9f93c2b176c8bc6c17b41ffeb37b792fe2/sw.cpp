void build(Solution &s)
{
    auto &tgbot = s.addTarget<StaticLibraryTarget>("reo7sp.tgbot", "master");
    tgbot += Git("https://github.com/reo7sp/tgbot-cpp", "", "master");

    tgbot.Public += "HAVE_CURL"_d;

    tgbot.Public += "org.sw.demo.boost.property_tree-1"_dep;
    tgbot.Public += "org.sw.demo.openssl.ssl-1"_dep;
    tgbot.Public += "org.sw.demo.boost.system-1"_dep;
    tgbot.Public += "org.sw.demo.boost.date_time-1"_dep;
    tgbot.Public += "org.sw.demo.badger.curl.libcurl-7"_dep;
    tgbot.Public += "org.sw.demo.boost.asio-1"_dep;
}