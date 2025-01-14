void build(Solution &s)
{
    auto &t = s.addTarget<StaticLibraryTarget>("socketio.socket_io_client_cpp", "3.1.0");
    t += Git("https://github.com/socketio/socket.io-client-cpp");

    t.Public += "org.sw.demo.zaphoyd.websocketpp"_dep;
    t.Public += "org.sw.demo.miloyip.rapidjson"_dep;
    t.Public += "org.sw.demo.boost.asio"_dep;
    t.Public += "org.sw.demo.boost.random"_dep;
}
