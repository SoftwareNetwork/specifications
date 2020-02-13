void build(Solution &s)
{
    auto &sqlpp11_connector_sqlite3 = s.addTarget<StaticLibraryTarget>("rbock.sqlpp11_connector_sqlite3", "master");
    sqlpp11_connector_sqlite3 += Git("https://github.com/rbock/sqlpp11-connector-sqlite3");
    sqlpp11_connector_sqlite3.Public += "org.sw.demo.rbock.sqlpp11-develop"_dep;
    sqlpp11_connector_sqlite3.Public += "org.sw.demo.sqlite3"_dep;
}
