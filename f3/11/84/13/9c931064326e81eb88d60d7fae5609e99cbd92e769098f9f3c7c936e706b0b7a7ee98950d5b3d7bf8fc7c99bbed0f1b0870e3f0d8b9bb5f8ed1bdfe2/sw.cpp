void build(Solution &s)
{
	auto &sqlpp11 = s.addTarget<LibraryTarget>("rbock.sqlpp11", "master");
	sqlpp11 += Git("https://github.com/rbock/sqlpp11", "", "{v}");
	sqlpp11.Public += "org.sw.demo.howardhinnant.date.date-2"_dep;
	sqlpp11.replaceInFileOnce("include/sqlpp11/char_sequence.h", "Input", "s");
	sqlpp11.replaceInFileOnce("include/sqlpp11/char_sequence.h", "(&s)[N]", "*s");
}
