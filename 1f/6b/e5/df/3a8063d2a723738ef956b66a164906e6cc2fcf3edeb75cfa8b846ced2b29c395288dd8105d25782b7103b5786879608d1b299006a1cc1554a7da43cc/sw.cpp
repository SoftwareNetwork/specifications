void build(Solution &s)
{
    auto &libtorrent = s.addTarget<LibraryTarget>("arvidn.libtorrent", "1.2.14");
    libtorrent += Git("https://github.com/arvidn/libtorrent", "v{v}");

    libtorrent +=
        "ed25519/src/.*"_rr,
        "include/.*"_rr,
        "src/.*"_rr;

    libtorrent.Private += "TORRENT_BUILDING_LIBRARY"_d;
    libtorrent.Public += "TORRENT_USE_ICONV"_d;
    libtorrent.Public += "TORRENT_USE_LIBCRYPTO"_d;
    libtorrent.Public += "TORRENT_USE_OPENSSL"_d;
    if (libtorrent.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        libtorrent.Private += "BOOST_ASIO_ENABLE_CANCELIO"_d;
        libtorrent.Public += "_WIN32_WINNT=0x0601"_d;
        libtorrent.Public += "NOMINMAX"_d;
        libtorrent.Public += "WIN32_LEAN_AND_MEAN"_d;
        libtorrent.Public += "iphlpapi.lib"_slib;
    }
    libtorrent.Private += sw::Shared, "TORRENT_BUILDING_SHARED"_d;

    libtorrent.Public += "org.sw.demo.boost.asio"_dep;
    libtorrent.Public += "org.sw.demo.gnu.iconv.libiconv"_dep;
    libtorrent.Public += "org.sw.demo.boost.crc"_dep;
    libtorrent.Public += "org.sw.demo.boost.date_time"_dep;
    libtorrent.Public += "org.sw.demo.openssl.ssl"_dep;
    libtorrent.Public += "org.sw.demo.boost.scope_exit"_dep;
    libtorrent.Public += "org.sw.demo.boost.system"_dep;
    libtorrent.Public += "org.sw.demo.boost.multiprecision"_dep;
}
