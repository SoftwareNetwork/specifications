void build(Solution &s)
{
    auto &t = s.addLibrary("kimkulling.openddl_parser", "master");
    t += Git("https://github.com/kimkulling/openddl-parser");

    t += "include/.*"_rr;
    t += "code/.*"_rr;

    t.Public += "include"_idir;
    t += "code"_idir;

    t += "OPENDDLPARSER_BUILD"_def;
    t += sw::Static, "OPENDDL_STATIC_LIBARY"_def;
}
