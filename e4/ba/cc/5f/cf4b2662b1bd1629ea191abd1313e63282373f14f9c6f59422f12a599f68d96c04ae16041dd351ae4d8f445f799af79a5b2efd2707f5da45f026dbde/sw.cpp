void build(Solution &s)
{
    auto &t = s.addStaticLibrary("litehtml", "master");
    t += Git("https://github.com/litehtml/litehtml");

    t.Public += "include"_idir;
    t += "include/litehtml"_idir;

    t += "include/.*"_rr;
    t += "src/.*"_r;

    t.Public += "org.sw.demo.google.gumbo_parser"_dep;
}
