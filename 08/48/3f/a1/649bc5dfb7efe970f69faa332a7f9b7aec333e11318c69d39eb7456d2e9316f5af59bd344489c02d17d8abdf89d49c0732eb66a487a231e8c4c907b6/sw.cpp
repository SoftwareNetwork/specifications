void build(Solution &s)
{
    auto &date = s.addTarget<StaticLibraryTarget>("howardhinnant.date.date_full", "2.4.1");
    date += Git("https://github.com/HowardHinnant/date", "v{v}");
}
