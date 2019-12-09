void build(Solution &s)
{
    auto &t = s.addStaticLibrary("litehtml", "master");
    t += Git("https://github.com/litehtml/litehtml");

    t += "include/.*"_rr;
    t += "src/.*"_r;

    t.Public += "LITEHTML_UTF8"_def;

    t.Public += "include"_idir;
    t += "include/litehtml"_idir;

    t.Public += "org.sw.demo.google.gumbo_parser"_dep;
}
