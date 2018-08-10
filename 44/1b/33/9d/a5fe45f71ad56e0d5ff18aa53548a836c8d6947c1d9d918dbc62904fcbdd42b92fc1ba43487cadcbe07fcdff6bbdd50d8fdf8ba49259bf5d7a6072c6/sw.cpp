void build(Solution &s)
{
    auto &sqlpp11_connector_sqlite3 = s.addTarget<StaticLibraryTarget>("rbock.sqlpp11_connector_sqlite3", "0.29");
    sqlpp11_connector_sqlite3 += Git("https://github.com/rbock/sqlpp11-connector-sqlite3", "{M}.{m}");
    sqlpp11_connector_sqlite3.Public += "org.sw.demo.rbock.sqlpp11-develop"_dep;
    sqlpp11_connector_sqlite3.Public += "org.sw.demo.sqlite3-3"_dep;
}
