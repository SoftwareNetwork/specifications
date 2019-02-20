void build(Solution &s)
{
    auto &libtorrent = s.addTarget<LibraryTarget>("arvidn.libtorrent", "1.1.10");
    libtorrent += Git("https://github.com/arvidn/libtorrent", "libtorrent-{M}_{m}_{p}");

    libtorrent +=
        "ed25519/src/.*"_rr,
        "include/.*"_rr,
        "src/.*"_rr;

    libtorrent.Private += "TORRENT_BUILDING_LIBRARY"_d;
    libtorrent.Public += "TORRENT_USE_ICONV"_d;
    libtorrent.Public += "TORRENT_USE_LIBCRYPTO"_d;
    libtorrent.Public += "TORRENT_USE_OPENSSL"_d;
    if (s.Settings.TargetOS.Type == OSType::Windows)
    {
        libtorrent.Private += "BOOST_ASIO_ENABLE_CANCELIO"_d;
        libtorrent.Public += "_WIN32_WINNT=0x0601"_d;
        libtorrent.Public += "NOMINMAX"_d;
        libtorrent.Public += "iphlpapi.lib"_slib;
    }
    libtorrent.Private += sw::Shared, "TORRENT_BUILDING_SHARED"_d;

    libtorrent.Public += "org.sw.demo.boost.asio-1"_dep;
    libtorrent.Public += "org.sw.demo.gnu.iconv.libiconv-1"_dep;
    libtorrent.Public += "org.sw.demo.boost.crc-1"_dep;
    libtorrent.Public += "org.sw.demo.boost.date_time-1"_dep;
    libtorrent.Public += "org.sw.demo.openssl.ssl-1.*.*.*"_dep;
    libtorrent.Public += "org.sw.demo.boost.scope_exit-1"_dep;
    libtorrent.Public += "org.sw.demo.boost.system-1"_dep;
    libtorrent.Public += "org.sw.demo.boost.multiprecision-1"_dep;
}
