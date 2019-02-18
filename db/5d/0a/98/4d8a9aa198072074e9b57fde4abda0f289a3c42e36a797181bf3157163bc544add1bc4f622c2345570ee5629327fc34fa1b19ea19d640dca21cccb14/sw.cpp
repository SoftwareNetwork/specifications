void build(Solution &s)
{
    auto &sqlpp11 = s.addTarget<LibraryTarget>("rbock.sqlpp11", "0.53.0");
    sqlpp11 += Git("https://github.com/rbock/sqlpp11", "{M}.{m}");
    sqlpp11.Public += "org.sw.demo.howardhinnant.date.date-2"_dep;
    sqlpp11.replaceInFileOnce("include/sqlpp11/char_sequence.h", "Input", "s");
    sqlpp11.replaceInFileOnce("include/sqlpp11/char_sequence.h", "(&s)[N]", "*s");
}