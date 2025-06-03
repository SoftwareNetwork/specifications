void build(Solution &s)
{
    auto &t = s.addTarget<Library>("beltoforion.muparser", "2.3.5");
    t += Git("https://github.com/beltoforion/muparser", "v{v}");

    t.Public += sw::Static, "MUPARSER_STATIC"_def;
    t += "MUPARSERLIB_EXPORTS"_def;
}
