void build(Solution &s)
{
    auto &cpprestsdk = s.addTarget<LibraryTarget>("microsoft.cpprestsdk", "2.10.13");
    cpprestsdk += Git("https://github.com/Microsoft/cpprestsdk", "v{v}");
    cpprestsdk.setRootDirectory("Release");

    cpprestsdk +=
        "include/.*"_rr,
        "src/http/.*"_rr,
        "src/json/.*"_rr,
        "src/pch/.*"_rr,
        "src/pplx/.*"_rr,
        "src/streams/.*"_rr,
        "src/uri/.*"_rr,
        "src/utilities/.*\\.cpp"_rr,
        "src/websockets/.*"_rr;

    cpprestsdk +=
        "src/pch"_id;

    cpprestsdk.Public +=
        "include"_id;

    cpprestsdk += "CPPREST_NO_SSL_LEAK_SUPPRESS"_d;
    cpprestsdk += "NOMINMAX"_d;
    cpprestsdk += "SSL_R_SHORT_READ=boost::asio::ssl::error::stream_truncated"_d;
    if (cpprestsdk.getSettings().TargetOS.Type == OSType::Windows)
    {
        cpprestsdk.Public += "bcrypt.lib"_lib;
        cpprestsdk.Public += "httpapi.lib"_lib;
        cpprestsdk.Public += "winhttp.lib"_lib;
    }
    cpprestsdk += sw::Shared, "_ASYNCRT_EXPORT"_d;
    cpprestsdk += sw::Shared, "_PPLX_EXPORT"_d;
    cpprestsdk.Public += sw::Static, "_NO_ASYNCRTIMP"_d;
    cpprestsdk.Public += sw::Static, "_NO_PPLXIMP"_d;

    cpprestsdk.Public += "org.sw.demo.boost.asio-1"_dep;
    cpprestsdk.Public += "org.sw.demo.boost.date_time-1"_dep;
    cpprestsdk.Public += "org.sw.demo.boost.system-1"_dep;
    cpprestsdk.Public += "org.sw.demo.zaphoyd.websocketpp"_dep;
    cpprestsdk.Public += "org.sw.demo.openssl.ssl-1"_dep;
    cpprestsdk.Public += "org.sw.demo.madler.zlib-1"_dep;
}
