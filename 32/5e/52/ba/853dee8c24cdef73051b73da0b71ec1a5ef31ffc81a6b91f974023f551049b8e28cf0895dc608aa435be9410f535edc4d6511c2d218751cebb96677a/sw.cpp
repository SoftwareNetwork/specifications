void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("zeroxs.aegis", "2.7.0");
    t += Git("https://github.com/zeroxs/aegis.cpp");

    t.AutoDetectOptions = false;
    t.Public += "AEGIS_HEADER_ONLY"_def;
    t -= "include/.*"_rr;
    t.Public += "include"_idir;

    t.Public += "org.sw.demo.nlohmann.json"_dep;
    t.Public += "org.sw.demo.gabime.spdlog"_dep;
    t.Public += "org.sw.demo.chriskohlhoff.asio"_dep;
    t.Public += "org.sw.demo.zaphoyd.websocketpp"_dep;
}
