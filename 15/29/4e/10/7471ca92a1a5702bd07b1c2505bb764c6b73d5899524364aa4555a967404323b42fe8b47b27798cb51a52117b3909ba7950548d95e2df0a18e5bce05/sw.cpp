void build(Solution &s)
{
    auto &date = s.addTarget<StaticLibraryTarget>("HowardHinnant.date.date", "2.4.1");
    date += Git("https://github.com/HowardHinnant/date", "v{v}");
    date += "include/date/date.h";
}