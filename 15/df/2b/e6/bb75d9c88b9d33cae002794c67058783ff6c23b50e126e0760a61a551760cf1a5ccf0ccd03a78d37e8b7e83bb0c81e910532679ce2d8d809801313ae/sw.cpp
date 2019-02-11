void build(Solution &s)
{
    auto &c = s.addTarget<StaticLibraryTarget>("Erroneous1.sqlpp11_connector_odbc", "0.5");
    c += Git("https://github.com/Erroneous1/sqlpp11-connector-odbc", "v{M}.{m:02}");
    c.Public += "org.sw.demo.rbock.sqlpp11-0"_dep;
    if (s.Settings.TargetOS.is(OSType::Windows))
        c += "odbc32.lib"_slib;
}
