void build(Solution &s)
{
    auto &t = s.addLibrary("zmartzone.liboauth2", "1.4.0");
    t += Git("https://github.com/zmartzone/liboauth2", "v{v}");

    t += "org.sw.demo.badger.curl.libcurl"_dep;
    t += "org.sw.demo.akheron.jansson"_dep;
}
