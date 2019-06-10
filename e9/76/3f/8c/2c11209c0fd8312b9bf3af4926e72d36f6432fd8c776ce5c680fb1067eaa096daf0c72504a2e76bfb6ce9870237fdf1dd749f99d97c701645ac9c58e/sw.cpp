void build(Solution &s)
{
    auto &sqlite3pp = s.addTarget<StaticLibraryTarget>("iwongu.sqlite3pp", "1.0.8");
    sqlite3pp += Git("https://github.com/iwongu/sqlite3pp", "v{v}");
    sqlite3pp.Public += "org.sw.demo.sqlite3-3"_dep;
}
