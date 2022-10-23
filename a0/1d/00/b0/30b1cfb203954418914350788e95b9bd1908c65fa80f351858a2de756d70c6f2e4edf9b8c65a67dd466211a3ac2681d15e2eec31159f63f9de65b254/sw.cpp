void build(Solution &s)
{
    auto &t = s.addTarget<LibraryTarget>("arvidn.libtorrent", "2.0.8");
    t += Git("https://github.com/arvidn/libtorrent", "v{v}");

    t += cpp14;

    t += "TORRENT_BUILDING_LIBRARY"_d;
    t += sw::Shared, "TORRENT_BUILDING_SHARED"_d;

    t += "BOOST_ASIO_HAS_STD_CHRONO"_d;
    t += "BOOST_ASIO_ENABLE_CANCELIO"_d;
    t += "BOOST_ASIO_NO_DEPRECATED"_d;

    t.Public += "TORRENT_USE_LIBCRYPTO"_d;
    //t.Public += "TORRENT_USE_SSL"_d;
    t.Public += "TORRENT_USE_OPENSSL"_d;
    t.Public += "TORRENT_SSL_PEERS"_d;
    //t.Public += "OPENSSL_NO_SSL2"_d;

    if (t.getBuildSettings().TargetOS.Type == OSType::Windows)
    {
        t.Public += "_WIN32_WINNT=0x0600"_d;
        t.Public += "NOMINMAX"_d;
        t.Public += "WIN32_LEAN_AND_MEAN"_d;
        t.Public += "iphlpapi.lib"_slib;
        t.CompileOptions.push_back("/bigobj");
    }

    t.Public += "org.sw.demo.boost.asio"_dep;
    t.Public += "org.sw.demo.boost.crc"_dep;
    t.Public += "org.sw.demo.boost.date_time"_dep;
    t.Public += "org.sw.demo.boost.scope_exit"_dep;
    t.Public += "org.sw.demo.boost.system"_dep;
    t.Public += "org.sw.demo.boost.logic"_dep;
    t.Public += "org.sw.demo.boost.multi_index"_dep;
    t.Public += "org.sw.demo.boost.multiprecision"_dep;
    t.Public += "org.sw.demo.boost.variant"_dep;

    t.Public += "org.sw.demo.openssl.ssl"_dep;
    t.Public += "org.sw.demo.arvidn.try_signal-master"_dep;
}
