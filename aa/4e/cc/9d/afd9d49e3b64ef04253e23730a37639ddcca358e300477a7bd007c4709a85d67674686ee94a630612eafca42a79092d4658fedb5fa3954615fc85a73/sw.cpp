void build(Solution &s)
{
    auto &t = s.addStaticLibrary("yourWaifu.sleepy_discord", "master");
    t += Git("https://github.com/yourWaifu/sleepy-discord");

    t += cpp17;

    t += "include/.*"_rr;
    t += "sleepy_discord/.*"_rr;

    t.Public += "include"_idir;
    t += "include/sleepy_discord"_idir;

    //t += "NONEXISTENT_WEBSOCKETPP"_def;
    t += "NONEXISTENT_UWEBSOCKETS"_def;
    t += "NONEXISTANT_GIT_INFO"_def;

    //t.Public += "org.sw.demo.uNetworking.uWebSockets"_dep;
    t.Public += "org.sw.demo.zaphoyd.websocketpp"_dep;
    t.Public += "org.sw.demo.whoshuu.cpr"_dep;
    t.Public += "org.sw.demo.xiph.opus"_dep;
    t.Public += "org.sw.demo.chriskohlhoff.asio"_dep;
}
