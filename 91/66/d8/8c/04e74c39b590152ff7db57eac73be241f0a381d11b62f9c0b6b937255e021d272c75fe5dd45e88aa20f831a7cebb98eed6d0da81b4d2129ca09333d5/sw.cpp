void build(Solution &s)
{
    auto &t = s.addStaticLibrary("whoshuu.cpr", "1.6.2");
    t += Git("https://github.com/whoshuu/cpr");

    t += cpp17;

    t += "include/.*"_rr;
    t += "cpr/.*"_rr;

    t.Public += "org.sw.demo.badger.curl.libcurl"_dep;
}
