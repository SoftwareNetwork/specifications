void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("uNetworking.uSockets", "0.7.1");
    t += Git("https://github.com/uNetworking/uSockets", "v{v}");

    t += cpp17;

    t += "LIBUS_USE_LIBUV"_def;
    t += "org.sw.demo.libuv"_dep;

    t += "LIBUS_USE_OPENSSL"_def;
    t += "org.sw.demo.openssl.ssl"_dep;
}
