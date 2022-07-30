void build(Solution &s)
{
    auto &asio = s.addTarget<LibraryTarget>("chriskohlhoff.asio", "1.22.2");
    asio += Git("https://github.com/chriskohlhoff/asio", "asio-{M}-{m}-{p}");

    asio.setRootDirectory("asio");
    asio += "include/.*\\.[hi]pp"_rr;
    asio.Public += "ASIO_STANDALONE"_d;

    if (asio.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        asio.Public += "_WIN32_WINNT=0x0601"_def;
        asio += "Mswsock.lib"_slib;
        asio += "Ws2_32.lib"_slib;
    }
    else
        asio += "pthread"_slib;

    asio.Public += "org.sw.demo.openssl.ssl"_dep;
}
