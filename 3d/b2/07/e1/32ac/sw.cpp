void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &sqlpp11 = s.addTarget<LibraryTarget>("rbock.sqlpp11", "develop");
    sqlpp11.Source = Git("https://github.com/rbock/sqlpp11", "", "{v}");
    sqlpp11.Public += "pub.cppan2.demo.howardhinnant.date-2.2"_dep;
}