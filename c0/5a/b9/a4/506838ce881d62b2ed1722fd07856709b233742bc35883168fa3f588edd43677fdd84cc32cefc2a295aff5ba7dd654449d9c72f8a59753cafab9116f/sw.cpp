void build(Solution &s)
{
    auto &t = s.addLibrary("zmartzone.liboauth2", "1.4.5.1");
    t += Git("https://github.com/zmartzone/liboauth2", "v{M}.{m}.{p}{to}");

    t += "org.sw.demo.badger.curl.libcurl"_dep;
    t += "org.sw.demo.akheron.jansson"_dep;
}
