void build(Solution &s)
{
    auto &p = s.addTarget<Library>("spotify.NFHTTP", "master");
    p += Git("https://github.com/spotify/NFHTTP");

    p += "org.sw.demo.nlohmann.json"_dep;
    p += "org.sw.demo.badger.curl.libcurl"_dep;
    p += "org.sw.demo.sqlite3"_dep;
}
