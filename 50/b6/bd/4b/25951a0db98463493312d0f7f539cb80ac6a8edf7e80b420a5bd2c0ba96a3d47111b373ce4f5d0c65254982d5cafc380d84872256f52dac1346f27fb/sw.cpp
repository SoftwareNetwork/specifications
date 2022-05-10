void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("zaphoyd.websocketpp", "0.8.2");
    t += Git("https://github.com/zaphoyd/websocketpp");
    t += "websocketpp/.*\\.hpp"_rr;
    t.patch("websocketpp/endpoint.hpp", "~endpoint<connection,config>", "~endpoint");
    t.patch("websocketpp/logger/basic.hpp", "<concurrency,names>", "");
}
