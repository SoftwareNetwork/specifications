void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &sqlpp11_connector_sqlite3 = s.addTarget<StaticLibraryTarget>("rbock.sqlpp11_connector_sqlite3", "0.24.0");
    sqlpp11_connector_sqlite3.Source = Git("https://github.com/rbock/sqlpp11-connector-sqlite3", "{M}.{m}");
    sqlpp11_connector_sqlite3.Public += "pub.cppan2.demo.rbock.sqlpp11-0"_dep;
    sqlpp11_connector_sqlite3.Public += "pub.cppan2.demo.sqlite3-3"_dep;
}
