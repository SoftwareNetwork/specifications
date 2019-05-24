void build(Solution &s)
{
    auto &websocketpp = s.addTarget<LibraryTarget>("zaphoyd.websocketpp", "0.8.1");
    websocketpp += Git("https://github.com/zaphoyd/websocketpp");
    websocketpp += "websocketpp/.*\\.hpp"_rr;
}
