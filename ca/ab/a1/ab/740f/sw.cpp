void build(Solution &sln)
{
    auto &s = sln.addDirectory("demo");
    auto &date = s.addTarget<LibraryTarget>("howardhinnant.date.date", "2.2.0");
    date.Source = Git("https://github.com/HowardHinnant/date", "v{M}.{m}");

    date +=
        "date.h";

}