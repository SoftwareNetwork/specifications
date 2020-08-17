void build(Solution &s)
{
    auto &asio = s.addTarget<LibraryTarget>("chriskohlhoff.asio", "1.17.0");
    asio += Git("https://github.com/chriskohlhoff/asio", "asio-{M}-{m}-{p}");

    asio.setRootDirectory("asio");
    asio += "include/.*\\.[hi]pp"_rr;
    asio.Public += "ASIO_STANDALONE"_d;
}
