void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &sqlpp11 = s.addTarget<LibraryTarget>("rbock.sqlpp11", "0.49.0");
    sqlpp11.Source = Git("https://github.com/rbock/sqlpp11", "{M}.{m}");
    sqlpp11.Public += "pub.cppan2.demo.howardhinnant.date-2.2"_dep;
}