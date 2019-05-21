void build(Solution &s)
{
    auto &p = s.addProject("howardhinnant.date", "master");
    p += Git("https://github.com/HowardHinnant/date");

    auto &date = p.addTarget<StaticLibraryTarget>("date");
    date += "include/date/date.h";

    auto &date_full = p.addTarget<StaticLibraryTarget>("date_full");
}
