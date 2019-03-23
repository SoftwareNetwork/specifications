void build(Solution &s)
{
    auto &sqlpp11 = s.addTarget<LibraryTarget>("rbock.sqlpp11", "develop");
    sqlpp11 += Git("https://github.com/rbock/sqlpp11");
    sqlpp11.Public += "org.sw.demo.howardhinnant.date.date-2"_dep;
    sqlpp11.replaceInFileOnce("include/sqlpp11/char_sequence.h", "Input", "s");
    sqlpp11.replaceInFileOnce("include/sqlpp11/char_sequence.h", "(&s)[N]", "*s");

    sqlpp11.replaceInFileOnce("include/sqlpp11/select_column_list.h", "-> result_t<decltype(db.select(composite)), _result_row_t<Db>>", "//->  result_t<decltype(db.select(composite)), _result_row_t<Db>>");
    sqlpp11.replaceInFileOnce("include/sqlpp11/select_column_list.h", "return {db.select(composite), get_dynamic_names()};", "return result_t<decltype(db.select(composite)), _result_row_t<Db>>{db.select(composite), get_dynamic_names()};");

    sqlpp11.replaceInFileOnce("include/sqlpp11/select_column_list.h", "-> result_t<decltype(db.select(std::declval<_statement_t>())), _result_row_t<Db>>", "//->  result_t<decltype(db.select(std::declval<_statement_t>())), _result_row_t<Db>>");
    sqlpp11.replaceInFileOnce("include/sqlpp11/select_column_list.h", "return {db.select(_get_statement()), get_dynamic_names()};", "return result_t<decltype(db.select(std::declval<_statement_t>())), _result_row_t<Db>>{db.select(_get_statement()), get_dynamic_names()};");
}
